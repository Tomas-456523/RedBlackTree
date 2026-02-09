//implementation file for binary tree nodes

#include "Node.h"

Node::Node(int _data) { //create the node and assign it the given int
    data = _data;
    left = right = 0; //child nodes start as 0 which is equal to NULL except NULL requires #including stuff
}
void Node::setNext(Node* _next, bool lside) { //set the node that goes after this one and on which side
    Node*& next = lside ? left : right; //get if we're editing the left or right child; left if true, right if false
    next = _next; //set the child node to the given one
}
int Node::getData() { //get the int associated with this node
    return data;
}
Node* Node::getNext(bool lside) { //get the left or right child node
    return lside ? left : right; //return left child if true was passed, right if false
}
Node::~Node() { //default destructor, does nothing

}