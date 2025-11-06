#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}



//===================== Matrix Code ==============================================


bool syntacticParseEXPORTMatrix()
{
    logger.log("syntacticParseEXPORTMatrix");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORTMatrix()
{
    logger.log("semanticParseEXPORTMatrix");
    
    //Matrix should exist
    if(matrixCatalogue.isMatrix(parsedQuery.exportMatrixName)) {
        return true;
    }

    cout << "SEMANTIC ERROR: No such Matrix exists" << endl;
    return false;
}

void executeEXPORTMatrix()
{
    logger.log("executeEXPORTMatrix");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportMatrixName);
    matrix->makePermanent();
    return;
}