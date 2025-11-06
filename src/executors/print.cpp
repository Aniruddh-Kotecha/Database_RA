#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT()
{
    logger.log("executePRINT");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    table->print();
    return;
}

//========================= Matrix Code Starts ==========================

bool syntacticParsePRINTMatrix()
{
    logger.log("syntacticParsePRINTMatrix");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;
    parsedQuery.printMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINTMatrix()
{
    logger.log("semanticParsePRINTMatrix");
    if (!matrixCatalogue.isMatrix(parsedQuery.printMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINTMatrix()
{
    logger.log("executePRINTMatrix");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printMatrixName);
    matrix->print();
    return;
}
