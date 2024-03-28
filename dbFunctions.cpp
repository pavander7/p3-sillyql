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
void CREATE (std::unordered_map<std::string, Tab*> &tables, std::string command, bool quiet) {
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
   Tab* t = new Tab(tablename, types, names, quiet);
    tables.emplace(tablename,t);
}
void QUIT (std::unordered_map<std::string, Tab*> &tables) {
    for (auto & [ key, value ] : tables) {
        delete value;
    }
    cout << "Thanks for being silly!" << endl;
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
    Tab* target = nullptr;
    try {
        target = LOC(tables, tablename);
    } catch (const exception& e) {
        cout << "Error during PRINT: " << e.what() << endl;
        return;
    }
    bool quiet = target->quiet;
    for (size_t n = 0; n < size_t(N); n++) {
        ss >> junk;
        try {target->findCol(junk);}
        catch (const exception& e) { 
            cout << "Error during PRINT: " << e.what() << endl;
            return;
        }
        colnames.push_back(junk);
    }
    ss >> junk;
    size_t M = 0;
    if (junk == "WHERE") {
        string colname;
        char OP;
        size_t col = 0;
        ss >> colname >> OP;
        try {col = target->findCol(colname);}
        catch (const exception& e) { 
            cout << "Error during PRINT: " << e.what() << endl;
            return;
        }
        string type = target->findType(col);
        TableEntry* val = nullptr;
        if (type == "double") {
            double temp = 0.0;
            ss >> temp;
            val = new TableEntry{temp};
        } else if (type == "string") {
            string temp;
            ss >> temp;
            val = new TableEntry{temp};
        } else if (type == "int") {
            int temp = 0;
            ss >> temp;
            val = new TableEntry{temp};
        } else if (type == "bool") {
            string word;
            ss >> word;
            bool temp = (word == "true");
            val = new TableEntry{temp};
        }
        ColComp comp(col, OP, *val, target);
        M = target->print(colnames, quiet, comp);
        delete val;
    } else {
        M = target->print(colnames, quiet);
    }
    cout << "Printed " << M << " matching rows from " << tablename << endl;
}
void DELETE (std::unordered_map<std::string, Tab*> &tables, string command) {
    string junk, tablename, colname;
    char OP;
    size_t col = 0;
    stringstream ss(command);
    ss >> junk >> junk >> tablename >> junk >> colname >> OP;
    Tab* target = nullptr;
    try {
        target = LOC(tables, tablename);
    } catch (const exception& e) {
        cout << "Error during DELETE: " << e.what() << endl;
        return;
    } try {
        col = target->findCol(colname);
    } catch (const exception& e) {
        cout << "Error during DELETE: " << e.what() << endl;
        return;
    }
    string type = target->findType(col);
    TableEntry* val = nullptr;
    if (type == "double") {
        double temp;
        ss >> temp;
        val = new TableEntry{temp};
    } else if (type == "string") {
        string temp;
        ss >> temp;
        val = new TableEntry{temp};
    } else if (type == "int") {
        int temp;
        ss >> temp;
        val = new TableEntry{temp};
    } else if (type == "bool") {
        string word;
        ss >> word;
        bool temp = (word == "true");
        val = new TableEntry{temp};
    }
    ColComp comp(col, OP, *val, target);
    //target->makeIndex(true, colname);
    size_t M = 0;
    try {
        M = target->sift(colname, comp);
    } catch (const exception& e) {
        cout << "Error during INSERT: " << e.what() << endl;
        return;
    } 
    delete val;
    cout << "Deleted " << M << " rows from " << tablename << endl;
}
void GENERATE (std::unordered_map<std::string, Tab*> &tables, std::unordered_map<std::string, Index*> &indices, string command) {
    string junk, tablename, indextype, colname;
    stringstream ss(command);
    ss >> junk >> junk >> tablename >> indextype >> junk >> junk >> colname;
    Tab* target = nullptr;
    try {
        target = LOC(tables, tablename);
    } catch (const exception& e) {
        cout << "Error during GENERATE: " << e.what() << endl;
        return;
    }
    size_t i = 0;
    try {
        i = target->findCol(colname);
    } catch (const exception& e) {
        cout << "Error during GENERATE: " << e.what() << endl;
        return;
    }
    //target->makeIndex((indextype == "bst"), colname);
    if (indices.count(tablename) == 0) {
        Index* her = new Index((indextype == "bst"), i, target);
        indices.emplace(tablename, her);
    } else {
        indices[tablename]->reindex((indextype == "bst"), i, target);
    } cout << "Created " << indextype << " index for table " << tablename << " on column " << colname << ", with " << indices[tablename]->size() << " distinct keys\n"; 
}
void JOIN (unordered_map<string, Tab*> &tables, string command, bool quiet) {
    string junk, tablename1, tablename2, colname1, colname2;
    size_t N = 0;
    size_t i_1 = 0;
    size_t i_2 = 0;
    vector<size_t> cols;
    vector<bool> modes;
    stringstream ss(command);
    ss  >> junk >> tablename1 >> junk >> tablename2 
        >> junk >> colname1 >> junk >> colname2
        >> junk >> junk >> N;
    Tab* target1 = nullptr;
    Tab* target2 = nullptr;
    try {
        target1 = LOC(tables, tablename1);
    } catch (const exception& e) {
        cout << "Error during JOIN: " << e.what() << endl;
        return;
    } try {
        target2 = LOC(tables, tablename2);
    } catch (const exception& e) {
        cout << "Error during JOIN: " << e.what() << endl;
        return;
    }
    try {
        i_1 = target1->findCol(colname1);
    } catch (const exception& e) {
        cout << "Error during JOIN: " << e.what() << endl;
        return;
    }
    try {
        i_2 = target2->findCol(colname2);
    } catch (const exception& e) {
        cout << "Error during JOIN: " << e.what() << endl;
        return;
    }
    for (size_t c = 0; c < N; c++) {
        string temp_name;
        size_t temp_mode = 0;
        ss >> temp_name >> temp_mode;
        size_t temp_i = 0;
        try {
            if(temp_mode == 1) temp_i = target1->findCol(temp_name);
            else               temp_i = target2->findCol(temp_name);
        } catch (const exception& e) {
            cout << "Error during JOIN: " << e.what() << endl;
            return;
        }
        cols.push_back(temp_i);
        modes.push_back((temp_mode == 2));
    }
    target1->join(target2, i_1, i_2, cols, modes, quiet);
}
//table interface functions

//TAB FUNCTIONS
Tab::Tab(std::string tablename, std::vector<std::string> types_in, std::vector<std::string> names_in, bool quiet_in) : name(tablename), quiet(quiet_in), types(types_in), names(names_in) {
    cout << "New table " << name << " with column(s) ";
    for (auto t : names) {
        cout << t << " ";
    } cout << "created\n";
}
Tab::Tab(const Tab& other) {
    name = other.name;
    types = other.types;
    names = other.names;
    quiet = other.quiet;
    //i = new Index(names, data);
    for (auto her : other.data) {
        data.push_back(her);
    }
}
Tab &Tab::operator=(const Tab& rhs) {
    name = rhs.name;
    types = rhs.types;
    names = rhs.names;
    quiet = rhs.quiet;
    //i = new Index(names, data);
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
size_t Tab::print(vector<std::string> cols, bool quiet) {
    //map<string,vector<TableEntry>> her(data.begin(), data.end());
    vector<size_t> iCols;
    size_t M = 0;
    for (auto mother : cols) {
        cout << mother << " ";
        for(size_t q = 0; q < names.size(); q++) {
            if (names[q] == mother) {
                iCols.push_back(q);
                break;
            }
        } 
    } cout << endl;
    for (auto rupaul : data) {
        for (size_t him : iCols) {
            if (!quiet) cout << rupaul[him] << " ";
        } if (!quiet) cout << endl;
        M++;
    } return M;
}
size_t Tab::print(vector<std::string> cols, bool quiet, ColComp comp) {
        //map<string,vector<TableEntry>> her(data.begin(), data.end());
        vector<size_t> iCols;
        size_t M = 0;
        for(auto mother : cols) {
            cout << mother << " ";
            for(size_t q = 0; q < names.size(); q++) {
                if (names[q] == mother) {
                    iCols.push_back(q);
                    break;
                }
            } 
        } cout << endl;
        for(auto rupaul : data) {
            if (comp(&rupaul)) {
                for (size_t him : iCols) {
                    if (!quiet) cout << rupaul[him] << " ";
                } if (!quiet) cout << endl;
                M++;
            }
        } return M;
    }
/*void Tab::makeIndex(bool order, string col) {
   this->i-> 
}*/
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
    for (size_t a = 0; a < data.size(); a++) {
        if (!comp(a)) {
            temp.push_back(data[a]);
        } else N++;
    } data.swap(temp);
    return N;
}
void Tab::join (Tab* other, std::size_t col1, std::size_t col2, 
        std::vector<std::size_t> cols, std::vector<bool> modes, bool quiet) {
    size_t M = 0;
    if (!quiet) {
        for (size_t u = 0; u < cols.size(); u++) {
            if (modes[u]) cout << names[cols[u]] << ' ';
            else cout << other->names[cols[u]] << ' ';
        } cout << endl;
    } for (Row* her = &(data.front()); her != &*(data.end()); her++) {
        ColComp comp(col2, '=', (*her)[col1], other);
        for (Row* him = &(other->data.front()); him != &*(other->data.end()); him++) {
            if (comp(him)) {
                M++;
                if (!quiet) {
                    for (size_t u = 0; u < cols.size(); u++) {
                        if (modes[u]) cout << (*her)[cols[u]] << ' ';
                        else cout << (*him)[cols[u]] << ' ';
                    } cout << endl;
                }
            }
        }
    } cout << "Printed " << M << " rows from joining " << this->name << " and " << other->name << endl;
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
            string word;
            ss >> word;
            bool temp = (word == "true");
            rowData.emplace_back(temp);
        }
    }
}
//ROW FUNCTIONS

//COMP FUNCTIONS
ColComp::ColComp(string colname_in, char OP_in, TableEntry val_in, Tab* database) 
    : val(val_in), OP(OP_in) {
    this->data = &(database->data);
    try {
        col = database->findCol(colname_in);
    } catch (const exception& e) {
        throw e; 
    }
}
ColComp::ColComp(size_t col_in, char OP_in, TableEntry val_in, Tab* database) 
    : col(col_in), val(val_in), OP(OP_in) {
    this->data = &(database->data);
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
/*Index::Index(vector<string> names, vector<Tab::Row> &rawData) {
    order = true;
    reindex(order, "name", names, rawData);
}*/
Index::Index(bool order_in, size_t col, vector<Tab::Row> &rawData) {
    if (order_in) {
        //map<TableEntry, Tab::Row*> temp;
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            o[her->at(col)] = her;
        }
        //o.swap(temp);
        order = true;
    } else {
        //unordered_map<TableEntry, Tab::Row*> temp;
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            u[her->at(col)] = her;
        }
        //u.swap(temp);
        order = false;
    }
}
Index::Index(bool order_in, size_t col, Tab* target) {
    vector<Tab::Row> rawData = target->data;
    if (order_in) {
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            o[her->at(col)] = her;
        }
        order = true;
    } else {
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            u[her->at(col)] = her;
        }
        order = false;
    }
}
void Index::reindex(bool order_in, size_t col, Tab* target) {
    vector<Tab::Row> rawData = target->data;
    vector<string> names = target->names;
    o.clear();
    u.clear();
    if (order_in) {
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            o[her->at(col)] = her;
        }
        order = true;
    } else {
        for (size_t n = 0; n < rawData.size(); n++) {
            Tab::Row* her = &rawData[n];
            u[her->at(col)] = her;
        }
        order = false;
    }
}
//INDEX FUNCTIONS