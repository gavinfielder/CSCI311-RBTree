/**
 * @file rbtree.cpp An implementation of a red-black tree storing string
 *                  key with string values.
 * 
 * @author Gavin Fielder
 * @date 11/13/2017
 **/


#include <iostream>
#include <iomanip>
#include <vector>
#include "rbtree.h"

using std::cout;
using std::setw;
using std::endl;
using std::vector;

/**
 * Default constructor for Node. Used only by nil
 */
RBTree::Node::Node() {
    key = new string("nil");
    value = new string("");
    color = 'B';
    left = this;
    right = this;
    parent = this;
}

/**
 * Normal constructor for Node. Used for all internal nodes
 *
 * @param  key  the key of the new node
 * @param  value  the value of the new node
 */
RBTree::Node::Node(const string& key_in, const string& value_in) {
    key = new string(key_in);
    value = new string(value_in);
}

/**
 * Destructor for Node
 */
RBTree::Node::~Node() {
    delete key;
    delete value;
}

/**
 * Constructor for RBTree
 */
RBTree::RBTree() {
    nil = new Node();
    root = nil;
}

/**
 * Destructor for RBTree
 */
RBTree::~RBTree() {
    deallocateRecursive(root);
    delete nil;
}

/**
 * Deallocates entire tree. Recursive helper function for ~RBTree
 *
 * @param  x  the node to delete
 * 
 * @return  true if no more children at this node, false if has children
 */
void RBTree::deallocateRecursive(Node* x) {
    if (x != nil) { //x == nil is base case
        deallocateRecursive(x->left);
        deallocateRecursive(x->right);
        //all children of x have been deleted
        delete x; //now delete me
    }
}

/**
 * Inserts a new node
 *
 * @param  key    the key of the new node
 * @param  value  the data of the new node
 */
void RBTree::rbInsert(const string& key, const string& value) {
    //create new node
    Node* newNode = new Node(key, value);
    //insert new node
    rbInsert(newNode);
}

/**
 * Inserts a new node
 *
 * @param  z  the node to insert
 */
void RBTree::rbInsert(Node* z) {
    Node* y = nil;
    Node* x = root;
    while (x != nil) {
        y = x;
        if (*(z->key) < *(x->key)) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == nil) {
        root = z;
    }
    else if (*(z->key) < *(y->key)) {
        y->left = z;
    }
    else {
        y->right = z;
    }
    z->left = nil;
    z->right = nil;
    z->color = 'R';
    rbInsertFixup(z);
}

/**
 * Maintains red-black tree properties after insert
 *
 * @param  z  the node to fixup
 */
void RBTree::rbInsertFixup(Node* z) {
    Node* y;
    while (z->parent->color == 'R') {
        if (z->parent == z->parent->parent->left) { //parent is a left child
            y = z->parent->parent->right; //set y to be uncle of z
            if (y->color == 'R') { //if uncle is red                  //Case 1
                z->parent->color = 'B';     //parent is black         //Case 1
                y->color = 'B';             //uncle is black          //Case 1
                z->parent->parent->color = 'R'; //grandparent is red  //Case 1
                z = z->parent->parent; //move up z to point to grandparent   1
            }
            else { //uncle is black
                if (z == z->parent->right) { //if z is a right child  //Case 2
                    z = z->parent; //move z to point to parent        //Case 2
                    leftRotate(z);                                    //Case 2
                }
                z->parent->color = 'B'; //parent is black             //Case 3
                z->parent->parent->color = 'R'; //grandparent is red  //Case 3
                rightRotate(z->parent->parent); //right rotate on grndparent 3
            }
        }
        else { //parent is a right child
            y = z->parent->parent->left; //set y to be uncle of z
            if (y->color == 'R') { //if uncle is red                  //Case 4
                z->parent->color = 'B'; //parent is black             //Case 4
                y->color = 'B';         //uncle is black              //Case 4
                z->parent->parent->color = 'R'; //grandparent is red  //Case 4
                z = z->parent->parent; //move up z to point to grandparent   4
            }
            else { //uncle is black
                if (z == z->parent->left) { //if z is a left child    //Case 5
                    z = z->parent; //move up z to point to parent     //Case 5
                    rightRotate(z);                                   //Case 5
                }
                z->parent->color = 'B'; //parent is black             //Case 6
                z->parent->parent->color = 'R'; //grandparent is red  //Case 6
                leftRotate(z->parent->parent); //left rotate on grandparent  6
            }
        }
    }
    root->color = 'B';
}

/**
 * Deletes all instances of matching key-value pair
 *
 * @param  key    the key of the data to delete
 * @param  value  the value of the data to delete
 */
void RBTree::rbDelete(const string& key, const string& value) {
    //Get a vector of pointers to all matching nodes
    vector<Node*> results;
    Node* x = rbTreeSearch(root, key); //gets the first match, or nil
    if (x != nil) { //we have at least one result
        if (*(x->value) == value) 
            results.push_back(x);
        //Loop through predecessors of the first result
        Node* y = rbTreePredecessor(x);
        while ((y != nil) && (*(y->key) == key)) {
            if (*(y->value) == value)
                results.push_back(y);
            y = rbTreePredecessor(y);
        }
        //Loop through successors of the first result
        y = rbTreeSuccessor(x);
        while ((y != nil) && (*(y->key) == key)) {
            if (*(y->value) == value)
                results.push_back(y);
            y = rbTreeSuccessor(y);
        }
    }
    //Delete all these nodes
    Node* toDelete;
    while (!(results.empty())) {
        toDelete = results.back();
        rbDelete(results.back());
        results.pop_back();
        delete toDelete;
    }
}

/**
 * Deletes a node
 *
 * @param  z  the node to delete
 */
void RBTree::rbDelete(Node* z) {
    Node* y = z;
    Node* x;
    char yoc = y->color;
    if (z->left == nil) {
        x = z->right;
        rbTransplant(z, z->right);
    }
    else if (z->right == nil) {
        x = z->left;
        rbTransplant(z, z->left);
    }
    else {
        y = rbTreeMaximum(z->left);
        yoc = y->color;
        x = y->left;
        if (y->parent == z) {
            x->parent = y;
        }
        else {
            rbTransplant(y, y->left);
            y->left = z->left;
            y->left->parent = y;
        }
        rbTransplant(z,y);
        y->right = z->right;
        y->right->parent = y;
        y->color = z->color;
    }
    if (yoc == 'B') {
        rbDeleteFixup(x);
    }
}


/**
 * Maintains red-black tree properties after delete
 *
 * @param  node  the node to fixup
 */
void RBTree::rbDeleteFixup(Node* x) {
    while ((x != root) && (x->color == 'B')) {
        Node* w;
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == 'R') {
                w->color = 'B';
                x->parent->color = 'R';
                leftRotate(x->parent);
                w = x->parent->right;
            }
            if ((w->left->color == 'B') && (w->right->color == 'B')) {
                w->color = 'R';
                x = x->parent;
            }
            else {
                if (w->right->color == 'B') {
                    w->left->color = 'B';
                    w->color = 'R';
                    rightRotate(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 'B';
                w->right->color = 'B';
                leftRotate(x->parent);
                x = root;
            }
        }
        else {
            w = x->parent->left;
            if (w->color == 'R') {
                w->color = 'B';
                x->parent->color = 'R';
                rightRotate(x->parent);
                w = x->parent->left;
            }
            if ((w->right->color == 'B') && (w->left->color == 'B')) {
                w->color = 'R';
                x = x->parent;
            }
            else {
                if (w->left->color == 'B') {
                    w->right->color = 'B';
                    w->color = 'R';
                    leftRotate(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 'B';
                w->left->color = 'B';
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->color = 'B';
}

/**
 * Transplants subtree v at u's location
 *
 * @param  u  the location to transplant to
 * @param  v  the subtree root to transplant
 */
void RBTree::rbTransplant(Node* u, Node* v) {
    if (u->parent == nil)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}


/**
 * Finds all instances of a key
 *
 * @param  key  the key to find
 *
 * @return  a vector containing all the values associated with the key
 */
vector<const string*> RBTree::rbFind(const string& key) {
    vector<const string*> results;
    Node* x = rbTreeSearch(root, key); //gets the first match, or nil
    if (x != nil) { //we have at least one result
        results.push_back(x->value);
        //Loop through predecessors of the first result
        Node* y = rbTreePredecessor(x);
        while ((y != nil) && (*(y->key) == key)) {
            results.push_back(y->value);
            y = rbTreePredecessor(y);
        }
        //Loop through successors of the first result
        y = rbTreeSuccessor(x);
        while ((y != nil) && (*(y->key) == key)) {
            results.push_back(y->value);
            y = rbTreeSuccessor(y);
        }
    }
    return results;
}

/**
 * Return the node with the requested data
 * If the key is not found, returns nil.
 *
 * @param  node  the node to search
 * @param  key   the data to search for
 *
 * @return the requested node, or nil if not found
 */
RBTree::Node* RBTree::rbTreeSearch(Node* x, const string& key) {
    if ((x == nil) || (key == *(x->key))) {
        return x;
    }
    if (key < *(x->key)) {
        return rbTreeSearch(x->left, key);
    }
    else {
        return rbTreeSearch(x->right, key);
    }
}


/**
 * Returns a pointer to the node which is the minimum of the tree
 * rooted at the given node
 *
 * @param  node  the root of the tree
 *
 * @return  the minimum of the tree
 */
RBTree::Node* RBTree::rbTreeMinimum(Node* x) {
    while (x->left != nil)
        x = x->left;
    return x;
}

/**
 * Returns a pointer to the node which is the maximum of the tree
 * rooted at the given node
 *
 * @param  node  the root of the tree
 *
 * @return  the maximum of the tree
 */
RBTree::Node* RBTree::rbTreeMaximum(Node* x) {
    while (x->right != nil)
        x = x->right;
    return x;
}

/**
 * Returns a pointer to the successor of the given node
 *
 * @param  node  the node whose successor to find
 *
 * @return  the successor of the given node
 */
RBTree::Node* RBTree::rbTreeSuccessor(Node* x) {
    if (x->right != nil)
        return rbTreeMinimum(x->right);
    Node* y = x->parent;
    while (y != nil && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

/**
 * Returns a pointer to the predecessor of the given node
 *
 * @param  node  the node whose predecessor to find
 *
 * @return the predecessor of the given node
 */
RBTree::Node* RBTree::rbTreePredecessor(Node* x) {
    if (x->left != nil)
        return rbTreeMaximum(x->left);
    Node* y = x->parent;
    while (y != nil && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

/**
 * Rotate left at node
 *
 * @param  x  the node to rotate on
 */
void RBTree::leftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nil)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

/**
 * Rotate right at node
 *
 * @param  x  the node to rotate on
 */
void RBTree::rightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != nil)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nil)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

/**
 * Prints the tree
 */
void RBTree::rbPrintTree() {
    reverseInOrderPrint(root, 0);
}


/**
 * Prints the tree in reverse order
 *
 * @param  x  the node whose tree to print
 * @param  depth  tracks the depth for formatting
 */
void RBTree::reverseInOrderPrint(Node *x, int depth) {
   if ( x != nil ) {
      reverseInOrderPrint(x->right, depth+1);
      cout << setw(depth*4+4) << x->color << " ";
      cout << *(x->key) << " " << *(x->value) << endl;
      reverseInOrderPrint(x->left, depth+1);
   }
}




