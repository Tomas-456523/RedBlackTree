#include "RBTree.h"
#include "Node.h"


RBTree::RBTree() : NIL(new Node(0)), root(NIL) {
    NIL->setNext(NIL, 0);
    NIL->setNext(NIL, 1);
    NIL->setPrev(NIL);
    NIL->setRed(0);
}

RBTree::~RBTree() {
    if (root != NIL) deleteAll(root);
    delete NIL;
}

//rotates the given node in the given direction, assumes the passed Node*& is a reference to the pointer to node in node's parent
void RBTree::rotateNode(Node*& node, bool right) { //"right" represents if it's a right rotation or not
    Node* lchild = node->getNext(!right); //node's left child, as long as you rotate the tree so that the right variable is actually pointing to the right
    Node* lrchild = lchild->getNext(right); //lchild's right child. lchild is guaranteed to not be NIL because that's how the RB tree logic works, so this is safe to call
    
    lchild->setNext(node, right); //set lchild's right child to node
    node->setNext(lrchild, !right); //set node's left child to lchild's right child
    
    if (lrchild) lrchild->setPrev(node); //if lrchild isn't NIL we tell it it has been adopted by node
    lchild->setPrev(node->getPrev()); //set lchild's parent to node's old parent because it's in node's place now
    node->setPrev(lchild); //lchild is now node's parent so we set that

    node = lchild; //update node's old parent to now point to lchild instead
}

void RBTree::balanceInsertion(Node*& current) {
    while (current->getPrev()->getRed()) {
        Node*& parent = current->getPrev();
        Node*& gparent = parent->getPrev();
        bool pleft = parent == gparent->getNext(0); //we orient the operations around if the parent is a left child

        Node*& uncle = gparent->getNext(!pleft);

        if (uncle && uncle->getRed()) { //if the grandparent's !pleft child is red, make the grandparent red, and both the grandparent's children black. After that, set current to the grandparent
            gparent->setRed(1);
            parent->setRed(0);
            gparent->getNext(!pleft)->setRed(0);
            current = gparent;
        } else if (current == parent->getNext(!pleft)) { //else if current is the !pleft child, set current to the parent and rotate current to the pleft
            current = parent;
            rotateNode(current, pleft);
        } else { //else, set the parent to black and the grandparent to red, then rotate the grandparent to the !pleft
            parent->setRed(0);
            gparent->setRed(1);
            rotateRight(gparent, !pleft);
        }
    }
    //make sure root is black
}

//future function I'm sure will be very fun to implement
void RBTree::balanceDeletion(Node*& recent) {
    //so spooky
}

//recursively find where to put the int and then put it there, or increment the duplicate counter if it's a duplicate
void RBTree::addInt(Node*& current, int theint) {
    if (current == NIL) { //if we've reached the leaf of the tree, we add the int here in a new node
        current = new Node(theint); //works because getNext returns a reference

        //assign the parent

        //balanceInsertion()

        return; //return because NIL has no children and we don't need to check all that
    }
    int data = current->getData(); //get current's int so we don't get it twice
    if (theint < data) { //if the int < current int, it goes somewhere to the left of the current node, so keep recursing to the left
        addInt(current->getNext(0), theint);
    } else if (theint > data) { //if the int > current int, it goes somewhere to the right of the current node, so keep recursing to the right
        addInt(current->getNext(1), theint);
    } else { //if the int == current int, it's a duplicate so we increment the int amount in the current node
        current->increment();
    }
}

//gets the inorder successor to the given node (the node with smallest number greater than current's)
Node*& RBTree::getSuccessor(Node* current) { //start finding the successor one to the right, since all numbers to the right are greater than current's
    Node** successor = &current->getNext(1); //store successor as Node** instead of Node*& so we don't modify the tree while going to the left
    while ((*successor)->getNext(0) != NIL) { //go to the left until we can't, since going to the left gives a smaller number, and we want the smallest number from here
        successor = &(*successor)->getNext(0); //since successor is a Node**, we have do de-node it, go to the left, then pointerize it again
    }
    return *successor; //return the current successor because we've left the for loop meaning we've reached the leftmost and smallest number on the right branch of the passed node
}

//recursively finde the int to delete in the tree, and then delete it and reorganize the tree. The whole process works because Node.getNext returns a reference, and that also makes it so we don't need any special condition for removing the root
void RBTree::removeNode(Node*& current, int theint) {
    if (current == NIL) { //if we ran out of nodes, that means the int isn't in the tree, so we say error and return
        cout << "\n" << theint << " is not in the tree; can't remove anything.";
        return;
    } //recurse to the left or right branch, depending on if it's < or > the current data, since that's how BSTs work
    if (theint < current->getData()) {
        removeNode(current->getNext(0), theint);
        return; //return because we don't have anything to do on this layer
    } else if (theint > current->getData()) {
        removeNode(current->getNext(1), theint);
        return;
    }
    current->decrement(); //decrease amount of the int stored
    if (current->getAmount()) { //if there's still some of the int left in the node, we say how much are left
        cout << "\nRemoved one " << theint << " from tree; " << current->getAmount() << " of this integer left.";
        return; //return since we can't delete anything yet
    } //store the current node as old so we can replace it but still manage it later
    Node* old = current;
    if (!current->getNext(0)) { //if the left child is null, pull the right child into the current node of the tree. This presevres balance because both of current's children will also be greater than current's parent if current is a right child, and vice versa, which is what BSTs are supposed to do
        current = current->getNext(1); //the right child might also be null, but that's fine cause if it has no children, we need to just nullify it anyway                                                    ^
        delete old; //delete the old node because that's what we're here for                                                                                                                                   |
    } else if (!current->getNext(1)) { //if the left child is not null, but the right child is, pull the left into its parent's slot. This also preserves balance for the vice versa reason of this comment ___|
        current = current->getNext(0); //left child will never be null here
        delete old; //still delete the old node because we replaced it
    } else { //if both children are not null, so current has two children
        Node*& nextOld = getSuccessor(current); //get the successor of the current node (the smallest node larger than it; guaranteed to preserve BST balance since it's smaller than everything else in the right, but still bigger than everything to the left), and also its old position before we move it move it
        Node* next = nextOld; //get the old value of next before we overwrite its old position
        nextOld = next->getNext(1); //put the right child onto where next was, so we don't abandon any children that might've been there (successor is guaranteed to have no left children, since it was found by going all the way to the left). This might be null, but if that's the case we need to nullify nextOld anyway so that works out pretty well
        current = next; //move the successor to the old current's position in the tree
        current->setNext(old->getNext(0), 0); //sets the new current's children to the old current's children, to preserve continuity
        current->setNext(old->getNext(1), 1);
        delete old; //delete the old node because that's the reason we called this function in the first place
    } //success text! also confirms what we just deleted, and that there weren't any duplicates
    cout << "\nSuccessfully removed " << theint << " from the tree!";
}

//gets an integer to remove from the tree and starts the integer removal process
void RBTree::removeInt(Node*& root) {
    if (root == NIL) { //error text and return if there's no tree to remove from yet
        cout << "\nTree is empty, no integers to remove. (Type HELP for help)";
        return;
    } //prompt
    cout << "\nWhich integer do you want to remove?";
    removeNode(root, makeInt()); //gets an int from the user and starts the int removal process for that int
}

//recursively delete all the nodes, branch off from current and delete all descendants then delete the current node
void RBTree::deleteAll(Node* current) {
    if (current == NIL) return; //if we've reached the end of the tree, there's nothing more to delete so just return
    deleteAll(current->getNext(0)); //start deleting all descendants to the left
    deleteAll(current->getNext(1)); //start deleting all descendants to the right
    delete current; //now that all the descendants are deleted, delete the current node
}

//recursively searches through the tree and returns the node whose data matches the given int, and also builds a string with a visual representation of the path to it, used by search
Node* RBTree::findPath(Node* current, int theint, string& path, const string& prefix, bool right = false, bool root = false) {
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
    path += "\n" + prefix + curve + to_string(current->getData()); //add the current node to the path using the prefix passed down from its ancestors, and the curve
    if (found) { //if this was the node we were trying to find, we also add a marker arrow after the node
        path += "  <----- HERE IT IS!"; //makes it very obvious, also the marker looks nice actually
        return current; //return the node because that's what we were trying to find!
    }
    if (!lr) { //keep checking to the left if that's where the int might be, and continue building the path string with the child prefix
        foundNode = findPath(current->getNext(lr), theint, path, childPrefix, lr);
    }
    return foundNode; //return whatever the left or right child found, passing it all the way back to the first calling of findPath. What an inspirational story
}

//confirms if a given int is in the tree, along with a visual representation of the path to it starting from the root if it is indeed there
void RBTree::searchInt(Node* root) {
    if (root == NIL) { //can't search for an int if there's nothing there so error and return
        cout << "\nTree is empty, no integers to search for. (Type HELP for help)";
        return;
    } //prompt
    cout << "\nWhat integer do you want to search for?";
    int theint = makeInt(); //gets the int the user wants to find
    string pathVisual; //the visual of the path to the int that we build into
    Node* thenode = findPath(root, theint, pathVisual, "", false, true); //find the node the int is in and build the path visual
    if (thenode == NIL) { //give error and return if we didn't find a node with the given int in the tree
        cout << "\n" << theint << " is not in the tree.";
        return;
    }//confirm that the integer is indeed in the tree
    cout << "\n" << theint << " is indeed in the tree";
    if (thenode->getAmount() > 1) { //indicate how many of the int is in the tree if it isn't just one
        cout << ", " << thenode->getAmount() << " times and";
    }
    cout << "in a " << (thenode->getRed() ? "red" : "black") << " node.\nPath:" << pathVisual; //finish the last sentence and also print the visual of the path we found
}

//recursively prints a visual representation of the tree with connecting lines (a sideways tree, can't be normalways because it would hit the edge of the terminal really quickly)
void RBTree::printNode(Node* current, const string prefix = "", bool right = false, bool root = false) {
    if (current == NIL) { //no ints in the tree to print, we know this because we manually check if children are nil before recursively passing them back into this function, so if current is nil it's guaranteed to be the first call
        cout << "\nTree is empty, no integers to print. (Type HELP for help)";
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
        printNode(child, childPrefix, true);
    } //prints the current node after everything to the left of it (printing in this order makes it so the tree gets automatically spaced and formatted!)
    cout << '\n' << prefix << curve << current->getData() << " (" << (current->getRed() ? "Red" : "Black") << ")"; //prints the preceding stuff over the int, then the connector curve to the parent, and finally the int itself, all in a new line
    if (current->getAmount() > 1) { //if there's more than just one of that int, we also print how much of that int there is
        cout << " (" << current->getAmount() << ")";
    }
    child = current->getNext(0); //get the left child now
    if (child != NIL) { //if the left child exists, print it!
        string childPrefix = prefix; //same general process as the right child
        if (!root) { //if this is a right child, the left child will have an extra line, since we're curving back towards the parent. Visual: _|'|
            childPrefix += right ? "|   " : "    "; //otherwise do the fake tab                                                                |
        } //starts printing the right child with the new prefix
        printNode(child, childPrefix, false);
    }
}

//recursively get (census) the sum and amount of all the ints, used by the average function
void RBTree::censeTree(Node* current, long long& sum, int& population) { //the correct verb is census but that's also the noun and it sounded weird to me so I used the incorrect word cense instead
    if (current == NIL) return; //stop once we run out of nodes
    size_t amount = current->getAmount(); //get how many of this int are in the node
    population += amount; //add the amount to the total amount of ints
    sum += current->getData() * amount; //add the current node's int(s) to the total
    censeTree(current->getNext(0), sum, population); //continue checking through the left branch
    censeTree(current->getNext(1), sum, population); //continue through the right branch
}

//prints the average of all the ints.
void RBTree::printAverage(Node* root) {
    if (root == NIL) { //error text and return because there's no ints in the tree to get the average value of
        cout << "\nThere are no integers to average. (Type HELP for help)";
        return;
    }
    long long sum = 0; //the sum is a long long int in case we have like a million ints in the tree, so we can fit them all in this big int
    int count = 0; //how many integers in the tree
    censeTree(root, sum, count); //start the sum and count censusing process starting from the root
    cout << "\nTree average: " << 1.0*sum/count; //prints the average (multiplied by a floatified 1 so division doesn't truncate)
}