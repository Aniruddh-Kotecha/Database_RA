#include "global.h"
/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param matrixName 
 * @param columns 
 */


// Matrix::Matrix(string tableName, vector<string> columns)
// {
//     logger.log("Table::Table");
//     this->sourceFileName = "../data/temp/" + tableName + ".csv";
//     this->matrixName = matrixName;
//     this->columns = columns;
//     this->columnCount = columns.size();
//     this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
//     this->writeRow<string>(columns);
// }

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    logger.log(this->sourceFileName);
    fstream fin(this->sourceFileName, ios::in);
    logger.log("File open successfully");
    string line;
    if (getline(fin, line))
    {
        fin.close();
        this->getSize(line);
        if (this->blockify())
            return true;
    }
    fin.close();
    return false;
}


/**
 * @brief Finds the size of the matrix by counting elements in a CSV line.
 * 
 * This function reads a CSV-formatted string, counts the number of elements,
 * and updates the matrix's row and column counts accordingly.
 * 
 * @param line The CSV line representing a matrix row.
 * @return int The number of elements (columns) in the line.
 */

void Matrix::getSize(string line) {
    logger.log("Matrix::getSize");

    stringstream ss(line);
    string element;
    int count = 0;
    while (getline(ss, element, ',')) {
        count++;
    }
    logger.log(to_string(count));
    this->columnCount = count;
    this->rowCount = count;
    logger.log(to_string(this->rowCount) + " " + to_string(this->columnCount));

    this->maxRowsPerBlock = sqrt((BLOCK_SIZE * 1000) / sizeof(int)); 

    return;

}


/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */

bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;

    int pageCounter = 0;
    int rowCounter = 0;

    int blocksInOneRow = ceil(this->rowCount * 1.0 / this->maxRowsPerBlock); // ubha blocks
    int blocksInOneCol = ceil(this->columnCount * 1.0 / this->maxRowsPerBlock); // aada blocks
 
    for(int i = 0; i < blocksInOneRow; i++) {
        for(int j = 0; j < blocksInOneCol; j++) {
        
            vector<vector<int>> blockData;
            int pos = fin.tellg();
            

            for(int k = 0; k < this->maxRowsPerBlock; k++) {
                if(!getline(fin, line)) {
                    break;
                }
                stringstream ss(line);
                string value;
                
                vector<int> row;

                while (getline(ss, value, ',')) {
                    row.push_back(stoi(value));
                }

                int startCol = j * this->maxRowsPerBlock;
                int endCol = min(int((j + 1) * this->maxRowsPerBlock), (int)row.size());

    
                vector<int> data(row.begin() + startCol, row.begin() + endCol);

                blockData.push_back(data);
            }

            if(j != blocksInOneCol - 1) {
                fin.clear();

                fin.seekg(pos);
            }

            bufferManager.writeMatrixPage(this->matrixName, pageCounter, blockData, blockData.size());
 
            pageCounter++;
        }
    }
    return true;
}

void swapBlocks(string matrixName, int first, int second) 
{
    logger.log("Matrix::swapBlocks");

    Page page1 = bufferManager.getMatrixPage(matrixName, first);

    Page page2 = bufferManager.getMatrixPage(matrixName, second);

    page1.swapContent(page2);

    bufferManager.writeExistingPage(page1);
    bufferManager.writeExistingPage(page2);
} 

void Matrix::transpose()
{
    logger.log("Matrix::transpose");

    int totalBlocksInRow = ceil(this->columnCount * 1.0 / this->maxRowsPerBlock);

    for(int i = 0; i < (totalBlocksInRow * totalBlocksInRow); i++) {
        Page page = bufferManager.getMatrixPage(this->matrixName, i);
        page.transposeBlockInternally();
    }

    for(int i = 0; i < (totalBlocksInRow * totalBlocksInRow); i++) {
        int r = i / totalBlocksInRow;
        int c = i % totalBlocksInRow;

        if(r >= c) continue;

        int firstBlock = i;
        int secondBlock = c * totalBlocksInRow + r;
        logger.log(to_string(i) + " " + to_string(secondBlock));
        swapBlocks(this->matrixName, firstBlock, secondBlock);

    }
}
void Matrix::reverse() 
{
    logger.log("Matrix::reverse");
    int totalBlocksInCol = ceil(this->columnCount * 1.0 / this->maxRowsPerBlock);
    int total_col = this->columnCount;

    Page *page1 = nullptr;
    Page *page2 = nullptr;
    Page *commonPage = nullptr;

    Page temp1;
    Page temp2;

    logger.log("Total Blocks in Column: " + to_string(totalBlocksInCol));

    for (int i = 0; i < totalBlocksInCol; i++) {
        int j = 0;
        int page1Num = (i * totalBlocksInCol) + (j / maxRowsPerBlock);
        int page2Num = (i * totalBlocksInCol) + ((total_col - j - 1) / maxRowsPerBlock);

        if (page1Num != page2Num) {
            temp1 = bufferManager.getMatrixPage(matrixName, page1Num);
            page1 = &temp1;
            temp2 = bufferManager.getMatrixPage(matrixName, page2Num);
            page2 = &temp2;

                    
        } 
        else {
            temp1 = bufferManager.getMatrixPage(this->matrixName, page1Num);
            commonPage = &temp1;

        }

        while (j < total_col / 2) {
            if (page1Num != (i * totalBlocksInCol) + (j / maxRowsPerBlock)) {
                if (page1) bufferManager.writeExistingPage(*page1);
                page1Num = (i * totalBlocksInCol) + (j / maxRowsPerBlock);

                if (page1Num == page2Num) {
                    commonPage = page2;
                } 
                else 
                {
                    temp1 = bufferManager.getMatrixPage(matrixName, page1Num);
                    page1 = &temp1;
                }
            }

            if (page2Num != (i * totalBlocksInCol) + ((total_col - j - 1) / maxRowsPerBlock)) {
                if (page2) bufferManager.writeExistingPage(*page2);
                page2Num = (i * totalBlocksInCol) + ((total_col - j - 1) / maxRowsPerBlock);

                if (page1Num == page2Num) {
                    commonPage = page1;
                } 
                else {
                    temp2 = bufferManager.getMatrixPage(matrixName, page2Num);
                    page2 = &temp2;
                }
            }

            if (page1Num == page2Num && commonPage) {
                while (j < total_col / 2) {
                    int column1 = j % maxRowsPerBlock;
                    int column2 = (total_col - j - 1) % maxRowsPerBlock;
                    commonPage->swapColumns(*commonPage, column1, column2);
                    j++;
                }
                break;
            } 
            else if (page1 && page2) {
                int column1 = j % maxRowsPerBlock;
                int column2 = (total_col - j - 1) % maxRowsPerBlock;

                page1->swapColumns(*page2, column1, column2);
            }
            j++;
        }

        if (page1Num == page2Num && commonPage) {
            bufferManager.writeExistingPage(*commonPage);
        } 
        else {
            if (page1) bufferManager.writeExistingPage(*page1);
            if (page2) bufferManager.writeExistingPage(*page2);
        }
    }
}



void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNextPage");
        int temp = ceil(this->rowCount * 1.0 / this->maxRowsPerBlock);
        
        
        if (cursor->pageIndex < temp * temp)
        {
            cursor->nextMatrixPage(cursor->pageIndex+1);
        }
}

bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
    return true;
    return false;
}




void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");

    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    int blockCount= ceil(this->rowCount * 1.0 / this->maxRowsPerBlock);
    long long totalBlocks = blockCount * blockCount;

    for(int i=0; i<totalBlocks; i = i + blockCount){

        Page page = bufferManager.getMatrixPage(this->matrixName, i);

        for(int rowIndex= 0; rowIndex < page.getRowCount(); rowIndex++){
            vector<int> fullBlockRow;
            
            for(int blockIndex = 0; blockIndex<blockCount; blockIndex++){

                Page block = bufferManager.getMatrixPage(this->matrixName , i + blockIndex);
                vector<int>row = block.getRow(rowIndex);
                fullBlockRow.insert(fullBlockRow.end(), row.begin(), row.end());
            }
            this->writeRow(fullBlockRow);   
        }
        
    }
    
    fout.close();
}


void Matrix::print()
{
    logger.log("Matrix::PRINT");

    int blockCount= ceil(this->rowCount * 1.0 / this->maxRowsPerBlock);
    int currentRow = 0;
    long long totalBlocks = blockCount * blockCount;

    for(int i=0; i<totalBlocks; i = i + blockCount) {
        Page page = bufferManager.getMatrixPage(this->matrixName, i);

        for(int rowIndex= 0; rowIndex < page.getRowCount(); rowIndex++){
            vector<int> fullBlockRow;
            
            for(int blockIndex = 0; blockIndex<blockCount; blockIndex++){

                Page block = bufferManager.getMatrixPage(this->matrixName , i + blockIndex);
                vector<int>row = block.getRow(rowIndex);
                fullBlockRow.insert(fullBlockRow.end(), row.begin(), row.end());
                if(fullBlockRow.size() >= min(this->rowCount, int(PRINT_COUNT))){
                    break;
                }
            }
            this->printRow(vector<int>(fullBlockRow.begin(), fullBlockRow.begin() + min(this->rowCount, int(PRINT_COUNT))));   
            currentRow++;
            if(currentRow >= min(rowCount, int(PRINT_COUNT))) {
                return;
            }
        }
        
    }
    
}
