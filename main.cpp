/* Tomas Carranza Echaniz
*  2/10/26
*  This program is a binary search tree that stores integers from 1 to 999. The user can ADD strings of integers manually, or
*  READ in integers from a file. Nodes also track an amount of ints, so duplicates will be stored in the same node. The user
*  can REMOVE a specified integer from the tree, which will decrement its amount, and fully remove it if the amount reaches 0.
*  The user can SEARCH to find if an integer is in the tree, and also optionally print where it is, and also PRINT the whole
*  tree normally. They can also print the AVERAGE of all the ints in the tree.
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>
#include "Node.h"
using namespace std;

//for ignoring faulty input and extra characters, functionality modified from my previous projects
void CinIgnoreAll(bool force = false) {
    if (force || !cin) { //only go if input is faulty, otherwise forces the user to input an extra enter
        cin.clear(); //clears error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //ignores all characters up until a new line
    }
}

//disposes of all characters in the given istream up until whitespace or the end of the input, used so istreams can dispose of faulty non-int text in parseInts without making a string to chuck faulty text into
void DisposeGarbage(istream& in) {
    in.clear(); //removes error flag because when we call this there's an error flagged
    int next; //the next character in the input, except istream.peek() returns an int so yeah it has to be an int
    //sets next to the next char, until we've reached the end (EOF), or the next character is whitespace, so we've reached the end of the garbage
    while ((next = in.peek()) != EOF && !isspace(static_cast<unsigned char>(next))) { //check as unsigned char to prevent undefined behavior
        in.get(); //removes the next char
    }
}

//capitalizes the given string for easier command interpretation
void AllCaps(string& word) {
    for (size_t i = 0; i < word.size(); i++) { //sets all the characters to a capitalized unsigned version of the char (unsigned because some systems sign the chars)
        word[i] = toupper((unsigned char)word[i]);
    }
}

//gets a singular int from the user, used to know which int to search for or remove in those functions
int makeNum() {
    int num = 0; //the int we input into and check
    while (true) { //loops forever
        cout << "\n> ";
        if (cin >> num) { //gets the input and returns it if it was a valid integer
            CinIgnoreAll(true); //removes the newline character after valid integer input
            return num;
        } else { //otherwise give generic error message and try again
            cout << "\nPlease enter one integer.";
        }
        CinIgnoreAll(); //removes the newline character or invalid input
    }
}

//recursively find where to put the int and then put it there, or increment the duplicate counter if it's a duplicate
void addInt(Node*& current, int theint) {
    if (current == NULL) { //if we've reached the leaf of the tree, we add the int here in a new node
        current = new Node(theint); //works because getNext returns a reference
        return; //return because NULL has no children and we don't need to check all that
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

//looks at a given input, and adds all valid ints in it into the table
void parseInts(istream& stin, Node*& root) { //stin = string + in, like cin = char + in, but sin is something else so I didn't go with that
    int good = 0, bad = 0, ugly = 0; //counts the good, the bad (non-int), and the ugly (out of range) inputs
    for (int current;;) { //creates a current int to read into from stin, and loops until the end of the input
        if (stin >> current) { //gets the next chunk of data, skipping whitespace, and at the same time checks if it was valid input!
            if (current < 1 || current > 999) { //if it's out of range, we increment the ugly counter and continue to the next loop so we don't label it as another good
                ugly++;
                continue;
            } //adds the int to the table and updates the heap accordingly
            addInt(root, current);
            good++; //increment the good counter because this was good input
        } else if (stin.eof()) { //if stin gave faulty input because it reached the end
            break; //leave the loop
        } else { //if stin gave faulty input, and not because it was the end
            DisposeGarbage(stin); //dispose of the current faulty input up until whitespace
            bad++; //increments the bad counter because it was faulty input
        }
    } //if we had good input, say how many (or if there was no input at all, print that we successfully added nothing)
    if (good || (!bad && !ugly)) {
        cout << "\nSuccessfully added " << good << " integer" << (good != 1 ? "s" : "") << "!";
    } //if we had bad inputs, tell the user how many + instructions for the future
    if (bad) {
        cout << '\n' << bad << " faulty item" << (bad != 1 ? "s" : "") << " in input; must be integers.";
    } //if we had out of range inputs, tell the user how many + instructions for the future
    if (ugly) {                                                                        //Bingo Seane Trevor no know how to wrangle ints outside 1-999 :(
        cout << '\n' << ugly << " integer" << (ugly != 1 ? "s" : "") << " found outside area of expertise; must be 1-999.";
    }
}

//user types a string of integers, and we try to add those to the tree
void typeInts(Node*& root) {
    string ints; //string we input into
    cout << "\nEnter your integer(s).\n> ";
    getline(cin, ints); //get the input from the user
    stringstream stin(ints); //create a new stringstream so we can treat the ints string like cin in parseInts
    parseInts(stin, root); //interprets the string as various ints, and adds them to the tree
}

//user chooses a file, which the program then reads ints from and adds those to the tree
void readInts(Node*& root) {
    cout << "\nWhich file do you want to read from?\n> ";
    string file; //the file name
    getline(cin, file); //the user says which file to read from
    ifstream stin(file); //creates an ifstream with the file, so we can read from the file the same way as reading from cin
    if (!stin) { //if no file was found say error and don't proceed with getting ints, because there's nowhere to get ints from
        cout << "\nNo file named \"" << file << "\" found.";
        return;
    } //interprets the file text as various ints, and adds them to the tree
    parseInts(stin, root);
}

//gets the inorder successor to the given node (the node with smallest number greater than current's)
Node*& getSuccessor(Node* current) { //start finding the successor one to the right, since all numbers to the right are greater than current's
    Node** successor = &current->getNext(1); //store successor as Node** instead of Node*& so we don't modify the tree while going to the left
    while ((*successor)->getNext(0) != NULL) { //go to the left until we can't, since going to the left gives a smaller number, and we want the smallest number from here
        successor = &(*successor)->getNext(0); //since successor is a Node**, we have do de-node it, go to the left, then pointerize it again
    }
    return *successor; //return the current successor because we've left the for loop meaning we've reached the leftmost and smallest number on the right branch of the passed node
}

//recursively finde the int to delete in the tree, and then delete it and reorganize the tree. The whole process works because Node.getNext returns a reference, and that also makes it so we don't need any special condition for removing the root
void removeNode(Node*& current, int theint) {
    if (current == NULL) { //if we ran out of nodes, that means the int isn't in the tree, so we say error and return
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
        Node*& nextOld = getSuccessor(current); //get the successor of the current node (the largest node smaller than it; guaranteed to preserve BST balance since it's smaller than everything else in the right, but still bigger than everything to the left), and also its old position before we move it move it
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
void removeInt(Node*& root) {
    if (root == NULL) { //error text and return if there's no tree to remove from yet
        cout << "\nTree is empty, no integers to remove. (Type HELP for help)";
        return;
    } //prompt
    cout << "\nWhich integer do you want to remove?\n> ";
    removeNode(root, makeInt()); //gets an int from the user and starts the int removal process for that int
}

//recursively delete all the nodes, branch off from current and delete all descendants then delete the current node
void deleteAll(Node* current) {
    if (current == NULL) return; //if we've reached the end of the tree, there's nothing more to delete so just return
    deleteAll(current->getNext(0)); //start deleting all descendants to the left
    deleteAll(current->getNext(1)); //start deleting all descendants to the right
    delete current; //now that all the descendants are deleted, delete the current node
}

//recursively searches through the tree and returns the node whose data matches the given int, and also builds a string with a visual representation of the path to it, used by search
Node* findPath(Node* current, int theint, string& path, string prefix, bool right = false, bool root = false) {
    if (current == NULL) { //if we ran out of nodes, that means the int is nowhere in the tree, so we just return NULL
        return NULL; //this works since BSTs are organized by int comparison, so we can be sure it isn't just somewhere else in the tree
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
    if (!root && lr) { //add onto the prefix in this way if we need to go to the right
        childPrefix += (!right ? "|   " : "    ");
    }
    path += string("\n") + prefix + curve + to_string(current->getData());
    if (found) {
        path += "<-----";
        return current;
    }
    if (!root && !lr) {
        childPrefix += (right ? "|   " : "    ");
    }
    return findPath(current->getNext(lr), theint, path, childPrefix, lr); //continue finding the int in that direction, and return to the pervious call of findInt whatever we find in that direction
}

//confirms if a given int is in the tree, along with a visual representation of the path to it starting from the root if it is indeed there
void searchInt(Node* root) {
    if (root == NULL) { //can't search for an int if there's nothing there so error and return
        cout << "\nTree is empty, no integers to find. (Type HELP for help)";
        return;
    } //prompt
    cout << "\nWhat integer do you want to find?\n> ";
    int theint = makeInt(); //gets the int the user wants to find
    string pathVisual; //the visual of the path to the int that we build into
    Node* thenode = findPath(root, theint, pathVisual, "", false, true); //find the node the int is in and build the path visual
    if (thenode == NULL) { //give error and return if we didn't find a node with the given int in the tree
        cout << "\n" << theint << " is not in the tree.";
        return;
    }//confirm that the integer is indeed in the tree
    cout << "\n" << theint << " is indeed in the tree";
    if (thenode->getAmount() > 1) { //indicate how many of the int is in the tree if it isn't just one
        cout << ", " << thenode->getAmount() << " times";
    }
    cout << "." << pathVisual; //punctuate the last sentance and also print the visual of the path we found
}

//recursively prints a visual representation of the tree with connecting lines (a sideways tree, can't be normalways because it would hit the edge of the terminal really quickly)
void printNode(Node* current, const string prefix = "", bool right = false, bool root = false) {
    if (current == NULL) { //no ints in the tree to print, we know this because we manually check if children are null before recursively passing them back into this function, so if current is null it's guaranteed to be the first call
        cout << "\nTree is empty, no integers to print. (Type HELP for help)";
        return;
    }
    string curve; //the curvy line that connects the vertical line or parent to the number, defaults to "" for the root
    if (!root) { //make the curve face the parent depending on which side the parent is on
        curve = right ? ",-- " : "'-- ";
    }
    Node* child = current->getNext(1); //get the right child first
    if (child != NULL) { //if the right child exists, print it!
        //the prefix the child will use, also defaults to "" for the root, since the root's children aren't below anything
        string childPrefix = prefix; //we start with the current prefix, since whatever the parent is under the child will also be under                    _|
        if (!root) { //if this is a left child, the right child will have an extra line over it, since we just curved back towards the parent above. Visual: |_|
            childPrefix += !right ? "|   " : "    "; //otherwise just do a fake tab, we use spaces since our implementation must have consistent spacing on all consoles
        } //starts printing the left child with the new prefix
        printNode(child, childPrefix, true);
    } //prints the current node after everything to the left of it (printing in this order makes it so the tree gets automatically spaced and formatted!)
    cout << '\n' << prefix << curve << current->getData(); //prints the preceding stuff over the int, then the connector curve to the parent, and finally the int itself, all in a new line
    if (current->getAmount() > 1) { //if there's more than just one of that int, we also print how much of that int there is
        cout << " (" << current->getAmount() << ")";
    }
    child = current->getNext(0); //get the left child now
    if (child != NULL) { //if the left child exists, print it!
        string childPrefix = prefix; //same general process as the right child
        if (!root) { //if this is a right child, the left child will have an extra line, since we're curving back towards the parent. Visual: _|'|
            childPrefix += right ? "|   " : "    "; //otherwise do the fake tab                                                                |
        } //starts printing the right child with the new prefix
        printNode(child, childPrefix, false);
    }
}

//recursively get (census) the sum and amount of all the ints, used by the average function
void censeTree(Node* current, long long& sum, int& population) { //the correct verb is census but that's also the noun and it sounded weird to me so I used the incorrect word cense instead
    if (current == NULL) return; //stop once we run out of nodes
    population++; //increment in count cause we're checking an int
    sum += current->getData(); //add the current node's int to the total
    censeTree(current->getNext(0), sum, population); //continue checking through the left branch
    censeTree(current->getNext(1), sum, population); //continue through the right branch
}

//prints the average of all the ints. This is actually pretty useful for testing
void printAverage(Node* root) {
    if (root == NULL) { //error text and return because there's no ints in the tree to get the average value of
        cout << "\nThere are no integers to average. (Type HELP for help)";
        return;
    }
    long long sum = 0; //the sum is a long long int in case we have like a million ints in the tree, so we can fit them all in this big int
    int count = 0; //how many integers in the tree
    censeTree(root, sum, count); //start the sum and count censusing process starting from the root
    cout << "\nTree average: " << 1.0*sum/count; //prints the average (multiplied by a floatified 1 so division doesn't truncate)
}

//the main loop
int main() {
    Node* root = NULL; //the root node of the binary tree, empty until something is added

    //welcome message with instructions, also sets precision to 3 decimal points for the average function
    cout << "\nHowdy pardner. Name's Bingo Seane Trevor the Binary Search Tree.\nI'm the best integer wrangler in the west, for integers 1 through 999.\n\nWhat would you like to do? (Type HELP for help)" << fixed << setprecision(3);

    string command; //the command that the user inputs into
    //continues until continuing is falsified (by typing QUIT)
    for (bool continuing = true; continuing;) {
        cout << "\n> "; //thing for the player to type after

        getline(cin, command); //gets the player input

        AllCaps(command); //capitalizes the command for easier interpretation

        //calls function corresponding to the given command word
        if (command == "ADD") { //add integer(s)
            typeInts(root);
        } else if (command == "READ") { //read in integers from file
            readInts(root);
        } else if (command == "REMOVE") { //remove a specified int in the tree
            removeInt(root);
        } else if (command == "SEARCH") { //find a given int in the tree
            searchInt(root);
        } else if (command == "PRINT") { //print visualization of tree
            printNode(root);
        } else if (command == "AVERAGE") { //print average of all ints
            printAverage(root);
        } else if (command == "HELP") { //print all valid command words
            cout << "\nYour command words are:\nADD     - Manually insert a string of integers (1-999).\nREAD    - Read in a string of integers (1-999) from a file.\nREMOVE  - Remove an integer from the tree.\nSEARCH  - Find an integer in the tree.\nAVERAGE - Calculate the average of all integers.\nHELP    - Print all valid commands.\nQUIT    - Exit the program.";
        } else if (command == "QUIT") { //quit the program
            continuing = false; //leave the main player loop
        } else { //give error message if the user typed something unacceptable
            cout << "\nInvalid command \"" << command << "\". (type HELP for help)";
        }
    }

    //says bye
    cout << "\nSo long.\n";

    //recursively delete all the nodes for good practice, starting from the root
    deleteAll(root);
}


