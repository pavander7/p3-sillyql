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
    Tab(std::string tablename, std::vector<std::string> types_in, std::vector<std::string> names_in); //ctor
    Tab(const Tab& other);
    Tab &operator= (const Tab& rhs);

    void insert(int N);
    void print(std::vector<std::string> cols);
    void print(std::vector<std::string> cols, ColComp comp);
    std::size_t sift(std::string col, ColComp comp);

    class Row;
    std::string name;
    std::size_t findCol (std::string colname);

    friend class ColComp;

    void makeIndex(bool order, std::string col); // no idea how this is supposed to work

private:
    std::vector<Row> data;
    std::vector<std::string> types, names;
    Index *i;
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
    ColComp(std::string colname_in, char OP_in, int val_in, Tab* database);
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
    Index(std::vector<std::string> names, std::vector<Tab::Row> &rawData);
    Tab::Row* &operator() (TableEntry x);
    void reindex(bool order_in, std::string col, std::vector<std::string> names, std::vector<Tab::Row> &rawData);

private:
    std::unordered_map<TableEntry, Tab::Row*> u;
    std::map<TableEntry, Tab::Row*> o;
    bool order;
};