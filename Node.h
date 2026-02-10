//header file for binary tree nodes

#ifndef NODE
#define NODE

#include <cstddef>

class Node {
public:
    Node(int _data); //constructor, requires int to store, and sets child nodes to NULL to start
    ~Node(); //default destructor

    void setNext(Node* _next, bool rside); //sets the left or right child node which is gotten from getNext
    void increment(); //in or decrement the amount of the int stored, called by adding and deleting functions respectively
    void decrement();

    int getData(); //get the int associated with this node
    size_t getAmount(); //get how many of the int this node has

    Node*& getNext(bool rside); //get the left or right node based on the bool value passed, return by reference so it's easier to set when adding numbers
private:
    Node* left; //the child nodes that goes after this one in the tree, they default to NULL in the constructor
    Node* right;
    int data; //the int stored by this node
    size_t amount; //how many of this int is stored, in order to handle duplicates, 1 by default
};
#endif