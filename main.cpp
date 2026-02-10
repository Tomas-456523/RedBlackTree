/* Tomas Carranza Echaniz
*  binary search tree wahoo
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

//removes the root of the heap, and adjusts the heap accordingly
/*void removeRoot(int*& table, size_t& endi, bool printText = true) {
    if (endi == 1) { //we can't remove the root if there's no root (no available items in table)
        cout << "\nThere are no integers to remove. (Type HELP for help)";
        return;
    }
    if (printText) { //only print this if called directly from main, not if called by removeAll, cause that would look bad
        cout << "\nRemoving root integer:";
    }
    cout << " " << table[1]; //prints (formatting space and) the value of the root we just got rid of
    table[1] = table[--endi]; //moves the last item in the heap to the root, and also decrements the end index since there's one less int now
    bubbleDown(table, 1, endi);//recursively bubble sorts the heap starting from the root to make sure the values are still sorted correctly; we just put a small int above the big ones, after all.
}*/

//recursively searches through the tree and returns the node whose data matches the given int, used by search
Node* findInt(Node* current, int theint) {
    if (current == NULL) { //if we ran out of nodes, that means the int is nowhere in the tree, so we just return NULL
        return NULL; //this works since BSTs are organized by int comparison, so we can be sure it isn't just somewhere else in the tree
    } if (theint == current->getData()) { //if the current node's data matches the int, we return current because it's a match!
        return current;
    } //find direction to continue checking in, left if the int is less than the current data, and right if it's greater
    bool lr = theint < current->getData() ? 0 : 1; //BSTs are organized so that if the int is in the tree, it's guaranteed to be in that direction
    return findInt(current->getNext(lr), theint); //continue finding the int in that direction, and return to the pervious call of findInt whatever we find in that direction
}

void search(Node* root) {
    if (root == NULL) {
        cout << "\nTree is empty, no integers to find. (Type HELP for help)";
    }
    cout << "\nWhat integer do you want to find?\n> ";
    int theint;
    while (cin >> theint)) {
        if (current < 1 || current > 999) { //if it's out of range, we increment the ugly counter and continue to the next loop so we don't label it as another good
                ugly++;
                continue;
            }
        cout << "\n"
    }
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
        cout << " x " << current->getAmount();
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
            
        } else if (command == "SEARCH") { //find a given int in the tree
            
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

    //delete everything
}
