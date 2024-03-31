// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "dbFunctions.h"
#include "TableEntry.h"
#include <getopt.h>

using namespace std;

Tab* LOC (std::unordered_map<std::string, Tab*> &tables, std::string tablename);
TableEntry PRODUCE (string type);

int main (int argc, char* argv[]) {
    // step one: setup
    unordered_map<string, Tab*> tables;
    //unordered_map<string, Index*> indices;
    ios_base::sync_with_stdio(false);
    cout.clear();
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
    while (cin >> line) {
        char c = line[0];
        switch (c) {
            case 'C' : { //CREATE
                string junk, tablename;
                int N = 0;
                cin >> tablename >> N;
                vector<string> types, names;
                for (int q = 0; q < N; q++) {
                    cin >> junk;
                    types.push_back(junk);
                } for (int q = 0; q < N; q++) {
                    cin >> junk;
                    names.push_back(junk);
                }
                Tab* t = new Tab(tablename, types, names, quiet);
                tables.emplace(tablename,t);
                break;
            } case 'Q': { //QUIT
                for (auto & [ key, value ] : tables) {
                    delete value;
                }
                cout << "Thanks for being silly!" << endl;
                return 0;
                break;
            } case '#': { //#COMMENT
                string junk;
                getline(cin, junk);
                break;
            } case 'R': { //REMOVE
                string junk, tablename;
                cin >> tablename;
                delete tables[tablename];
                tables.erase(tablename);
                cout << "Table " << tablename << " removed" << endl;
                break;
            } case 'I': { //INSERT
                string junk, tablename;
                int N = 0;
                cin >> junk >> tablename >> N >> junk;
                Tab* target = nullptr;
                try {
                    target = LOC(tables, tablename);
                }
                catch (const exception& e) {
                    cout << "Error during INSERT: " << e.what() << endl;
                } 
                target->insert(N);
                break;
            } case 'P': { //PRINT
                string junk, tablename;
                int N = 0;
                cin >> junk >> tablename >> N;
                vector<string> colnames;
                Tab* target = nullptr;
                try {
                    target = LOC(tables, tablename);
                } catch (const exception& e) {
                    cout << "Error during PRINT: " << e.what() << endl;
                    break;
                }
                bool quiet = target->quiet;
                for (size_t n = 0; n < size_t(N); n++) {
                    cin >> junk;
                    try {target->findCol(junk);}
                    catch (const exception& e) { 
                        cout << "Error during PRINT: " << e.what() << endl;
                        break;
                    }
                    colnames.push_back(junk);
                }
                cin >> junk;
                size_t M = 0;
                if (junk == "WHERE") {
                    string colname;
                    char OP;
                    size_t col = 0;
                    cin >> colname >> OP;
                    try {col = target->findCol(colname);}
                    catch (const exception& e) { 
                        cout << "Error during PRINT: " << e.what() << endl;
                        break;
                    }
                    string type = target->findType(col);
                    ColComp comp(col, OP, PRODUCE(type), target);
                    M = target->print(colnames, quiet, comp);
                } else {
                    M = target->print(colnames, quiet);
                }
                cout << "Printed " << M << " matching rows from " << tablename << endl;
                break;
            } case 'D': { //DELETE
                string junk, tablename, colname;
                char OP;
                size_t col = 0;
                cin >> junk >> tablename >> junk >> colname >> OP;
                Tab* target = nullptr;
                try {
                    target = LOC(tables, tablename);
                } catch (const exception& e) {
                    cout << "Error during DELETE: " << e.what() << endl;
                    break;
                } try {
                    col = target->findCol(colname);
                } catch (const exception& e) {
                    cout << "Error during DELETE: " << e.what() << endl;
                    break;
                }
                string type = target->findType(col);
                ColComp comp(col, OP, PRODUCE(type), target);
                //target->makeIndex(true, colname);
                size_t M = 0;
                try {
                    M = target->sift(/*colname, */comp);
                } catch (const exception& e) {
                    cout << "Error during INSERT: " << e.what() << endl;
                    break;
                }
                cout << "Deleted " << M << " rows from " << tablename << endl;
                break;
            } case 'J': { //JOIN
                string junk, tablename1, tablename2, colname1, colname2;
                size_t N = 0;
                size_t i_1 = 0;
                size_t i_2 = 0;
                vector<size_t> cols;
                vector<bool> modes;
                cin  >> tablename1 >> junk >> tablename2 
                    >> junk >> colname1 >> junk >> colname2
                    >> junk >> junk >> N;
                Tab* target1 = nullptr;
                Tab* target2 = nullptr;
                try {
                    target1 = LOC(tables, tablename1);
                } catch (const exception& e) {
                    cout << "Error during JOIN: " << e.what() << endl;
                    break;
                } try {
                    target2 = LOC(tables, tablename2);
                } catch (const exception& e) {
                    cout << "Error during JOIN: " << e.what() << endl;
                    break;
                }
                try {
                    i_1 = target1->findCol(colname1);
                } catch (const exception& e) {
                    cout << "Error during JOIN: " << e.what() << endl;
                    break;
                }
                try {
                    i_2 = target2->findCol(colname2);
                } catch (const exception& e) {
                    cout << "Error during JOIN: " << e.what() << endl;
                    break;
                }
                for (size_t c = 0; c < N; c++) {
                    string temp_name;
                    size_t temp_mode = 0;
                    cin >> temp_name >> temp_mode;
                    size_t temp_i = 0;
                    try {
                        if(temp_mode == 1) temp_i = target1->findCol(temp_name);
                        else               temp_i = target2->findCol(temp_name);
                    } catch (const exception& e) {
                        cout << "Error during JOIN: " << e.what() << endl;
                        break;
                    }
                    cols.push_back(temp_i);
                    modes.push_back((temp_mode == 2));
                }
                target1->join(target2, i_1, i_2, cols, modes, quiet);
                break;
            } case 'G': { //GENERATE
                string junk, tablename, indextype, colname;
                cin >> junk >> tablename >> indextype >> junk >> junk >> colname;
                Tab* target = nullptr;
                try {
                    target = LOC(tables, tablename);
                } catch (const exception& e) {
                    cout << "Error during GENERATE: " << e.what() << endl;
                    break;
                }
                size_t y = 0;
                try {
                    y = target->findCol(colname);
                } catch (const exception& e) {
                    cout << "Error during GENERATE: " << e.what() << endl;
                    break;
                }
                //target->makeIndex((indextype == "bst"), colname);
                if (target->i == nullptr) {
                    target->i = new Index((indextype == "bst"), y, target);
                } else {
                    target->i->reindex((indextype == "bst"), y, target);
                } cout << "Created " << indextype << " index for table " << tablename << " on column " << colname << ", with " << target->i->size() << " distinct keys\n"; 
                break;
            } default: { //Error: unrecognized command
                cout << "Error: unrecognized command" << ": \"" << line << "\"" << endl;
                break;
            }
        }
        //cin >> line;
        cout << "% ";
    }

    //failsafe quit (just in case)
    for (auto & [ key, value ] : tables) {
        delete value;
    }
    cerr << "exited without QUIT()\n";
    return 1;
}