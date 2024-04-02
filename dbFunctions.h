// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "TableEntry.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>

class ColComp;
class Index;

class Tab {
public: 
    Tab(std::string tablename, std::vector<std::string> &types_in, std::vector<std::string> &names_in, bool quiet); //ctor
    Tab(const Tab& other);
    Tab &operator= (const Tab& rhs);

    void insert(std::size_t N);
    void print(std::vector<std::string> &cols);
    std::size_t print(std::vector<std::string> &cols, bool quiet, std::size_t col, char OP, TableEntry val);
    std::size_t sift(ColComp &comp);
    void join(Tab &other, std::size_t col1, std::size_t col2, 
        std::vector<std::size_t> &cols, std::vector<bool> &modes, bool quiet);
    void indexify(bool order, std::size_t col);

    std::vector<TableEntry>* rowify(std::vector<std::string> &types);
    std::string name;
    bool quiet;
    std::size_t findCol (const std::string colname);
    std::string findType (const std::size_t col) {return types[col];}
    std::size_t width () {return types.size();}
    std::size_t size () {return data.size();}

    friend class ColComp;
    friend class Index;

    Index* i;

    ~Tab();

private:
    std::vector<std::vector<TableEntry>*> data;
    std::vector<std::string> types, names;
};

class ColComp {
public:
    ColComp(std::size_t col_in, char OP_in, TableEntry val_in, Tab &database);
    bool operator() (std::vector<TableEntry>* ptr);

    std::size_t col;
    
    friend class Tab;

private:
    TableEntry val;
    char OP;
    std::vector<std::vector<TableEntry>*>* data;
};

class Index {
public:
    Index(bool order_in, std::size_t col, Tab &target);
    std::vector<std::vector<TableEntry>*> &operator() (TableEntry x);
    std::size_t size() {
        if (order) return o.size();
        else return u.size();
    } void emplace (std::vector<TableEntry>* elt);
    void erase (TableEntry key, std::vector<TableEntry>* elt);
    void reindex(bool order_in, std::size_t col, Tab &target);
    std::size_t count (TableEntry elt) {
        if (order) return o.count(elt);
        else return u.count(elt);
    }

    friend class Tab;

private:
    bool order;
    size_t col;
    std::unordered_map<TableEntry, std::vector<std::vector<TableEntry>*>> u;
    std::map<TableEntry, std::vector<std::vector<TableEntry>*>> o;
};