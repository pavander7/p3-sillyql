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

//auxiliary functions
Tab* LOC (std::unordered_map<std::string, Tab*> &tables, string tablename) {
    auto target = tables.find(tablename);
    if (target == tables.end()) throw runtime_error(tablename + " does not name a table in the database");
    return target->second;
}
TableEntry PRODUCE (string type) {
    if (type == "double") {
        double temp = 0.0;
        cin >> temp;
        return TableEntry{temp};
    } else if (type == "string") {
        string temp;
        cin >> temp;
        return TableEntry{temp};
    } else if (type == "int") {
        int temp = 0;
        cin >> temp;
        return TableEntry{temp};
    } else if (type == "bool") {
        string word;
        cin >> word;
        bool temp = (word == "true");
        return TableEntry{temp};
    } else {
        throw runtime_error("unknown type\n");
        return TableEntry{1};
    }
}
//auxiliary functions

//TAB FUNCTIONS
Tab::Tab(std::string tablename, std::vector<std::string> types_in, std::vector<std::string> names_in, bool quiet_in) : 
        name(tablename), quiet(quiet_in), types(types_in), names(names_in) {
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
    string line;
    getline(cin, line);
    for (int r = 0; r < N; r++) {
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
size_t Tab::sift(/*string col,*/ ColComp comp) {
    size_t N = 0;
    /*size_t w = 0;
    try {
        w = findCol(col);
    } catch (const exception& e) {
        throw e;
    }*/
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
            string temp;
            if (!modes[u]) temp = names[cols[u]];
            else temp = other->names[cols[u]];
            cout << temp << ' ';
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