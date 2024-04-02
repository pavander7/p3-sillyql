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
    ios_base::sync_with_stdio(false);
    std::cout.clear();
    cin >> std::boolalpha;
    std::cout << std::boolalpha;

    //std::cout << "checkpoint 1: beginning \n";

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
                std::cout << "help message";
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
    std::cout << "% ";
    while (cin >> line) {
        char c = line[0];
        switch (c) {
            case 'C' : { //CREATE
                string junk, tablename;
                size_t N = 0;
                cin >> tablename >> N;
                vector<string> types, names;
                types.reserve(N);
                names.reserve(N);
                for (size_t q = 0; q < N; q++) {
                    cin >> junk;
                    types.push_back(junk);
                } for (size_t q = 0; q < N; q++) {
                    cin >> junk;
                    names.push_back(junk);
                }
                if (tables.count(tablename) != 0) {
                    std::cout << "Error during CREATE: Cannot create already existing table " << tablename << endl;
                    break;
                } else {
                    Tab* t = new Tab(tablename, types, names, quiet);
                    tables.emplace(tablename,t);
                    break;
                } break;
            } case 'Q': { //QUIT
                for (auto & [ key, value ] : tables) {
                    delete value;
                }
                std::cout << "Thanks for being silly!" << endl;
                return 0;
                break;
            } case '#': { //#COMMENT
                string junk;
                getline(cin, junk);
                break;
            } case 'R': { //REMOVE
                string junk, tablename;
                cin >> tablename;
                if (tables.count(tablename) == 0) {
                    std::cout << "Error during REMOVE: " << tablename << " does not name a table in the database\n";
                    break;
                } else {
                    delete tables[tablename];
                    tables.erase(tablename);
                    std::cout << "Table " << tablename << " removed" << endl;
                    break;
                } break;
            } case 'I': { //INSERT
                string junk, tablename;
                size_t N = 0;
                cin >> junk >> tablename >> N >> junk;
                Tab* target = nullptr;
                target = LOC(tables, tablename);
                if (target == nullptr) {
                    std::cout << "Error during INSERT: " << tablename << " does not name a table in the database\n";
                    for (size_t n = 0; n < N; n++) {
                        getline (cin, junk);
                    }
                    break;
                }
                target->insert(N);
                break;
            } case 'P': { //PRINT
                string junk, tablename;
                size_t N = 0;
                cin >> junk >> tablename >> N;
                vector<string> colnames;
                Tab* target = nullptr;
                target = LOC(tables, tablename);
                if (target == nullptr) {
                    std::cout << "Error during PRINT: " << tablename << " does not name a table in the database\n";
                    getline (cin, junk);
                    break;
                }
                bool quiet = target->quiet;
                bool err = false;
                for (size_t n = 0; n < N; n++) {
                    cin >> junk;
                    size_t w = target->findCol(junk);
                    if (w == target->width()) { 
                        std::cout << "Error during PRINT: " << junk << " does not name a column in " << target->name << endl;
                        err = true;
                        getline (cin, junk);
                        break;
                    }
                    colnames.push_back(junk);
                } if (err) {
                    //getline (cin, junk);
                    break;
                } 
                cin >> junk;
                size_t M = 0;
                if (junk == "WHERE") {
                    string colname;
                    char OP;
                    size_t col = 0;
                    cin >> colname >> OP;
                    col = target->findCol(colname);
                    if (col == target->width()) { 
                        std::cout << "Error during PRINT: " << colname << " does not name a column in " << target->name << endl;
                        getline (cin, junk);
                        break;
                    }
                    string type = target->findType(col);
                    M = target->print(colnames, quiet, col, OP, PRODUCE(type));
                } else {
                    M = target->size();
                    if (!quiet) target->print(colnames);
                }
                std::cout << "Printed " << M << " matching rows from " << tablename << endl;
                break;
            } case 'D': { //DELETE
                string junk, tablename, colname;
                char OP;
                size_t col = 0;
                cin >> junk >> tablename >> junk >> colname >> OP;
                Tab* target = nullptr;
                target = LOC(tables, tablename);
                if (target == nullptr) {
                    std::cout << "Error during DELETE: " << tablename << " does not name a table in the database\n";
                    break;
                }
                col = target->findCol(colname);
                if (col == target->width()) {
                    std::cout << "Error during DELETE: " << colname << " does not name a column in " << target->name << endl;
                    break;
                }
                string type = target->findType(col);
                ColComp comp(col, OP, PRODUCE(type), target);
                size_t M = 0;
                M = target->sift(comp);
                std::cout << "Deleted " << M << " rows from " << tablename << endl;
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
                target1 = LOC(tables, tablename1);
                if (target1 == nullptr) {
                    std::cout << "Error during JOIN: " << tablename1 << " does not name a table in the database\n";
                    getline(cin,junk);
                    break;
                } target2 = LOC(tables, tablename2);
                if (target2 == nullptr) {
                    std::cout << "Error during JOIN: " << tablename2 << " does not name a table in the database\n";
                    getline(cin,junk);
                    break;
                } i_1 = target1->findCol(colname1);
                if (i_1 == target1->width()) {
                    std::cout << "Error during JOIN: " << colname1 << " does not name a column in " << target1->name << endl;
                    getline(cin,junk);
                    break;
                } i_2 = target2->findCol(colname2);
                if (i_2 == target2->width()) {
                    std::cout << "Error during JOIN: " << colname2 << " does not name a column in " << target2->name << endl;
                    getline(cin,junk);
                    break;
                } bool err = false;
                for (size_t c = 0; c < N; c++) {
                    string temp_name;
                    size_t temp_mode = 0;
                    cin >> temp_name >> temp_mode;
                    size_t temp_i = 0;
                    if(temp_mode == 1) temp_i = target1->findCol(temp_name);
                    else               temp_i = target2->findCol(temp_name);
                    if ((temp_mode == 1)&&(temp_i == target1->width())) {
                        std::cout << "Error during JOIN: " << temp_name << " does not name a column in " << target1->name << endl;
                        getline(cin,junk);
                        err = true;
                        break;
                    } else if ((temp_mode == 2)&&(temp_i == target2->width())) {
                        std::cout << "Error during JOIN: " << temp_name << " does not name a column in " << target2->name << endl;
                        getline(cin,junk);
                        err = true;
                        break;
                    }
                    cols.push_back(temp_i);
                    modes.push_back((temp_mode == 2));
                } if (err) break;
                target1->join(target2, i_1, i_2, cols, modes, quiet);
                break;
            } case 'G': { //GENERATE
                string junk, tablename, indextype, colname;
                cin >> junk >> tablename >> indextype >> junk >> junk >> colname;
                Tab* target = nullptr;
                target = LOC(tables, tablename);
                if (target == nullptr) {
                    std::cout << "Error during GENERATE: " << tablename << " does not name a table in the database\n";
                    break;
                }
                size_t y = 0;
                y = target->findCol(colname);
                if (y == target->width()) {
                    std::cout << "Error during GENERATE: " << tablename << " does not name a table in the database\n";
                    break;
                }
                target->indexify((indextype == "bst"), y);
                std::cout << "Created " << indextype << " index for table " << tablename << " on column " << colname << ", with " << target->i->size() << " distinct keys\n"; 
                break;
            } default: { //Error: unrecognized command
                std::cout << "Error: unrecognized command" << ": \"" << line << "\"" << endl;
                string junk;
                getline (cin, junk);
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