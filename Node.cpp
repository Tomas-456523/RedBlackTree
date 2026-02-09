//implementation file for binary tree nodes

#include "Node.h"

Node::Node(int _data) { //create the node and assign it the given int
    data = _data;
    left = right = 0; //child nodes start as 0 which is equal to NULL except NULL requires #including stuff
}
void Node::setNext(Node* _next, bool rside) { //set the node that goes after this one and on which side
    Node*& next = rside ? right : left; //get if we're editing the left or right child; right if true, left if false, more intuitive this way since 0 is typically represented as being to the left of 1
    next = _next; //set the child node to the given one
}
int Node::getData() { //get the int associated with this node
    return data;
}
Node* Node::getNext(bool rside) { //get the left or right child node
    return rside ? right : left; //return right child if true was passed, left if false
}
Node::~Node() { //default destructor, does nothing

}