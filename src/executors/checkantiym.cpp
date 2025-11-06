#include "global.h"
//========================= Matrix Code Starts ==========================

bool syntacticParseCHECKANTISYM()
{
    logger.log("syntacticParseCHECKANTISYM");
    logger.log(to_string(tokenizedQuery.size()));
    
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKANTISYM;
    parsedQuery.antiSymFirstMatrixName = tokenizedQuery[1];
    parsedQuery.antiSymSecondMatrixName = tokenizedQuery[2];

    return true;
}
    
bool semanticParseCHECKANTISYM()
{
    logger.log("semanticParseCHECKANTISYM");
    if (!matrixCatalogue.isMatrix(parsedQuery.antiSymFirstMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix " << parsedQuery.antiSymFirstMatrixName << " doesn't exist" << endl;
        return false;
    }
    if (!matrixCatalogue.isMatrix(parsedQuery.antiSymSecondMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix " << parsedQuery.antiSymSecondMatrixName << " doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKANTISYM()
{
    logger.log("executeCHECKANTISYM");
    Matrix *matrix1 = matrixCatalogue.getMatrix(parsedQuery.antiSymFirstMatrixName);
    Matrix *matrix2 = matrixCatalogue.getMatrix(parsedQuery.antiSymSecondMatrixName);


    int size1 = ceil(matrix1->rowCount * 1.0 / matrix1->maxRowsPerBlock);
    size1 = size1 * size1;

    int size2 = ceil(matrix2->rowCount * 1.0 / matrix2->maxRowsPerBlock);
    size2 = size2 * size2;

    if(size1 != size2) 
    {
        cout << "False" << "\n";
        matrix2->transpose();

        return;
    }
    matrix2->transpose();

    for(int k = 0; k < size1; k++) 
    {
        Page page1 = bufferManager.getMatrixPage(matrix1->matrixName, k);
        Page page2 = bufferManager.getMatrixPage(matrix2->matrixName, k);

        vector<vector<int>> pageData1 = page1.getRows();
        vector<vector<int>> pageData2 = page2.getRows();


        if(pageData1.size() != pageData2.size() || pageData1[0].size() != pageData2[0].size()) 
        {
            cout << "False" << "\n";
            matrix2->transpose();

            return;
        }

        for(int i = 0; i < pageData1.size(); i++) 
        {
            for(int j = 0; j < pageData1[0].size(); j++) 
            {
                logger.log(to_string(pageData1[i][j]) + " " + to_string(pageData2[i][j]));
                if(pageData1[i][j] != (-1 * pageData2[i][j]))
                {   
                    cout << "False" << "\n";
                    matrix2->transpose();
                    return;
                }
            }
        }

    }
    matrix2->transpose();

    cout << "True" << "\n";
    return;
}
