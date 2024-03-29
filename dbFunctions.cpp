// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "TableEntry.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "dbFunctions.h"

using namespace std;

Tab* LOC (std::unordered_map<std::string, Tab*> &tables, std::string tablename);

//database functions
void CREATE (std::unordered_map<std::string, Tab*> &tables, std::string command) {
    std::string junk, tablename;
    stringstream ss(command);
    int N = 0;
    ss >> junk >> tablename >> N;
    vector<string> types, names;
    for (int q = 0; q < N; q++) {
        ss >> junk;
        types.push_back(junk);
    } for (int q = 0; q < N; q++) {
        ss >> junk;
        names.push_back(junk);
    }
   Tab* t = new Tab(tablename, types, names);
    tables.emplace(tablename,t);
}
void QUIT (std::unordered_map<std::string, Tab*> &tables) {
    for (auto & [ key, value ] : tables) {
        delete value;
    }
    cout << "Thanks for being silly!\n";
}
void REMOVE (std::unordered_map<std::string, Tab*> &tables, string command) {
    string junk, tablename;
    stringstream ss(command);
    ss >> junk >> tablename;
    tables.erase(tablename);
    cout << "Table " << tablename << " removed" << endl;
}
//database functions

//auxiliary functions
Tab* LOC (std::unordered_map<std::string, Tab*> &tables, string tablename) {
    auto target = tables.find(tablename);
    if (target == tables.end()) throw runtime_error(tablename + " does not name a table in the database");
    return target->second;
}
//auxiliary functions

//table interface functions
void INSERT (std::unordered_map<std::string, Tab*> &tables, string command) {
    string junk, tablename;
    stringstream ss(command);
    int N = 0;
    ss >> junk >> junk >> tablename >> N;
    Tab* target = nullptr;
    try {
        target = LOC(tables, tablename);
    }
    catch (const exception& e) {
        cout << "Error during INSERT: " << e.what() << endl;
    }
    target->insert(N);
}
void PRINT (std::unordered_map<std::string, Tab*> &tables, string command) {
    string junk, tablename;
    stringstream ss(command);
    int N = 0;
    ss >> junk >> junk >> tablename >> N;
    vector<string> colnames;
    for (size_t n = 0; n < size_t(N); n++) {
        ss >> junk;
        colnames.push_back(junk);
    } 
    Tab* target = LOC(tables, tablename);
    string colname;
    char OP;
    int val;
    ss >> colname;
    size_t M = 0;
    if (ss >> OP) {
        ss >> val;
        ColComp comp(colname, OP, val, target);
        M = target->print(colnames, comp);
    } else {
        M = target->print(colnames);
    }
    cout << "Printed " << M << " matching rows from " << tablename << endl;
}
void DELETE (std::unordered_map<std::string, Tab*> &tables, string command) {
    string junk, tablename, col;
    char OP;
    int val;
    stringstream ss(command);
    ss >> junk >> junk >> tablename >> junk >> col >> OP >> val;
    Tab* target = LOC(tables, tablename);
    ColComp comp(col, OP, val, target);
    target->makeIndex(true, col);
    size_t N = 0;
    try {
        N = target->sift(col, comp);
    } catch (const exception& e) {
        cout << "Error during INSERT: " << e.what() << endl;
    } 
    cout << "Deleted " << N << " rows from " << tablename << endl;
}
//table interface functions

//TAB FUNCTIONS
Tab::Tab(std::string tablename, std::vector<std::string> types_in, std::vector<std::string> names_in) : name(tablename), types(types_in), names(names_in) {
    cout << "New table " << name << " with column(s) ";
    for (auto t : names) {
        cout << t << " ";
    } cout << "created\n";
}
Tab::Tab(const Tab& other) {
    name = other.name;
    types = other.types;
    names = other.names;
    i = new Index(names, data);
    for (auto her : other.data) {
        data.push_back(her);
    }
}
Tab &Tab::operator=(const Tab& rhs) {
    name = rhs.name;
    types = rhs.types;
    names = rhs.names;
    i = new Index(names, data);
    for (auto her : rhs.data) {
        data.push_back(her);
    }
    return *this;
}
void Tab::insert(int N) {
    size_t startN = data.size();
    for (int r = 0; r < N; r++) {
        string line;
        getline(cin, line);
        Row temp(types, line);
        data.push_back(temp);
    } size_t endN = data.size() - size_t(1);
    cout << "Added " << N << " rows to " << name << " from position " << startN << " to " << endN << endl;
}
size_t Tab::print(vector<std::string> cols) {
    //map<string,vector<TableEntry>> her(data.begin(), data.end());
    vector<size_t> iCols;
    size_t M = 0;
    for (auto mother : cols) {
        if (!iCols.empty()) cout << " ";
        cout << mother;
        for(size_t q = 0; q < names.size(); q++) {
            if (names[q] == mother) {
                iCols.push_back(q);
                break;
            }
        } 
    } cout << endl;
    for (auto rupaul : data) {
        bool first = true;
        for (size_t him : iCols) {
            if (!first) { cout << " ";}
            else first = false;
            cout << rupaul[him];
        } cout << endl;
        M++;
    } return M;
}
size_t Tab::print(vector<std::string> cols, ColComp comp) {
        //map<string,vector<TableEntry>> her(data.begin(), data.end());
        vector<size_t> iCols;
        size_t M = 0;
        for(auto mother : cols) {
            if (!iCols.empty()) cout << " ";
            cout << mother;
            for(size_t q = 0; q < names.size(); q++) {
                if (names[q] == mother) {
                    iCols.push_back(q);
                    break;
                }
            } 
        } cout << endl;
        for(size_t q = 0; q < data.size(); q++) {
            Row* rupaul = &data[q];
            if (comp(rupaul)) {
                bool first = true;
                for (size_t him : iCols) {
                    if (!first) { cout << " "; first = false; }
                    cout << &rupaul[him];
                } cout << endl;
                M++;
            }
        } return M;
    }
void Tab::makeIndex(bool order, string col) {
   this->i->reindex(order, col, this->names, data); 
}
size_t Tab::findCol (string colname) {
    size_t col = 0;
    for(size_t q = 0; q < names.size(); q++) {
        if (names[q] == colname) {
            col = q;
            return col;
        }
    } throw runtime_error(colname + " does not name a column in " + name);
    return data.size();
}
size_t Tab::sift(string col, ColComp comp) {
    size_t N = 0;
    size_t w = 0;
    try {
        w = findCol(col);
    } catch (const exception& e) {
        throw e;
    }
    vector<Tab::Row> temp;
    for (Tab::Row her : data) {
        if (!comp(w)) {
            temp.push_back(her);
        } else N++;
    } data.swap(temp);
    return N;
}
//TAB FUNCTIONS

//ROW FUNCTIONS
Tab::Row::Row(vector<std::string> &types, std::string line) {
    stringstream ss(line);
    for (size_t w = 0; w < types.size(); w++) {
        if (types[w] == "double") {
            double temp;
            ss >> temp;
            rowData.emplace_back(temp);
        } else if (types[w] == "string") {
            string temp;
            ss >> temp;
            rowData.emplace_back(temp);
        } else if (types[w] == "int") {
            int temp;
            ss >> temp;
            rowData.emplace_back(temp);
        } else if (types[w] == "bool") {
            bool temp;
            ss >> temp;
            rowData.emplace_back(temp);
        }
    }
}
//ROW FUNCTIONS

//COMP FUNCTIONS
ColComp::ColComp(string colname_in, char OP_in, int val_in, Tab* database) 
    : val(val_in), OP(OP_in) {
    this->data = &(database->data);
    try {
        col = database->findCol(colname_in);
    } catch (const exception& e) {

    }
}
bool ColComp::operator() (size_t i) {
    Tab::Row *ptr = &((*data)[i]);
    TableEntry datum = ptr->at(col);
    switch (OP) {
        case '=' :
            return (datum == val);
            break;
        case '<' :
            return (datum < val);
            break;
        case '>' :
            return (datum > val);
            break;
    } throw runtime_error("unknown OP"); 
    return 'x';
}
bool ColComp::operator() (Tab::Row *ptr) {
    TableEntry datum = ptr->at(col);
    switch (OP) {
        case '=' :
            return (datum == val);
            break;
        case '<' :
            return (datum < val);
            break;
        case '>' :
            return (datum > val);
            break;
    } throw runtime_error("unknown OP"); 
    return 'x';
}
//COMP FUNCTIONS

//INDEX FUNCTIONS
Tab::Row* &Index::operator() (TableEntry x) {
    if (order) return (o[x]);
    else return (u[x]);
}
Index::Index(vector<string> names, vector<Tab::Row> &rawData) {
    order = true;
    reindex(order, "name", names, rawData);
}
void Index::reindex(bool order_in, string col, vector<string> names, vector<Tab::Row> &rawData) {
    size_t c = 0;
    while (names[c] != col) c++;
    if (order_in) {
        map<TableEntry, Tab::Row*> temp;
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            temp[her->at(n)] = her;
        }
        o.swap(temp);
        order = true;
    } else {
        unordered_map<TableEntry, Tab::Row*> temp;
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            temp[her->at(n)] = her;
        }
        u.swap(temp);
        order = false;
    }
}
//INDEX FUNCTIONS