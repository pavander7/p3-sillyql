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
TableEntry PRODUCE (string type) { //creates TableEntries from cin
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
//constructors
Tab::Tab(std::string tablename, std::vector<std::string> &types_in, std::vector<std::string> &names_in, 
        bool quiet_in) : name(tablename), quiet(quiet_in), types(types_in), names(names_in) { //ctor
    i = nullptr;
    std::cout << "New table " << name << " with column(s) ";
    for (auto t : names) {
        std::cout << t << " ";
    } std::cout << "created\n";
}
Tab::Tab(const Tab& other) { //copy-ctor
    name = other.name;
    types = other.types;
    names = other.names;
    quiet = other.quiet;
    i = nullptr;
    data.reserve(other.data.size());
    for (auto her : other.data) {
        vector<TableEntry>* temp = new vector<TableEntry>(*her);
        data.push_back(temp);
    }
}
Tab &Tab::operator=(const Tab& rhs) { //copy-assign
    name = rhs.name;
    types = rhs.types;
    names = rhs.names;
    quiet = rhs.quiet;
    i = nullptr;
    data.reserve(rhs.data.size());
    for (auto her : rhs.data) {
        vector<TableEntry>* temp = new vector<TableEntry>(*her);
        data.push_back(temp);
    }
    return *this;
}
Tab::~Tab() { //dtor
    for (vector<TableEntry>* her : data) {
        delete her;
    } if (i != nullptr) delete i;
}

//data managers
void Tab::insert(size_t N) { //INSERT
    size_t startN = data.size();
    string line;
    getline(cin, line);
    data.reserve(startN + N);
    for (size_t r = 0; r < N; r++) {
        vector<TableEntry>* temp = rowify(types);
        data.push_back(temp);
        if (i != nullptr) i->emplace(temp);
    } size_t endN = data.size() - size_t(1);
    std::cout << "Added " << N << " rows to " << name << " from position " << startN << " to " << endN << endl;
}
size_t Tab::sift(ColComp &comp) { //DELETE
    size_t N = 0;
    vector<vector<TableEntry>*> temp;
    /*Index sieve(false, comp.col, this);
    for (auto & bucket : sieve.o) {
        auto directory = bucket.second;
        if (comp(directory[0])) {
            for (auto & elt : directory) {
                if (i != nullptr) i->erase((elt)->at(i->col), elt);
                delete elt;
                N++;
            }
        }
    }*/
    for (auto & elt : data) {
        if (comp(elt)) {
            if (i != nullptr) i->erase((elt)->at(i->col), elt);
            delete elt;
            N++;
        } else {
            temp.push_back(elt);
        }
    }
    data.swap(temp);
    return N;
}

//printers
void Tab::print(vector<std::string> &cols) { //PRINT ... ALL assumes !quiet
    //map<string,vector<TableEntry>> her(data.begin(), data.end());
    vector<size_t> iCols;
    iCols.reserve(cols.size());
    for (auto mother : cols) {
        std::cout << mother << " ";
        for(size_t q = 0; q < names.size(); q++) {
            if (names[q] == mother) {
                iCols.push_back(q);
                break;
            }
        } 
    } std::cout << endl;
    for(std::vector<TableEntry>* rupaul : data) {
        for (size_t him : iCols) {
            if (!quiet) std::cout << rupaul->at(him) << " ";
        } if (!quiet) std::cout << endl;
    }
}
size_t Tab::print(vector<std::string> &cols, bool quiet, size_t col, char OP, TableEntry val) { //PRINT ... WHERE
    //map<string,vector<TableEntry>> her(data.begin(), data.end());
    vector<size_t> iCols;
    iCols.reserve(cols.size());
    size_t M = 0;
    for(auto mother : cols) {
        if (!quiet) std::cout << mother << " ";
        for(size_t q = 0; q < names.size(); q++) {
            if (names[q] == mother) {
                iCols.push_back(q);
                break;
            }
        }
    } if(!quiet) std::cout << endl;
    if (data.size() == 0) return 0;
    else if (this->i != nullptr && this->i->order && (i->col == col)) {
        if ((OP == '<') && (i->o.lower_bound(val) == i->o.end())) return 0;
        else if ((OP == '>') && (i->o.upper_bound(val) == i->o.end())) return 0;
        else if (OP == '=') {
            if (i->count(val) != 0) {
                vector<vector<TableEntry>*> storage = (*i)(val);
                if (!quiet) {
                    for (auto &bucket : i->o) {
                        auto &storage = bucket.second;
                        for (auto &row : storage) {
                            M += storage.size();
                            if (!quiet) {
                                for (size_t him : iCols) {
                                    std::cout << row->at(him) << " ";
                                } std::cout << endl;
                            }
                        }
                    }
                }
            } return M;
        }
        ColComp comp(col, OP, val, *this);
        if (OP == '<') {
            auto lb = i->o.lower_bound(val);
            for(auto it = i->o.begin(); it != lb; it++) {
                auto &storage = it->second;
                M += storage.size();
                if (!quiet) {
                    for (auto row : storage) {
                        for (size_t him : iCols) {
                            if (!quiet) std::cout << row->at(him) << " ";
                        } if (!quiet) std::cout << endl;
                    }
                }
            }
        }
        else {
            for(auto it = i->o.upper_bound(val); it != i->o.end(); it++) { //copy constructing here (bad)
                auto &storage = it->second;
                M += storage.size();
                if (!quiet) {
                    for (auto row : storage) {
                        for (size_t him : iCols) {
                            if (!quiet) std::cout << row->at(him) << " ";
                        } if (!quiet) std::cout << endl;
                    }
                }
            }
        }
    } else {
        if (OP == '=') {
            Index sieve(false, col, *this);
            if (sieve.count(val) != 0) {
                vector<vector<TableEntry>*> storage = sieve(val);
                M = storage.size();
                if (!quiet) {
                    for (auto &row : storage) {
                        for (size_t him : iCols) {
                            if (!quiet) std::cout << row->at(him) << " ";
                        } if (!quiet) std::cout << endl;
                    }
                }
            } return M;
        }
        ColComp comp(col, OP, val, *this);
        for(vector<TableEntry>* rupaul : data) {
            if (comp(rupaul)) {
                for (size_t him : iCols) {
                    if (!quiet) std::cout << rupaul->at(him) << " ";
                } if (!quiet) std::cout << endl;
                M++;
            }
        }
    } return M;
}
void Tab::join (Tab &other, std::size_t col1, std::size_t col2, 
    std::vector<std::size_t> &cols, std::vector<bool> &modes, bool quiet) { //JOIN
    size_t M = 0;
    Index twoSieve(false, col2, other);
    if (!quiet) {
        for (size_t f = 0; f < cols.size(); f++) {
            string temp;
            size_t u = cols[f];
            if (!modes[f]) temp = names[u];
            else temp = other.names[u];
            std::cout << temp << ' ';
        } std::cout << endl;
    } if (i != nullptr && i->order && (i->col == col1)) {
        for (auto &pear : i->o) {
            auto &mother = pear.second;
            for (auto her : mother) {
                TableEntry val = her->at(col1);
                if (twoSieve.count(val) != 0) {
                    vector<vector<TableEntry>*> storage = twoSieve(val);
                    M += storage.size();
                    if (!quiet) {
                        for (auto row : storage) {
                            for (size_t f = 0; f < cols.size(); f++) {
                                size_t u = cols[f];
                                if (!modes[f]) std::cout << her->at(u) << ' ';
                                else std::cout << row->at(u) << ' ';
                            } std::cout << endl;
                        }
                    }
                }
            }
        }
    } else {
        for (vector<TableEntry>* her : data) {
            TableEntry val = her->at(col1);
            if (twoSieve.count(val) != 0) {
                vector<vector<TableEntry>*> &storage = twoSieve(val);
                M += storage.size();
                if (!quiet) {
                    for (auto &row : storage) {
                        for (size_t f = 0; f < cols.size(); f++) {
                            size_t u = cols[f];
                            if (!modes[f]) std::cout << her->at(u) << ' ';
                            else std::cout << row->at(u) << ' ';
                        } std::cout << endl;
                    }
                }
            }
        }
    }
    std::cout << "Printed " << M << " rows from joining " << this->name << " to " << other.name << endl;
}

//aux functions
size_t Tab::findCol (string colname) { //returns index given column name
    size_t col = 0;
    for(size_t q = 0; q < names.size(); q++) {
        if (names[q] == colname) {
            col = q;
            return col;
        }
    }
    return width();
}
void Tab::indexify(bool order, std::size_t col) { //GENERATE
    if (this->i == nullptr) {
        this->i = new Index(order, col, *this);
    } else {
        this->i->reindex(order, col, *this);
    } 
}
vector<TableEntry>* Tab::rowify(std::vector<std::string> &types) {
    vector<TableEntry>* row = new vector<TableEntry>;
    row->reserve(types.size());
    for (size_t f = 0; f < types.size(); f++) {
        row->push_back(PRODUCE(types[f]));
    } return row;
}
//TAB FUNCTIONS

//COMP FUNCTIONS
ColComp::ColComp(size_t col_in, char OP_in, TableEntry val_in, Tab &database) //ctor 
    : col(col_in), val(val_in), OP(OP_in) {
    this->data = &(database.data);
}
bool ColComp::operator() (vector<TableEntry>* ptr) { // access overload
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
std::vector<vector<TableEntry>*> &Index::operator() (TableEntry x) { // access overload
    if (order) return (o[x]);
    else return (u[x]);
}
Index::Index(bool order_in, size_t col_in, Tab &target) : order(order_in), col(col_in) { //ctor
    vector<vector<TableEntry>*> &rawData = target.data;
    for (vector<TableEntry>* her : rawData) emplace(her);
}
void Index::reindex(bool order_in, size_t col_in, Tab &target) { //re-ctor
    vector<vector<TableEntry>*> &rawData = target.data;
    order = order_in;
    col = col_in;
    o.clear();
    u.clear();
    for (vector<TableEntry>* her : rawData) emplace(her);
}
void Index::emplace (vector<TableEntry>* elt) { //emplace() interface
    TableEntry key = elt->at(col);
    if (order) {
        if (o.count(key) == 0) {
            std::vector<vector<TableEntry>*> temp;
            temp.push_back(elt);
            o.emplace(key, temp);
        } else {
            o.at(key).push_back(elt);
        }
    } else {
        if (u.count(key) == 0) {
            std::vector<vector<TableEntry>*> temp;
            temp.push_back(elt);
            u.emplace(key, temp);
        } else {
            u.at(key).push_back(elt);
        }
    }
}
void Index::erase (TableEntry key, vector<TableEntry>* elt) { //erase() interface
    if (order) {
        if (o.size() == 1) o.erase(key);
        else {
            vector<vector<TableEntry>*> temp;
            for (auto him : o[key]) {
                if (him != elt) temp.push_back(him);
            } swap(o[key], temp);
        }
    } 
    else {
        if (u.size() == 1) u.erase(key);
        else {
            vector<vector<TableEntry>*> temp;
            for (auto him : u[key]) {
                if (him != elt) temp.push_back(him);
            } swap(u[key], temp);
        }
    }
}
//INDEX FUNCTIONS