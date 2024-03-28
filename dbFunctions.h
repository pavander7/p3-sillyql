// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "TableEntry.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

class ColComp;
class Index;

class Tab {
public: 
    Tab(std::string tablename, std::vector<std::string> types_in, std::vector<std::string> names_in, bool quiet); //ctor
    Tab(const Tab& other);
    Tab &operator= (const Tab& rhs);

    void insert(int N);
    std::size_t print(std::vector<std::string> cols, bool quiet);
    std::size_t print(std::vector<std::string> cols, bool quiet, ColComp comp);
    std::size_t sift(std::string col, ColComp comp);
    void join(Tab* other, std::size_t col1, std::size_t col2, 
            std::vector<std::size_t> cols, std::vector<bool> modes, bool quiet);

    class Row;
    std::string name;
    bool quiet;
    std::size_t findCol (const std::string colname);
    std::string findType (const std::size_t col) {return types[col];}

    friend class ColComp;
    friend class Index;

private:
    std::vector<Row> data;
    std::vector<std::string> types, names;
};

class Tab::Row {
public:
    Row(std::vector<std::string> &types, std::string line);
    TableEntry &operator[] (const std::size_t y) {
        return rowData[y];
    }
    TableEntry at(std::size_t y) {
        return rowData[y];
    }
private:
    std::vector<TableEntry> rowData;
};

class ColComp {
public:
    ColComp(std::string colname_in, char OP_in, TableEntry val_in, Tab* database);
    ColComp(std::size_t col_in, char OP_in, TableEntry val_in, Tab* database);
    bool operator() (std::size_t i);
    bool operator() (Tab::Row *ptr);
private:
    std::size_t col;
    TableEntry val;
    char OP;
    std::vector<Tab::Row>* data;
};

class Index {
public:
    //Index(std::vector<std::string> names, std::vector<Tab::Row> &rawData);
    Index(bool order_in, std::size_t col, std::vector<Tab::Row> &rawData);
    Index(bool order_in, std::size_t col, Tab* target);
    Tab::Row* &operator() (TableEntry x);
    std::size_t size() {
        if (order) return o.size();
        else return u.size();
    }
    //void reindex(bool order_in, std::string col, std::vector<std::string> names, std::vector<Tab::Row> &rawData);
    void reindex(bool order_in, std::size_t col, Tab* target);

private:
    std::unordered_map<TableEntry, Tab::Row*> u;
    std::map<TableEntry, Tab::Row*> o;
    bool order;
};