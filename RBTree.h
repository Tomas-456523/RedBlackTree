//header file for the red-black tree data structure

#ifndef RBTREE
#define RBTREE

#include <vector>
#include "Node.h"

struct RBStatus { //contains checks for if the RB tree is valid
    bool bstOrder; //is the tree correctly sorted?
    bool rootBlack; //is the root black?
    bool nilBlack; //is NIL black?
    bool redChildBlack; //does every red node have a black child?
    bool blackHeightEqual; //is the black height equal on every path?
    size_t blackHeight; //what the black height is

    std::vector<int> orderviolators; //tracks which ints are improperly sorted
    std::vector<int> drviolators; //tracks which ints violate no double red rule to make it easier to tell where the issue is
    std::vector<int> bhviolators; //also tracks what ints violate black height

    //assumes the tree is valid and the black height is 0 by default; values must be set in the verification function
    RBStatus() : bstOrder(true), rootBlack(true), nilBlack(true), redChildBlack(true), blackHeightEqual(true), blackHeight(0) {}
};

class RBTree {
public:
    RBTree(); //initializes NIL and the root
    ~RBTree(); //destroys all the nodes including NIL on deletion

    void insert(int theint); //called by main to insert the given int into the tree
    int remove(int theint); //called by main to remove the given int from the tree, and returns how much is left or if removal was successful (unsuccessful if negative)
    int erase(int theint); //called by main to remove all of the given int from the tree (if there's multiple of the int)
    int clear(); //called by main to reset the tree and delete all the nodes, leaving only a NIL root, and returns how many ints were in the tree

    long double getAverage(); //called by main to get the average of all ints in the tree
    Node* search(int query, std::string& visual); //gets the node with the specified int, and builds a visual of the path to it

    RBStatus verify(); //returns a bunch of checks to verify that the tree follows all the RB tree properties

    bool empty(); //gets if there's no nodes in the tree

    friend std::ostream& operator<<(std::ostream& out, const RBTree& tree); //<< overload that makes a visual of the tree be printed when the tree itself is couted

private:
    Node* const NIL; //the sentinel NIL, used instead of NULL pointers for convenience
    Node* root; //the root of the tree, starts as NIL

    Node* initNode(int data, Node* parent); //initializes a node with NIL instead of NULL pointers, and sets the parent to the given parent
    void rotateNode(Node* _node, bool right); //rotates the tree from _node in the direction passed. For example, a rotation right would be make the shape go from < to >
    void balanceInsertion(Node* current); //balances the tree after inserting a node to make it follow the RB tree rules 
    void balanceDeletion(Node* current); //balances the tree after deleting a node

    Node* getSuccessor(Node* current); //gets the next-smallest value node in the tree after the given one
    Node*& relink(Node* node); //gets a fresh pointer reference to the given node in a way that makes it reference its parent's pointer to it
    void transplant(Node* repl, Node* dest); //puts the replacement node in the destination's spot in the tree and links everything accordingly

    void addInt(Node*& current, Node* parent, int theint); //recursively finds where to put the specified int, then makes a new node for it in the found address (or increments the amount if it's already there) and calls the tree balancing process
    int removeNode(Node*& current, Node* parent, int theint, bool erase); //recursively finds the node with the integer to remove, decrements it, and deletes it if the amount reaches 0. Then adjusts the tree accordingly. Also returns how much of the int is left

    void censeTree(Node* current, long long& sum, size_t& population); //recursively finds the sum of all ints in the tree and the amount of ints by adding up the passed counters
    bool ansiAllowed() const; //get if ansi escape codes are allowed, used by formatNode to check if it should add the codes
    std::string formatNode(Node* node, const std::string& prefix) const; //formats the node using the given prefix into a strings, and decides whether to set the text's color or just add a color label depending on if ansi escape codes are supported
    void deleteAll(Node* current, int* tally = NULL); //recursively deletes every node, called in the destructor, and optionally tallies how many ints were deleted
    size_t checkNode(Node* current, RBStatus& status, int min, int max); //recursed through the tree and makes sure evry node follows the black height and no double red rules, and also finds the black height from the root in the end, and checks if it follows BST order
    
    Node* findPath(Node* current, int theint, std::string& path, const std::string& prefix, bool right = false, bool root = false); //finds the node with the given int and builds a visual of the path to it in the given string
    void printNode(std::ostream& out, Node* current, const std::string& prefix = "", bool right = false, bool root = false) const; //recursively prints a visual of the tree to the given ostream
};

#endif