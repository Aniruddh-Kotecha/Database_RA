#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    logger.log("Here " + to_string(table.columnCount));
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    logger.log("blockCount " + to_string(table.blockCount) + " " + to_string(pageIndex % table.blockCount));
    this->rowCount = table.rowsPerBlockCount[(pageIndex % table.blockCount)];
    logger.log("Here " + to_string(pageIndex)  + "  " + to_string(this->rowCount));

    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    logger.log("Here 2");

    fin.close();
}
//  Constructor for matrix page 

Page::Page(string matrixName, int pageIndex, bool flag)
{

    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);

    ifstream fin(pageName, ios::in);
    int number;

    string line;
    while (getline(fin, line)) {
        stringstream ss(line);        
        vector<int> row;            
        int number;

        while (ss >> number) {        
            row.push_back(number);    
        }
        this->rows.push_back(row);        
    }
    if(rows.size() == 0) {
        this->columnCount = 0;
        this->rowCount = 0;
    }
    else {
        this->rowCount = this->rows.size();
        this->columnCount = this->rows[0].size();
    }

    fin.close();
}



Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    // cout <<"Inside page constructor" << this->rows.size() << "\n";
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows.size() == 0 ? 0 : rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
    // cout <<"Inside page constructor" << this->rows.size() << "\n";

}



Page::Page(string matrixName, int pageIndex, vector<vector<int>> rows, int rowCount, bool flag)
{
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->matrixName + "_Page" + to_string(pageIndex);
}
/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    // cout << "Page writePage " << this->rowCount << "\n";
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0) 
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

int Page::getRowCount() {
    return this->rowCount;
}

vector<vector<int>> Page::getRows() {
    return this->rows;
}

void Page::writeMatrixPage()
{
    logger.log("Page::writeMatrixPage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0) 
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

void Page::transposeBlockInternally() {
    logger.log("transposeBlockInternal for ");

    
    int r = this->rows.size();
    int c = this->rows[0].size();
    vector<vector<int>> transposePageData(c, vector<int>(r));

    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++) {
            transposePageData[j][i] = this->rows[i][j];
        }
    }
    this->rows = transposePageData;
    swap(this->rowCount, this->columnCount);

    bufferManager.writeExistingPage(*this);
    return;

}

void Page::setRows(vector<vector<int>> &rows) {
    this->rowCount = rows.size();
    this->rows = rows;
}

void Page::swapContent(Page &page) {
    swap(this->columnCount, page.columnCount);
    swap(this->rowCount, page.rowCount);
    swap(this->rows, page.rows);
}
void Page::swapColumns(Page &page, int i, int j) {
    logger.log("Page::swapColumns");

    for(int k = 0; k < this->rows.size(); k++) {
        swap(this->rows[k][i], page.rows[k][j]);
    }
}
void Page::swapColumns(int i, int j) {

    logger.log("Page::swapColumns");

    
    for(int k = 0; k < this->rows.size(); k++) {
        swap(this->rows[k][i], this->rows[k][j]);
    }
    
}
void Page::addRow(vector<int> &row) {
    logger.log("Page::addRow");
    // cout <<"rows size add row " << this->rows.size() << " ";
    // this->rows.push_back(row);
    this->rows[this->rowCount] = row;
    this->rowCount++;
}

