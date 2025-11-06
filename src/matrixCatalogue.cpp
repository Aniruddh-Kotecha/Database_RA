#include "global.h"

void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::~insertTable"); 
    this->matrixes[matrix->matrixName] = matrix;
}
void MatrixCatalogue::deleteMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::deleteMatrix"); 
    // this->matrixes[matrixName]->unload();
    delete this->matrixes[matrixName];
    this->matrixes.erase(matrixName);
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getMatrix"); 
    Matrix *matrix = this->matrixes[matrixName];
    return matrix;
}
bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isMatrix"); 
    if (this->matrixes.count(matrixName))
        return true;
    return false;
}

// bool MatrixCatalogue::isColumnFromTable(string columnName, string tableName)
// {
//     logger.log("TableCatalogue::isColumnFromTable"); 
//     if (this->isTable(tableName))
//     {
//         Table* table = this->getTable(tableName);
//         if (table->isColumn(columnName))
//             return true;
//     }
//     return false;
// }

// void MatrixCatalogue::print()
// {
//     logger.log("MatrixCatalogue::print"); 
//     cout << "\nMATRIXES" << endl;

//     int rowCount = 0;
//     for (auto rel : this->matrixes)
//     {
//         cout << rel.first << endl;
//         rowCount++;
//     }
//     printRowCount(rowCount);
// }

MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~MatrixCatalogue"); 
    for(auto matrix: this->matrixes){
        // matrix.second->unload();
        delete matrix.second;
    }
}
