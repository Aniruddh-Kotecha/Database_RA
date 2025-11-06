#include "global.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "cursor.h"
#include "BPlusTree.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};
enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};
/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */

class Table
{
    std::vector<std::unordered_set<int>> distinctValuesInColumns;

public:
    std::string sourceFileName = "";
    std::string tableName = "";
    std::vector<std::string> columns;
    std::vector<uint> distinctValuesPerColumnCount;
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    std::vector<uint> rowsPerBlockCount;
    bool indexed = false;
    std::string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;

    
    bool extractColumnNames(std::string firstLine);
    bool blockify();
    void updateStatistics(std::vector<int> row);
    Table();
    Table(std::string tableName);
    Table(std::string tableName, std::vector<std::string> columns);
    bool load();
    bool isColumn(std::string columnName);
    void renameColumn(std::string fromColumnName, std::string toColumnName);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(std::string columnName);
    void unload();
    void sortRelation();
    
    void copyTable(Table &table);

    void joinRelation();
    void groupByRelation();

    unordered_map<int, vector<int>> searchInTable(string tableName, string columnName, int value, BinaryOperator binaryOperator);
    pair<int, int> insertRow(vector<string> columns, vector<int> values);

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Table::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << endl;
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Table::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}
};