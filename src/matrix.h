#pragma once
#include "cursor.h"


/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a matrix object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Matrix
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string matrixName = "";
    int columnCount = 0;
    int rowCount = 0;
    int blockCount = 0;
    int maxRowsPerBlock = 0;
    vector<int> rowsPerBlockCount;
    
    bool blockify();
    // void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string matrixName);
    bool load();
    // void print();
    void getSize(string line);
    void transpose();
    void reverse();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    // Cursor getCursor();
    void print();

    // void unload();

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
    logger.log("Matrix::printRow");
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
template <typename T>

void printRow(vector<T> row) {
    logger.log("Matrix::printRow");
    this->writeRow(row, cout);
}
};
