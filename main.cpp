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

//recursively bubble sorts starting at the end of the table until the root
void bubbleUp(int*& table, size_t i) {
    if (i <= 1) return; //if we've reached the root or the table doesn't have any items, there's nothing more to sort
    size_t parenti = getParent(i); //finds the parent of the current item
    if (table[i] > table[parenti]) { //we switch the parent and the child if the child is bigger, since we start from the bottom and bigger ints go on top
        swap(table[i], table[parenti]);
        bubbleUp(table, parenti); //recursively continue with the int we just moved up into the parent index
    }
}

//recursively bubble sorts starting at the root of the heap until the end, goes down the paths with the bigger children
void bubbleDown(int*& table, size_t i, size_t endi) {
    size_t childi = getChild(i, 0); //left child, start assuming we're going with this one
    size_t right = getChild(i, 1); //right child
    if (childi >= endi) return; //if the left child is past the maximum i used, that means the node actually has no children, so we've reached the end and we stop here
    if (right < endi && table[right] > table[childi]) { //if the right child is (existant and) bigger than the left, we use that one instead
        childi = right;
    }
    if (table[childi] > table[i]) { //if the child is bigger than the current item, we move it up, since bigger ints go above
        swap(table[i], table[childi]); //move up through swappage, also makes the smaller parent go down
        bubbleDown(table, childi, endi); //recursively continue with the int we just moved down into the child index
    }
}

//adds an int to the end of table and begins the bubble sort process upwards
void addInt(int theint, int*& table, size_t& tablelen, size_t& endi) {
    if (endi >= tablelen) reSize(table, tablelen); //if we need more room, double the length of table
    table[endi] = theint; //adds the int to the end
    bubbleUp(table, endi); //recursively bubble sorts the heap starting from the new int
    endi++; //increments the end i since there's one more item now
}

//looks at a given input, and adds all valid ints in it into the table
void parseInts(istream& stin, int*& table, size_t& tablelen, size_t& endi) { //stin = string + in, like cin = char + in, but sin is something else so I didn't go with that
    int good = 0, bad = 0, ugly = 0; //counts the good, the bad (non-int), and the ugly (out of range) inputs
    for (int current;;) { //creates a current int to read into from stin, and loops until the end of the input
        if (stin >> current) { //gets the next chunk of data, skipping whitespace, and at the same time checks if it was valid input!
            if (current < 1 || current > 1000) { //if it's out of range, we increment the ugly counter and continue to the next loop so we don't label it as another good
                ugly++;
                continue;
            } //adds the int to the table and updates the heap accordingly
            addInt(current, table, tablelen, endi);
            good++; //increment the good counter because this was good input
        }
        else if (stin.eof()) { //if stin gave faulty input because it reached the end
            break; //leave the loop
        }
        else { //if stin gave faulty input, and not because it was the end
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
    if (ugly) {                                                                       //jurisdiction, cause Maximillian Halifax only handles ints 1-1000
        cout << '\n' << ugly << " integer" << (ugly != 1 ? "s" : "") << " found outside jurisdiction; must be 1-1000.";
    }
}

//user types a string of integers, and we try to add those to the heap
void typeInts(int*& table, size_t& tablelen, size_t& endi) {
    string ints; //string we input into
    cout << "\nEnter your integer(s).\n> ";
    getline(cin, ints); //get the input from the user
    stringstream stin(ints); //create a new stringstream so we can treat the ints string like cin in parseInts
    parseInts(stin, table, tablelen, endi); //interprets the string as various ints, and adds them to the heap
}

//user chooses a file, which the program then reads ints from and adds those to the heap
void readInts(int*& table, size_t& tablelen, size_t& endi) {
    cout << "\nWhich file do you want to read from?\n> ";
    string file; //the file name
    getline(cin, file); //the user says which file to read from
    ifstream stin(file); //creates an ifstream with the file, so we can read from the file the same way as reading from cin
    if (!stin) { //if no file was found say error and don't proceed with getting ints, because there's nowhere to get ints from
        cout << "\nNo file named \"" << file << "\" found.";
        return;
    } //interprets the file text as various ints, and adds them to the heap
    parseInts(stin, table, tablelen, endi);
}

//removes the root of the heap, and adjusts the heap accordingly
void removeRoot(int*& table, size_t& endi, bool printText = true) {
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
}

//prints the average of all the ints. This is actually pretty useful for testing
void average(int*& table, size_t endi) {
    if (endi == 1) { //error text and return because there's no ints in the heap to get the average value of
        cout << "\nThere are no integers to average. (Type HELP for help)";
        return;
    }
    long long sum = 0; //the sum is a long long int in case we have like a million ints in the table, so we can fit them all in this big int
    for (size_t i = 1; i < endi; i++) { //iterates through the table to add all the ints in the table to the sum
        sum += table[i];
    } //prints the average (multiplied by a floatified 1 so division doesn't truncate)
    cout << "\nHeap average: " << 1.0 * sum / (endi - 1); //the amount of ints is endi-1 so that's the denominator
}

//recursively prints a visual representation of the heap with connecting lines (a sideways tree, can't be normalways because it would hit the edge of the terminal really quickly)
void printNode(int*& table, size_t endi, size_t current, const string prefix = "") {
    if (endi == 1) { //error text and return because there's no ints in the heap to print, and we don't wanna print any garbage data left there
        cout << "\nHeap is empty, no integers to print. (Type HELP for help)";
        return;
    }
    bool right = current % 2; //if the current node is a right child (if it's even, false). Works since right children are found using parent*2 + 1, and are therefore always at an odd index
    bool root = current == 1; //if it's the root, we check because that has slighly different prefix rules
    string curve; //the curvy line that connects the vertical line or parent to the number, defaults to "" for the root
    if (!root) { //make the curve face the parent depending on which side the parent is on
        curve = right ? ",-- " : "'-- ";
    }
    size_t childi = getChild(current, 1); //get the right child first
    if (childi < endi) { //if the right child is in range (exists), print it!
        //the prefix the child will use, also defaults to "" for the root, since the root's children aren't below anything
        string childPrefix = prefix; //we start with the current prefix, since whatever the parent is under the child will also be under                    _|
        if (!root) { //if this is a left child, the right child will have an extra line over it, since we just curved back towards the parent above. Visual: |_|
            childPrefix += !right ? "|   " : "    "; //otherwise just do a fake tab, we use spaces since our implementation must have consistent spacing on all consoles
        } //starts printing the left child with the new prefix
        printNode(table, endi, childi, childPrefix);
    } //prints the current node after everything to the left of it (printing in this order makes it so the tree/heap gets automatically spaced and formatted!)
    cout << '\n' << prefix << curve << table[current]; //prints the preceding stuff over the int, then the connector curve to the parent, and finally the int itself, all in a new line
    if (--childi < endi) { //decrements the childi to get to the left child (because right child i = left child i + 1), and print it if in range
        string childPrefix = prefix; //same general process as the right child
        if (!root) { //if this is a right child, the left child will have an extra line, since we're curving back towards the parent. Visual: _|'|
            childPrefix += right ? "|   " : "    "; //otherwise do the fake tab                                                                |
        } //starts printing the right child with the new prefix
        printNode(table, endi, childi, childPrefix);
    }
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
            
        } else if (command == "READ") { //read in integers from file
            
        } else if (command == "REMOVE") { //remove a specified int in the tree
            
        } else if (command == "SEARCH") { //remove and print root int
            
        } else if (command == "PRINT") { //print visualization of tree
            
        } else if (command == "AVERAGE") { //print average of all ints
            
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
