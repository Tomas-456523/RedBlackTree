/* Tomas Carranza Echaniz
*  2/25/26
*  This program is a red-black tree that stores integers from 1 to 999. It is a binary search tree that balances itself
*  every time an item is added or removed, according to the official red-black tree rules. The user can ADD strings of
*  integers manually,or READ in integers from a file. Nodes also track an amount of ints, so duplicates will be stored
*  in the same node. The user cannot REMOVE a specified integer from the tree, because this feature has not been
*  implemented yet as of RB Tree Part 1. The user can SEARCH to find if an integer is in the tree, and also PRINT the
*  whole tree. They can also print the AVERAGE of all the ints in the tree.
*  
*  RED-BLACK TREE RULES:
*  1. The root is black
*  2. All leaves (NIL nodes) are black
*  3. Red nodes have only black children
*  4. Counting the black nodes starting from any node and going to any of its descendants should return the same amount
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>
#include "Node.h"
#include "RBTree.h"
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
int makeInt() {
    int num = 0; //the int we input into and check
    while (true) { //loops forever
        cout << "\n> ";
        if (cin >> num) { //gets the input and returns it if it was a valid integer
            CinIgnoreAll(true); //removes the newline character after valid integer input
            return num;
        } else { //otherwise give error message and try again
            cout << "\nThis is not an integer.";
        }
        CinIgnoreAll(); //removes the newline character or invalid input
    }
}

//looks at a given input, and adds all valid ints in it into the tree
void parseInts(istream& stin, RBTree& tree) { //stin = string + in, like cin = char + in, but sin is something else so I didn't go with that
    int good = 0, bad = 0, ugly = 0; //counts the good, the bad (non-int), and the ugly (out of range) inputs
    for (int current;;) { //creates a current int to read into from stin, and loops until the end of the input
        if (stin >> current) { //gets the next chunk of data, skipping whitespace, and at the same time checks if it was valid input!
            if (current < 1 || current > 999) { //if it's out of range, we increment the ugly counter and continue to the next loop so we don't label it as another good
                ugly++;
                continue;
            } //inserts the int into the tree
            tree.insert(current);
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
    if (ugly) {
        cout << '\n' << ugly << " integer" << (ugly != 1 ? "s" : "") << " beyond bounds; must be 1-999.";
    }
}

//user types a string of integers, and we try to add those to the tree
void typeInts(RBTree& tree) {
    string ints; //string we input into
    cout << "\nEnter your integer(s).\n> ";
    getline(cin, ints); //get the input from the user
    stringstream stin(ints); //create a new stringstream so we can treat the ints string like cin in parseInts
    parseInts(stin, tree); //interprets the string as various ints, and adds them to the tree
}

//user chooses a file, which the program then reads ints from and adds those to the tree
void readInts(RBTree& tree) {
    cout << "\nWhich file do you want to read from?\n> ";
    string file; //the file name
    getline(cin, file); //the user says which file to read from
    ifstream stin(file); //creates an ifstream with the file, so we can read from the file the same way as reading from cin
    if (!stin) { //if no file was found say error and don't proceed with getting ints, because there's nowhere to get ints from
        cout << "\nNo file named \"" << file << "\" found.";
        return;
    } //interprets the file text as various ints, and adds them to the tree
    parseInts(stin, tree);
}

//gets an integer to remove from the tree and starts the integer removal process
void removeInt(RBTree& tree) {
    if (tree.empty()) { //error text and return if there's no tree to remove from yet
        cout << "\nTree is empty, no integers to remove. (Type HELP for help)";
        return;
    }
    cout << "\nWhich integer do you want to remove?"; //prompt!
    int remaining = tree.remove(makeInt()); //gets an int from the user, starts the int removal process for that int, and checks how much of the int is left
    if (!remaining) {
        cout << "\nSuccessfully removed " << theint << " from the tree!";
    } else if (remaining > 0) {
        cout << "\nRemoved one " << theint << " from tree; " << current->getAmount() << " of this integer left.";
    } else {
        cout << "\n" << theint << " is not in the tree; can't remove anything.";
    }
}

//confirms if a given int is in the tree, along with a visual representation of the path to it starting from the root if it is indeed there
void searchInt(RBTree& tree) {
    if (tree.empty()) { //error text and return because any search would return NULL (I could just go with the process normally and it would be accurate but this is just for clarity)
        cout << "\nTree is empty, no integers to search for. (Type HELP for help)";
        return;
    }
    cout << "\nWhat integer do you want to search for?"; //prompt
    int theint = makeInt(); //gets the int the user wants to find
    string path; //the path visual string which we build into
    Node* thenode = tree.getNode(theint, path); //finds the node with the int and builds the visual of the path to the int
    if (thenode == NULL) { //if buildPath returns an empty string that means the tree does not contain the int
        cout << "\n" << theint << " is not in the tree."; //so we say that and return
        return;
    } //confirm that the integer is indeed in the tree
    cout << "\n" << theint << " is indeed in the tree";
    if (thenode->getAmount() > 1) { //indicate how many of the int is in the tree if it isn't just one
        cout << ", " << thenode->getAmount() << " times and";
    }
    cout << "in a " << (thenode->getRed() ? "red" : "black") << " node.\nPath:" << path; //finish the last sentence and also print the visual of the path we found
}

//finds the average of all the values in the tree and prints that
void printAverage(RBTree& tree) {
    if (tree.empty()) { //error text and return if there's no integers in the tree to average
        cout << "\nTree is empty, no integers to average. (Type HELP for help)";
        return;
    }
    cout << "\nTree average: " << tree.getAverage(); //asks the tree what its average is and prints it!
}

//the main loop
int main() {
    string username; //tries to find the username because Rubert wants to greet the user personally
    if (const char* name = getenv("USER")) { //unix terminals store the username under USER, so first we try to get that
        username = name; //set the name to that if it didn't return NULL
    } else if (const char* name = getenv("USERNAME")) { //if getting the name with USER returned NULL, try to get it with USERNAME because windows stores it like that
        username = name;
    }
    if (!username.empty()) { //capitalize the first letter of the username if we found one because when I saw my name in the terminal it was all lowercase and it looked weird
        username[0] = toupper(static_cast<unsigned char>(username[0]));
    }

    RBTree tree = RBTree(); //the red-black tree, starts with a NIL root until something is added

    //welcome message with instructions with a username-based greeting if we were able to find it, and also sets precision to 3 decimal points for the average function
    cout << "\nI've been expecting you" << (username.empty() ? "" : ", " + username) << ". I am Rubert, regent of Red-Black Trees. (RB Trees handle integers 1-999)\n\nWhat would you like to do? (Type HELP for help)" << fixed << setprecision(3);

    string command; //the command that the user inputs into
    //continues until continuing is falsified (by typing QUIT)
    for (bool continuing = true; continuing;) {
        cout << "\n> "; //thing for the player to type after

        getline(cin, command); //gets the player input

        AllCaps(command); //capitalizes the command for easier interpretation

        //main menu for managing all red black trees
        //PLANT
        //SELECT
        //COPY
        //DELETE
        //TREES
        //QUIT

        //after using select on one rb tree
        //ADD
        //READ
        //RENAME
        //REMOVE
        //SEARCH
        //PRINT
        //AVERAGE (or STATS?)
        //HELP
        //BACK
        //QUIT

        //calls function corresponding to the given command word
        if (command == "ADD") { //add integer(s)
            typeInts(tree);
        } else if (command == "READ") { //read in integers from file
            readInts(tree);
        } else if (command == "REMOVE") { //remove a specified int in the tree
            removeInt(tree);
        } else if (command == "SEARCH") { //find a given int in the tree
            searchInt(tree);
        } else if (command == "PRINT") { //print visualization of tree
            cout << tree; //I overloaded the << operator so we can just straight up print the tree
        } else if (command == "AVERAGE") { //print average of all ints
            printAverage(tree);
        } else if (command == "HELP") { //print all valid command words
            cout << "\nYour command words are:\nADD     - Manually insert one or more integers (1-999).\nREAD    - Read in a string of integers (1-999) from a file.\nREMOVE  - Remove an integer from the tree.\nSEARCH  - Find an integer in the tree.\nAVERAGE - Calculate the average of all integers.\nHELP    - Print all valid commands.\nQUIT    - Exit the program.";
        } else if (command == "QUIT") { //quit the program
            continuing = false; //leave the main player loop
        } else { //give error message if the user typed something unacceptable
            cout << "\nInvalid command \"" << command << "\". (type HELP for help)";
        }
    }

    //says a friendly farewell, wishing you a nice day
    cout << "\nEnjoy your next 24 hours.\n";

    //tree goes out of scope and gets deleted here
}
