/* Tomas Carranza Echaniz
*  2/24/26
*  This program is a red black tree that stores integers from 1 to 999. The user can ADD strings of integers manually, or
*  READ in integers from a file. Nodes also track an amount of ints, so duplicates will be stored in the same node. The user
*  can REMOVE a specified integer from the tree, which will decrement its amount, and fully remove it if the amount reaches 0.
*  The user can SEARCH to find if an integer is in the tree, and also optionally print where it is, and also PRINT the whole
*  tree normally. They can also print the AVERAGE of all the ints in the tree.
*  
*  RED-BLACK TREE RULES:
*  1. The root is black
*  2. All leaves are black
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

//looks at a given input, and adds all valid ints in it into the table
void parseInts(istream& stin, Node*& root) { //stin = string + in, like cin = char + in, but sin is something else so I didn't go with that
    int good = 0, bad = 0, ugly = 0; //counts the good, the bad (non-int), and the ugly (out of range) inputs
    for (int current;;) { //creates a current int to read into from stin, and loops until the end of the input
        if (stin >> current) { //gets the next chunk of data, skipping whitespace, and at the same time checks if it was valid input!
            if (current < 1 || current > 999) { //if it's out of range, we increment the ugly counter and continue to the next loop so we don't label it as another good
                ugly++;
                continue;
            } //adds the int to the table and updates the tree accordingly
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
    if (ugly) {
        cout << '\n' << ugly << " integer" << (ugly != 1 ? "s" : "") << " beyond bounds; must be 1-999.";
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

//the main loop
int main() {
    Node* root = NULL; //the root node of the tree, empty until something is added

    //welcome message with instructions, also sets precision to 3 decimal points for the average function
    cout << "\nI've been expecting you. I am Rubert the Red-Black Tree, and I maintain the balance for integers 1 through 999.\n\nWhat would you like to do? (Type HELP for help)" << fixed << setprecision(3);

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
            cout << "\nThis command be unimplemented at the moment."; //removeInt(root);
        } else if (command == "SEARCH") { //find a given int in the tree
            searchInt(root);
        } else if (command == "PRINT") { //print visualization of tree
            printNode(root, "", false, true);
        } else if (command == "AVERAGE") { //print average of all ints
            printAverage(root);
        } else if (command == "HELP") { //print all valid command words
            cout << "\nYour command words are:\nADD     - Manually insert one or more integers (1-999).\nREAD    - Read in a string of integers (1-999) from a file.\nREMOVE  - Doesn't do anything right now.\nSEARCH  - Find an integer in the tree.\nAVERAGE - Calculate the average of all integers.\nHELP    - Print all valid commands.\nQUIT    - Exit the program.";
        } else if (command == "QUIT") { //quit the program
            continuing = false; //leave the main player loop
        } else { //give error message if the user typed something unacceptable
            cout << "\nInvalid command \"" << command << "\". (type HELP for help)";
        }
    }

    //says a friendly farewell, wishing you a nice day
    cout << "\nEnjoy your next 24 hours.\n";

    //recursively delete all the nodes for good practice, starting from the root
    deleteAll(root);
}
