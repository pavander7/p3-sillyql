// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "dbFunctions.h"
#include "TableEntry.h"
#include <getopt.h>

using namespace std;

void CREATE (std::unordered_map<std::string, Tab*> &tables, std::string command, bool quiet);
void REMOVE (std::unordered_map<std::string, Tab*> &tables, std::string command);
void QUIT (std::unordered_map<std::string, Tab*> &tables);

void INSERT (std::unordered_map<std::string, Tab*> &tables, std::string command);
void PRINT (std::unordered_map<std::string, Tab*> &tables, std::string command);
void DELETE (std::unordered_map<std::string, Tab*> &tables, std::string command);
void JOIN (std::unordered_map<std::string, Tab*> &tables, std::string command, bool quiet);
void GENERATE (std::unordered_map<std::string, Tab*> &tables, std::unordered_map<std::string, Index*> &indices, std::string command);

int main (int argc, char* argv[]) {
    // step one: setup
    unordered_map<string, Tab*> tables;
    unordered_map<string, Index*> indices;
    ios_base::sync_with_stdio(false);
    cin >> std::boolalpha;
    cout << std::boolalpha;

    //cout << "checkpoint 1: beginning \n";

    static struct option long_options[] = {
    {"help",        no_argument,        NULL,  'h'},
    {"quiet",       no_argument,        NULL,  'q'},
    {0,             0,                  NULL,  0, }
    };
    int option_index = 0;

    //variables for input processing
    bool quiet = false;

    int c = getopt_long(argc, argv, "hq", long_options, &option_index);
    while (c != -1) {
        switch(c) {
            case 'h' :
                cout << "help message";
                return 0;
                break;
            case 'q' :
                quiet = true;
                break;
        }
        c = getopt_long(argc, argv, "hq", long_options, &option_index);
    }


    // step two: loop
    string line;
    cout << "% ";
    while (getline(cin, line)) {
        char c = line[0];
        switch (c) {
            case 'C':
                CREATE(tables, line, quiet);
                break;
            case 'Q':
                QUIT(tables);
                return 0;
                break;
            case '#':
                break;
            case 'R':
                REMOVE(tables, line);
                break;
            case 'I':
                INSERT(tables, line);
                break;
            case 'P':
                PRINT(tables, line);
                break;
            case 'D':
                DELETE(tables, line);
                break;
            case 'J':
                JOIN(tables, line, quiet);
                break;
            case 'G':
                GENERATE(tables, indices, line);
                break;
            default:
                cout << "Error: unrecognized command\n";
                break;
        }
        cout << "% ";
    }

    cerr << "exited without QUIT()\n";
    QUIT(tables);
    return(1);
}