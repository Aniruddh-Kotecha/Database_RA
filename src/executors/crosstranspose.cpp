#include "global.h"
//========================= Matrix Code Starts ==========================

bool syntacticParseCROSSTRANSPOSE()
{
    logger.log("syntacticParseCROSSTRANSPOSE");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CROSSTRANSPOSE;
    parsedQuery.crossTransposeFirstMatrixName = tokenizedQuery[1];
    parsedQuery.crossTransposeSecondMatrixName = tokenizedQuery[2];

    return true;
}

bool semanticParseCROSSTRANSPOSE()
{
    logger.log("semanticParseCROSSTRANSPOSE");
    if (!matrixCatalogue.isMatrix(parsedQuery.crossTransposeFirstMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix " << parsedQuery.crossTransposeFirstMatrixName << " doesn't exist" << endl;
        return false;
    }
    if (!matrixCatalogue.isMatrix(parsedQuery.crossTransposeSecondMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix " << parsedQuery.crossTransposeSecondMatrixName << " doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCROSSTRANSPOSE()
{
    logger.log("executeCROSSTRANSPOSE");
    Matrix *matrix1 = matrixCatalogue.getMatrix(parsedQuery.crossTransposeFirstMatrixName);
    Matrix *matrix2 = matrixCatalogue.getMatrix(parsedQuery.crossTransposeSecondMatrixName);

    matrix1->transpose();

    matrix2->transpose();

    logger.log(to_string(matrix1->maxRowsPerBlock));


    int size1 = ceil(matrix1->rowCount * 1.0 / matrix1->maxRowsPerBlock);
    size1 = size1 * size1;

    int size2 = ceil(matrix2->rowCount * 1.0 / matrix2->maxRowsPerBlock);
    size2 = size2 * size2;
    
    int i = 0;
    while(i < min(size1, size2)) 
    {
        Page matrixPage1 = bufferManager.getMatrixPage(matrix1->matrixName, i);
        Page matrixPage2 = bufferManager.getMatrixPage(matrix2->matrixName, i);

        matrixPage1.swapContent(matrixPage2);
        bufferManager.writeExistingPage(matrixPage1);
        bufferManager.writeExistingPage(matrixPage2);
        i++;

    }
    while(i < size1) 
    {
        Page matrixPage1 = bufferManager.getMatrixPage(matrix1->matrixName, i);
        Page matrixPage2 = bufferManager.getMatrixPage(matrix2->matrixName, i);


        matrixPage1.swapContent(matrixPage2);
        bufferManager.writeExistingPage(matrixPage2);
        bufferManager.deleteFile(matrix1->matrixName, i);
        i++;
    }


    while(i < size2) 
    {
        Page matrixPage1 = bufferManager.getMatrixPage(matrix1->matrixName, i);

        Page matrixPage2 = bufferManager.getMatrixPage(matrix2->matrixName, i);


        matrixPage1.swapContent(matrixPage2);
        
        bufferManager.writeExistingPage(matrixPage1);
        bufferManager.deleteFile(matrix2->matrixName, i);
        
        i++;

    }

    swap(matrix1->rowCount, matrix2->rowCount);
    swap(matrix1->columnCount, matrix2->columnCount);
    swap(matrix1->maxRowsPerBlock, matrix2->maxRowsPerBlock);

    

    return;
}
