//header file for the red-black tree data structure

#ifndef RBTREE
#define RBTREE

#include "Node.h"

class RBTree {
public:
    RBTree(); //initializes NIL and the root
    ~RBTree(); //destroys all the nodes including NIL on deletion

    void insert(int theint); //called by main to insert the given int into the tree
    void remove(int theint); //called by main to remove the given int from the tree

    long double getAverage(); //called by main to get the average of all ints in the tree
    Node* getNode(int query, string& visual); //gets the node with the specified int, and builds a visual of the path to it

    friend std::ostream& operator<<(std::ostream& out, const RBTree& tree); //<< overload that makes a visual of the tree be printed when the tree itself is couted

private:
    Node* const NIL; //the sentinel NIL, used instead of NULL pointers for convenience
    Node* root; //the root of the tree, starts as NIL

    Node* initNode(int data, Node* parent = NIL); //initializes a node with NIL instead of NULL pointers, and sets the given parent
    void rotateNode(Node*& node, bool right); //rotates the tree in the direction passed. For example, a rotation right would be: a           b
    void balanceInsertion(Node*& current); //balances the tree after inserting a node to make it follow the RB tree rules        /             \ 
    void balanceDeletion(Node*& recent); //balances the tree after deleting a node                                     //       b     ---->     a
                                                                                                                       //        \             /
    Node*& getSuccessor(Node* current); //gets the next-smallest value node in the tree after the given one            //         c           c
    Node*& relink(Node* node); //gets a fresh pointer reference to the given node in a way that makes it reference its parent's pointer to it

    void addInt(Node*& current, int theint); //recursively finds where to put the specified int, then makes a new node for it in the found address (or increments the amount if it's already there) and calls the tree balancing process
    void removeNode(Node*& current, int theint); //recursively finds the node with the integer to remove, decrements it, and deletes it if the amount reaches 0. Then adjusts the tree accordingly

    //finds the node with the given int and builds a visual of the path to it in the given string
    Node* findPath(Node* current, int theint, string& path, const string& prefix, bool right = false, bool root = false);
    //recursively prints a visual of the tree to the given ostream
    void printNode(std::ostream& out, Node* current, const string prefix = "", bool right = false, bool root = false);
};