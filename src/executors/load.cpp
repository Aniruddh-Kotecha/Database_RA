#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOAD;
    parsedQuery.loadRelationName = tokenizedQuery[1];
    return true;
}


bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");
    
    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}


//===================== Matrix Code ==============================================

/**
 * @brief 
 * SYNTAX: LOAD matrix_name
 */

bool syntacticParseLOADMatrix() {
    logger.log("syntacticParseLOADMatrix");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOAD;
    parsedQuery.loadMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseLOADMatrix()
{
    logger.log("semanticParseLOADMatrix");
    if(matrixCatalogue.isMatrix(parsedQuery.loadMatrixName)) {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;

    }

    if (!isFileExists(parsedQuery.loadMatrixName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOADMatrix()
{
    logger.log("executeLOADMatrix");
    Matrix *matrix = new Matrix(parsedQuery.loadMatrixName);
    if (matrix->load())
    {
        matrixCatalogue.insertMatrix(matrix);
        cout << "Loaded Matrix. Column Count: " << matrix->columnCount << " Row Count: " << matrix->rowCount << endl;
    }
    else {
        cout << "Invalid CSV content" << "\n";
    }
    return;
}
