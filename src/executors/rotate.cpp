#include "global.h"
//========================= Matrix Code Starts ==========================

bool syntacticParseROTATE()
{
    logger.log("syntacticParseROTATE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ROTATE;
    parsedQuery.rotateMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseROTATE()
{
    logger.log("semanticParseROTATE");
    if (!matrixCatalogue.isMatrix(parsedQuery.rotateMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeROTATE()
{
    logger.log("executeROTATE");
    //    Matrix *matrix = new Matrix(parsedQuery.loadMatrixName);

    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.rotateMatrixName);
    matrix->transpose();
    matrix->reverse();
    
    return;
}
