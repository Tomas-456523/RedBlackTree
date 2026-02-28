//implementation file for the red-black tree data structure

#include <iostream>
#include <limits>
#include <string>
#include <cstdio>
#include "RBTree.h"
#include "Node.h"
using namespace std;

#ifdef _WIN32 //defines stuff which ansiAllowed uses which might be different across platforms
    #include <windows.h> //get windows stuff
    #include <io.h>
    #define isatty _isatty //windows defines these with an underscore for some reason so for consistency and cleaner code I rename them to match unix
    #define fileno _fileno
#elif defined(__unix__) //defines the unix variables if it's unix instead of windows
    #include <unistd.h>
#else //if it's neither windows nor unix but some random weird thing
    int isatty(int)   { return  0;} //we don't know if ansi escape codes will be supported on the weird platform so we just set these so they always make ansiAllowed return false
    int fileno(FILE*) { return -1;} //I never use the -1 but 0 is a success on unix so for consistency I used that specifically
#endif

//get if we're allowed to use ansi escape codes, since some terminals or non-terminal outputs may not support it
bool RBTree::ansiAllowed() const {
    static bool ran = false; //if we've run this check before, tracked so we don't unnecessarily do all those checks again
    static bool allowed; //the result of the check
    if (ran) return allowed; //return the result of the check if we've done one before

    ran = true; //we're doing the check now so track that

    //isatty makses sure we're outputting to a terminal, fileno translates a FILE* (file stream object) to an int isatty can use, which might vary by os, and stdout is the output object, what cout outputs to
    if (!isatty(fileno(stdout))) return allowed = false; //return and set allowed to false if the output isn't a terminal, because non-terminals probably don't support ansi escape codes
    #ifdef _WIN32 //if we're on windows
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //windows uses handles to handle files and stuff, and we get the handle that handles output
        if (handle == INVALID_HANDLE_VALUE) return allowed = false; //we can't use ansi escape codes if the handle was something invalid
        DWORD mode = 0; //dwords are used for flags in windows, and this one is used to track handle's flags
        if (!GetConsoleMode(handle, &mode)) return allowed = false; //sets mode based on the handle, and if the mode getting failed ansi escape codes are prolly not allowed
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; //ors (if ansi escape codes are allowed) into the rest of the flags
        if (!SetConsoleMode(handle, mode)) return allowed = false; //sets the console's mode according to the flags we've gotten, and ansi escape codes are not allowed if this process fails
    #endif
    return allowed = true; //if we encountered no issues, we can indeed use ansi escape codes
}

//on construction, initializes NIL and initializes the root to NIL
RBTree::RBTree() : NIL(new Node(0)), root(NIL) {
    NIL->setNext(NIL, 0); //NIL points to itself
    NIL->setNext(NIL, 1);
    NIL->setPrev(NIL); //NIL's parent is occasionally set during balancing, but points to itself by default
    NIL->setRed(0); //NIL is black in order to follow the RB tree rules
}

//recursively delete all the nodes, branch off from current and delete all descendants then delete the current node
void RBTree::deleteAll(Node* current, int* tally) { //optionally counts how many ints were deleted, used by clear
    if (current == NIL) return; //if we've reached the end of the tree, there's nothing more to delete so just return
    deleteAll(current->getNext(0), tally); //start deleting all descendants to the left
    deleteAll(current->getNext(1), tally); //start deleting all descendants to the right
    if (tally) *tally += current->getAmount();
    delete current; //now that all the descendants are deleted, delete the current node
}

//when the RB tree is deleted, it also deletes all its nodes
RBTree::~RBTree() {
    deleteAll(root); //delete all the nodes that aren't NIL
    delete NIL; //delete NIL since it's associated with the tree instance
}

//recursively delete every node and reset the root to NIL, and return how many ints were deleted
int RBTree::clear() {
    int tally = 0; //counts how many integers were deleted in this int
    deleteAll(root, &tally); //start the recursive deletion process from the root
    root = NIL; //make the root NIL, so now the tree is how it started
    return tally; //return the amount that was deleted!
}

//returns a fresh link to the given Node* that makes sure it specifically references the parent's pointer to it. The root doesn't get affected since it has no parent
Node*& RBTree::relink(Node* node) {
    if (node->getPrev() == NIL) return root; //if it has no parents, then it's the root so we just return that
    Node*& parent = node->getPrev(); //finds the parent of the given node
    if (node == parent->getNext(0)) return parent->getNext(0); //if the node is a left child, return the parent's reference to the left child
    return parent->getNext(1); //otherwise, it's a right child, so return the parent's reference to the right child
}

//replace the node with the replacement at node's position in the tree
void RBTree::transplant(Node* repl, Node* dest) {
    Node*& addr = relink(dest); //get the address of node so we can replace it
    addr = repl; //replace the node with the replacement
    addr->setPrev(dest->getPrev()); //update the replaced node's parent since it just came from somewhere else
}

//rotates the given node in the given direction
void RBTree::rotateNode(Node* _node, bool right) { //"right" represents if it's a right rotation or not
    Node*& node = relink(_node); //get _node as a reference to its parent's pointer to it, to simplify the logic a bit
    Node* lchild = node->getNext(!right); //node's left child, as long as you rotate the tree so that the right variable is actually pointing to the right
    Node* lrchild = lchild->getNext(right); //lchild's right child. lchild is guaranteed to not be NIL because that's how the RB tree logic works, so this is safe to call
    
    lchild->setNext(node, right); //set lchild's right child to node
    node->setNext(lrchild, !right); //set node's left child to lchild's right child
    
    if (lrchild != NIL) lrchild->setPrev(node); //we tell lrchild its parent is node now, unless it's NIL since if current in deletion is NIL, it relies on its parent not being changed, and NIL is shared
    lchild->setPrev(node->getPrev()); //set lchild's parent to node's old parent because it's in node's place now. We don't have to check if lchild is NIL because if they're both NIL they will have the same parent anyway
    node->setPrev(lchild); //lchild is now node's parent so we set that

    node = lchild; //update node's old parent to now point to lchild instead
}

//balances the tree after having inserted a node
void RBTree::balanceInsertion(Node* current) {
    while (current->getPrev()->getRed()) { //do the balancing process while the current node's parent is red, since new nodes aren't black, so they can only violate the no double red rule
        Node* parent = current->getPrev(); //get references to the parent and grandparent of the current node
        Node* gparent = parent->getPrev();
        bool pright = parent == gparent->getNext(1); //we orient the operations around if the parent is a left or right child

        Node* uncle = gparent->getNext(!pright); //the uncle is the grandparent's child on the opposite side from the parent

        if (uncle->getRed()) { //CASE 1: if the uncle is red, we push the color black down from the grandparent
            gparent->setRed(1); //make the grandparent red
            parent->setRed(0); //make the grandparent's children black
            uncle->setRed(0);
            current = gparent; //move to the grandparent to keep balancing from there
        } else if (current == parent->getNext(!pright)) { //CASE 2: if current is the pleft child, we have to rotate it to get to case 3
            current = parent; //move to the parent to keep balancing from there
            rotateNode(current, pright); //rotate the current node (the old parent) to the pright
        } else { //CASE 3: if current is the pright child, we can finally balance the subtree
            parent->setRed(0); //make the parent black and the grandparent red
            gparent->setRed(1);
            rotateNode(gparent, !pright); //rotate the grandparent to the pright to make the now black parent the parent of the red current and gparent, thus resolving the double red situation
        }
    }
    //make sure root is black
    root->setRed(0);
}

//balances the tree after having deleted a node, assumes the node that was deleted is black before this is called
void RBTree::balanceDeletion(Node* current) { //conceptually, removing a black node means we have an "extra black" which we must place somewhere, either by reaching the root or painting a red node black
    while (current != root && !current->getRed()) { //while we're not at the root and we're currently at a black node
        Node* parent = current->getPrev(); //gets the parent of the current node
        bool xright = current == parent->getNext(1); //if current is a right child, we orient the balancing around this much like in insertion. Also, it's xright instead of cright because CLRS calls it x, and cleft is something else. Also it sounds cooler

        Node* sibling = parent->getNext(!xright); //gets the sibling node on the opposite side of the parent

        if (sibling->getRed()) { //CASE 1: if the sibling is red, we need it to be black to get to the other cases
            sibling->setRed(0); //make the sibling black
            parent->setRed(1); //make the parent red
            rotateNode(parent, xright); //rotate the parent to the xright
            parent = current->getPrev(); //updates the family pointers accordingly
            sibling = parent->getNext(!xright);
        } //now we need to check the nephews! to see if we just messed their rule compliance up. If the sibling is NIL, then the nephews would also be NIL, and that would be bad except it isn't because then case 2 triggers and doesn't use nephews anyway
        Node* lnephew = sibling->getNext(!xright); //far nephew
        Node* rnephew = sibling->getNext(xright); //near nephew

        if (!lnephew->getRed() && !rnephew->getRed()) { //CASE 2: if both the nephews are black, we have to propagate the extra black upwards
            sibling->setRed(1); //make the sibling red
            current = parent; //move to the parent
            parent = current->getPrev(); //update the parent accordingly
        } else { //if one of the nephews is red
            if (!lnephew->getRed()) { //CASE 3: if only the far nephew is black, we need to reverse the nephew colors to get to case 4
                rnephew->setRed(0); //make the near nephew black
                sibling->setRed(1); //make the sibling red
                rotateNode(sibling, !xright); //rotate the sibling to the xleft
                parent = current->getPrev(); //update the family accordingly
                sibling = parent->getNext(!xright);
                lnephew = sibling->getNext(!xright);
                rnephew = sibling->getNext(xright);
            }

            //CASE 4: if the far nephew is red, which might also have been due to case 3 if the near nephew was red instead
            sibling->setRed(parent->getRed()); //make the sibling's color match the parent
            parent->setRed(0); //make the parent black
            lnephew->setRed(0); //make the far nephew black as well
            rotateNode(parent, xright); //rotate the parent to the xright
            current = root; //move to the root so that we stop balancing; tree seems balanced enough
        }
    } //set current to black because A, we ended at the root and we need to make sure the root is black
    current->setRed(0); //or B, we ended at a red node, and we need to paint it black to put the extra black here
}

//recursively find where to put the int and then put it there, or increment the duplicate counter if it's a duplicate
void RBTree::addInt(Node*& current, Node* parent, int theint) {
    if (current == NIL) { //if we've reached the leaf of the tree, we add the int here in a new node
        current = initNode(theint, parent); //create a node (and set the parent) which the parent automatically points to because getNext returns a reference
        balanceInsertion(current);
        return; //return because we just successfully added the int so there's no more process to do
    }
    int data = current->getData(); //get current's int so we don't get it twice
    if (theint < data) { //if the int < current int, it goes somewhere to the left of the current node, so keep recursing to the left
        addInt(current->getNext(0), current, theint); //pass the current node as the parent, since we're recursing to the child of current
    } else if (theint > data) { //if the int > current int, it goes somewhere to the right of the current node, so keep recursing to the right
        addInt(current->getNext(1), current, theint);
    } else { //if the int == current int, it's a duplicate so we increment the int amount in the current node
        current->increment();
    }
}

//create a node and ensure all its NULL pointers are set to NIL pointers instead
Node* RBTree::initNode(int data, Node* parent) { //the parent is NIL if nothing is passed
    Node* newnode = new Node(data);
    newnode->setNext(NIL, 0); //change the new node's child pointers from NULL to NIL
    newnode->setNext(NIL, 1);
    newnode->setPrev(parent); //set the parent to whatever was passed, which might also be NIL
    return newnode; //return the new node!
}

//basically just starts the node adding process
void RBTree::insert(int theint) { //needed because main doesn't know what the root is, so we wouldn't be able to call addInt directly
    addInt(root, NIL, theint);
}

//gets the inorder successor to the given node (the node with smallest number greater than current's), assumes right child of the passed node is not NIL
Node* RBTree::getSuccessor(Node* current) {
    current = current->getNext(1); //start finding the successor one to the right, since all numbers to the right are greater than current's
    while (current->getNext(0) != NIL) { //go to the left until we can't, since going to the left gives a smaller number, and we want the smallest number from here
        current = current->getNext(0);
    } //return the current successor because we've left the for loop meaning we've reached the leftmost and smallest number on the right branch of the passed node
    return current;
}

//recursively finde the int to delete in the tree, and then delete it and reorganize the tree
int RBTree::removeNode(Node*& current, Node* parent, int theint, bool erase) { //erase is if we remove all of the int from the tree, as opposed to just one
    if (current == NIL) { //if we ran out of nodes, that means the int isn't in the tree, so return an error value
        return -1; //there can't be negative ints in the tree so -1 is the error value
    } //recurse to the left or right branch, depending on if it's < or > the current data, since that's how BSTs work
    if (theint < current->getData()) { //keep recursing to the left, and return whatever result is found there
        return removeNode(current->getNext(0), current, theint, erase);
    } else if (theint > current->getData()) { //keep recursing to the right, and return whatever result is found there
        return removeNode(current->getNext(1), current, theint, erase);
    }
    current->decrement(); //decrease amount of the int stored
    if (!erase && current->getAmount()) { //if there's still some of the int left in the node (and we haven't specified to just remove all of the int), we don't delete anything and return how much are left
        return current->getAmount();
    } //store current node as it was before all the processing so we can delete it later
    Node* old = current;
    bool redremoved = old->getRed(); //we only have to balance the tree if the node we remove is black, so we track if it's red here
    Node* repl = NIL; //the node that will replace whichever node is removed from the tree, which is old by default but can be the successor in the two-child case, NIL by default
    if (old->getNext(0) == NIL) { //if the left child is NIL, pull the right child into the current node of the tree. This presevres balance because both of current's children will also be greater than current's parent if current is a right child, and vice versa, which is what BSTs are supposed to do
        repl = old->getNext(1); //the right child might also be NIL, but that's fine cause if it has no children, we need to just make it NIL anyway                                                            ^
        transplant(repl, old); //transplants the replacement into the old node's spot                                                                                                                           |
    } else if (old->getNext(1) == NIL) { //if the left child is not NIL, but the right child is, pull the left into its parent's slot. This also preserves balance for the vice versa reason of this comment ___|
        repl = old->getNext(0); //left child will never be NIL here
        transplant(repl, old); //transplants the replacement into the old node's spot
    } else { //if both children are not NIL, so current has two children
        Node* successor = getSuccessor(old); //get the successor of the current node which we will move to where old is
        redremoved = successor->getRed(); //since we're removing the successor now, we update the color value of the node to remove
        repl = successor->getNext(1); //we want to replace the successor with the right child, so we don't abandon it. It might be NIL, but that's fine cause in that case NIL would fill in the gap created anyway
        if (successor->getPrev() == old) { //if we're removing old's direct right child, we have to make sure the replacement node knows the successor is its parent now
            repl->setPrev(successor); 
        } else { //if we're not removing old's direct right child, we have to do some extra stuff since that means the successor's right child might not be NIL, since we did some going left in addition to going right when getting the successor /\/ vs /
            transplant(repl, successor); //transplant the successor's replacement into it to fill in the gap created by moving the successor
            successor->setNext(old->getNext(1), 1); //set the successor's right child to old's right child because we're moving it to old's position
            successor->getNext(1)->setPrev(successor); //updates old's old right child to the successor since it's the parent now
        }
        transplant(successor, old); //moves the successor to where old is because it's succeeding it
        successor->setNext(old->getNext(0), 0); //update the old left child to be connected to the successor
        successor->getNext(0)->setPrev(successor);
        successor->setRed(old->getRed()); //update the successor's color to match what the old node was
    }
    delete old; //delete the old node because that's what we're doing all this for
    if (!redremoved) balanceDeletion(repl); //if we removed a black node, we have to rebalance the tree cause we're probably violating the black height rule now, unless we removed the last int
    return 0; //returns confirmation that there's no more of that int left
}

//used by main to tell the tree to remove this int and check if it was successfully removed; starts the int removal process
int RBTree::remove(int theint) { //must be its own seperate function instead of just calling removeNode because main doesn't have access to the root
    return removeNode(root, NIL, theint, false);
}

//same as remove, except it removes all of the given int from the tree
int RBTree::erase(int theint) { //eg. if we have 3 of 10 in the tree, erase would remove all three 10s
    return removeNode(root, NIL, theint, true);
}

//formats the given node's data with its respective color and the given prefix as a string
string RBTree::formatNode(Node* node, const string& prefix) const {
    bool ansiworks = ansiAllowed(); //gets if ansi escape codes are supported
    string output = "\n" + prefix; //starts building the string with the prefix in a new line
    if (ansiworks) { //if ansi escape codes are supported, we set the color to either bold red or white, corresponding to the node's color
        output += (node->getRed() ? "\033[1;31m" : "\033[1m");
    } //prints the data, colored or not
    output += to_string(node->getData());
    if (ansiworks) { //reset the color if the codes were supported, so we go back to printing regular white text
        output += "\033[0m";
    } else { //if the codes aren't supported, we print a label so the information isn't lost (looks like: [R] or [B])
        output += " [" + string(node->getRed() ? "R" : "B") + "]";
    }
    return output; //return what we got!
}

//recursively searches through the tree and returns the node whose data matches the given int, and also builds a string with a visual representation of the path to it, used by search
Node* RBTree::findPath(Node* current, int theint, string& path, const string& prefix, bool right, bool root) {
    if (current == NIL) { //if we ran out of nodes, that means the int is nowhere in the tree, so we just return NIL
        return NIL; //this works since BSTs are organized by int comparison, so we can be sure it isn't just somewhere else in the tree
    }
    bool found = false; //if the current node has the int we're looking for
    if (theint == current->getData()) { //if the current node's data matches the int, we truify found because it's a match!
        found = true;
    }
    string curve; //the curvy line that connects the vertical line or parent to the number, defaults to "" for the root
    if (!root) { //make the curve face the parent depending on which side the parent is on
        curve = right ? ",-- " : "'-- ";
    } //find direction to continue checking in, left if the int is less than the current data, and right if it's greater
    bool lr = theint < current->getData() ? 0 : 1; //BSTs are organized so that if the int is in the tree, it's guaranteed to be in that direction
    string childPrefix = prefix; //the prefix that the child will have, prefix plus an extra chunk
    if (!root) { //root is the very top, so no lines above that, so if it's the root we don't add anything for the children yet
        childPrefix += (lr == right ? "    " : "|   "); //an extra line goes above only if the directions are different (because the child goes under the grandparent's line which connects it to the parent)
    }
    Node* foundNode = NIL; //if we haven't found the int yet, make the children keep looking for it and put the result in here
    if (!found && lr) { //if we havent't found anything yet and we need to check to the right
        foundNode = findPath(current->getNext(lr), theint, path, childPrefix, lr); //try to find the node to the right, and continue building the path string with the child prefix
    }
    path += formatNode(current, prefix + curve); //add the current node to the path using the prefix passed down from its ancestors, and the curve
    if (found) { //if this was the node we were trying to find, we also add a marker arrow after the node
        path += "  <----- HERE IT IS!"; //makes it very obvious, also the marker looks nice actually
        return current; //return the node because that's what we were trying to find!
    }
    if (!lr) { //keep checking to the left if that's where the int might be, and continue building the path string with the child prefix
        foundNode = findPath(current->getNext(lr), theint, path, childPrefix, lr);
    }
    return foundNode; //return whatever the left or right child found, passing it all the way back to the first calling of findPath. What an inspirational story
}

//finds and returns the node with the given int, and builds a string showing the path to it starting from the root
Node* RBTree::search(int query, string& visual) {
    Node* address = findPath(root, query, visual, "", false, true); //finds the node with the query int starting from the root and builds the path string
    if (address == NIL) return NULL; //return NULL if the int isn't there and we found NIL (because we can't compare to NIL in main since it's part of the tree class)
    return address; //return the node we found if we did indeed find the int in the tree
}

//recursively prints a visual representation of the tree with connecting lines (a sideways tree, can't be normalways because it would hit the edge of the terminal really quickly)
void RBTree::printNode(ostream& out, Node* current, const string& prefix, bool right, bool root) const {
    if (current == NIL) { //no ints in the tree to print, we know this because we manually check if children are nil before recursively passing them back into this function, so if current is nil it's guaranteed to be the first call
        out << "\nTree is empty, no integers to print. (Type HELP for help)";
        return;
    }
    string curve; //the curvy line that connects the vertical line or parent to the number, defaults to "" for the root
    if (!root) { //make the curve face the parent depending on which side the parent is on
        curve = right ? ",-- " : "'-- ";
    }
    Node* child = current->getNext(1); //get the right child first
    if (child != NIL) { //if the right child exists, print it!
        //the prefix the child will use, also defaults to "" for the root, since the root's children aren't below anything
        string childPrefix = prefix; //we start with the current prefix, since whatever the parent is under the child will also be under                    _|
        if (!root) { //if this is a left child, the right child will have an extra line over it, since we just curved back towards the parent above. Visual: |_|
            childPrefix += !right ? "|   " : "    "; //otherwise just do a fake tab, we use spaces since our implementation must have consistent spacing on all consoles
        } //starts printing the left child with the new prefix
        printNode(out, child, childPrefix, true);
    } //prints the current node after everything to the left of it (printing in this order makes it so the tree gets automatically spaced and formatted!)
    out << formatNode(current, prefix + curve); //prints the stuff before the int including the line connecting it to the parent, and indicates the node's color
    if (current->getAmount() > 1) { //if there's more than just one of that int, we also print how much of that int there is
        out << " (" << current->getAmount() << ")";
    }
    child = current->getNext(0); //get the left child now
    if (child != NIL) { //if the left child exists, print it!
        string childPrefix = prefix; //same general process as the right child
        if (!root) { //if this is a right child, the left child will have an extra line, since we're curving back towards the parent. Visual: _|'|
            childPrefix += right ? "|   " : "    "; //otherwise do the fake tab                                                                |
        } //starts printing the right child with the new prefix
        printNode(out, child, childPrefix, false);
    }
}

//the tree is empty if the node is NIL
bool RBTree::empty() {
    return root == NIL;
}

//overloads the << operator so we can cout << tree and it prints out a visual of the tree
ostream& operator<<(std::ostream& out, const RBTree& tree) {
    tree.printNode(out, tree.root, "", false, true); //calls the recursive tree printer starting from the root
    return out; //returns out to support cout chaining (eg. cout << tree1 << tree2, etc. I don't use this but that's why << overloads return the ostream&)
}

//recursively get (census) the sum and amount of all the ints, used by the average function
void RBTree::censeTree(Node* current, long long& sum, size_t& population) { //the correct verb is census but that's also the noun and it sounded weird to me so I used the incorrect word cense instead
    if (current == NIL) return; //stop once we run out of nodes
    size_t amount = current->getAmount(); //get how many of this int are in the node
    population += amount; //add the amount to the total amount of ints
    sum += current->getData() * amount; //add the current node's int(s) to the total
    censeTree(current->getNext(0), sum, population); //continue checking through the left branch
    censeTree(current->getNext(1), sum, population); //continue through the right branch
}

//prints the average of all the ints.
long double RBTree::getAverage() {
    if (root == NIL) { //return NaN if there's nothing to average
        return numeric_limits<long double>::quiet_NaN();
    }
    long long sum = 0; //the sum is a long long int in case we have like a million ints in the tree, so we can fit them all in this big int
    size_t count = 0; //how many integers in the tree
    censeTree(root, sum, count); //start the sum and count censusing process starting from the root
    return static_cast<long double>(sum)/count; //prints the average (as a long double so division doesn't truncate)
}

//recurses through the tree to make sure the black height between each node and its descendants is the same, and also finds double red node errors
size_t RBTree::checkNode(Node* current, RBStatus& status, int min, int max) { //also needs to check bounds to check if it follows BST order
    if (current == NIL) return 0; //return if we've reached the end of the tree
    
    //checks if the current value is outside the bounds, meaning it's not properly balancd according to BST order
    if (current->getData() < min || current->getData() > max) {
        status.bstOrder = false; //marks the rule as violated
        status.orderviolators.push_back(current->getData()); //adds the violator to the tracker so it's easier to tell where the violation was
    }
    size_t bhincrement = (!current->getRed() ? 1 : 0); //adds 1 to the black height if the current node is black
    size_t lbh = checkNode(current->getNext(0), status, min, current->getData()); //check the left and right branches, bounded within the current node's value according to BST order
    size_t rbh = checkNode(current->getNext(1), status, current->getData(), max);
    
    if (current->getRed() && current->getPrev()->getRed()) { //if this node and the node's parent are both red, that's two reds in a row which violates rule 3
        status.redChildBlack = false; //marks the rule as violated
        status.drviolators.push_back(current->getData()); //adds the violator to the tracker so it's easier to tell where the violation was
    }
    if (lbh != rbh) { //if the black heights of both branches don't match, that's a violation of rule 4
        status.blackHeightEqual = false; //marks the rule as violated
        status.bhviolators.push_back(current->getData()); //adds the violator to the tracker so it's easier to tell where the violation was
    }

    return lbh + bhincrement; //the black height from this node is the black height of one of the diretcions (they SHOULD both be the same) plus 1 if this is a black node
}

//checks the tree for if it follows all the RB tree properties, then returns a struct with the data it collected
RBStatus RBTree::verify() {
    RBStatus status = RBStatus(); //initialize the rule tracker, all rules are true by default (meaning innocent until proven guilty)

    if (NIL->getRed()) status.nilBlack = false; //if the root or NIL is red, that's invalid and we set their respective rule bool to false because they're not following the rules
    if (root->getRed()) status.rootBlack = false;

    //make sure the tree's red nodes have only black children and that the black height is equal across all paths (also finds the black height from the root. If the tree is faulty, the black height will be the bh of the leftmost branch)
    status.blackHeight = checkNode(root, status, numeric_limits<int>::min(), numeric_limits<int>::max()); //also sets the initial bounds to anything within the 32-bit integer limit

    return status; //return the data we just collected
}
