#pragma once

#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    public:
    Page page;
    int pageIndex;
    string tableName;
    string matrixName;
    int rowIndex;
    int colIndex;
    int pagePointer;

    public:
    Cursor(string tableName, int pageIndex);
    Cursor(string matrixName, int pageIndex, bool flag);

    vector<int> getNext();
    vector<int> getNextRowFromCurrPage();
    void nextMatrixPage(int pageIndex);
    vector<int> getNextMatrix();
    void nextPage(int pageIndex);
};