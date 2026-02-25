#ifndef RBTREE
#define RBTREE

#include "Node.h"

class RBTree {
public:

private:
    void rotateNode(Node*& node, bool right);
    void balanceInsertion(Node*& current);
    void balanceDeletion(Node*& recent);

    Node*& getSuccessor(Node* current);

    void addInt(Node*& current, int theint);
    void removeNode(Node*& current, int theint);
    void removeInt(Node*& root);

    Node* findPath(Node* current, int theint, string& path, const string& prefix, bool right = false, bool root = false);
    void printNode(Node* current, const string prefix = "", bool right = false, bool root = false);
    
    Node* const NIL;
    Node* root;
};