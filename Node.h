//header file for binary tree nodes

#ifndef NODE
#define NODE

class Node {
public:
    Node(int _data); //constructor, requires int to store, and sets child nodes to NULL to start
    ~Node(); //default destructor

    void setNext(Node* _next, bool lside); //sets the left or right child node which is gotten from getNext

    int getData(); //get the int associated with this node
    Node* getNext(bool lside); //get the left or right node based on the bool value passed
private:
    Node* left; //the child nodes that goes after this one in the tree, they default to NULL in the constructor
    Node* right;
    int data; //the int stored by this node
};
#endif