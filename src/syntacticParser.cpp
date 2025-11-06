#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if (possibleQueryType == "LOAD") {
        if(tokenizedQuery[1] == "MATRIX") {
            return syntacticParseLOADMatrix();
        }
        return syntacticParseLOAD();
    }
    else if (possibleQueryType == "PRINT") {

        if(tokenizedQuery[1] == "MATRIX") {
            return syntacticParsePRINTMatrix();
        }

        return syntacticParsePRINT();
    }
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME();
    else if(possibleQueryType == "EXPORT") {

        if(tokenizedQuery[1] == "MATRIX") {
            return syntacticParseEXPORTMatrix();
        }
        return syntacticParseEXPORT();
    }
    else if(possibleQueryType == "SOURCE") {
        return syntacticParseSOURCE();
    }
    else if(possibleQueryType == "ROTATE") {
        return syntacticParseROTATE();
    }

    else if(possibleQueryType == "CROSSTRANSPOSE") {
        return syntacticParseCROSSTRANSPOSE();
    }
    
    else if(possibleQueryType == "CHECKANTISYM") {
        return syntacticParseCHECKANTISYM();
    }
    else if (possibleQueryType == "SORT") {
        return syntacticParseSORT();
    }
    else if(possibleQueryType == "INSERT") {
        return syntacticParseINSERT();
    }
    else if(possibleQueryType == "UPDATE") {
        return syntacticParseUPDATE();
    }
    else if(possibleQueryType == "DELETE") {
        return syntacticParseDELETE();
    }
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else if(possibleQueryType == "ORDER") {
            return syntacticParseORDERBY();
        }
        // else if (possibleQueryType == "SORT")
        //     return syntacticParseSORT();
        // else if (possibleQueryType == "SORT")
        //     return syntacticParseSORT();
        else if (possibleQueryType == "GROUP")
            return syntacticParseGROUPBY();
        else if(possibleQueryType == "SEARCH") 
            return syntacticParseSEARCH();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    logger.log("1");
    this->queryType = UNDETERMINED;
    logger.log("2");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";
    logger.log("3");

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";
    logger.log("4");

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;
    logger.log("5");

    this->sortingStrategy = {};
    this->sortResultRelationName = "";
    this->sortColumnNames = {};
    this->sortRelationName = "";
    logger.log("6");

    // this->orderByColumnName = "";
    this->orderByRelationName = "";
    logger.log("8");
    this->orderByResultantTableName = "";
    // this->orderBySortingStrategy = NO_SORT_CLAUSE;

    // this->sourceFileName = "";
    logger.log("7");

    this->havingAggreagateOp = NO_AGGREGATE_OP_CLAUSE;
    this->havingAggregateColumnName = "";
    this->havingAggregateValue = 0;
    this->havingBinaryOp = NO_BINOP_CLAUSE;
    this->groupByResultTableName = "";
    this->groupByColumnName = "";
    this->groupByTableName = "";
    this->returnAggreagateOp = NO_AGGREGATE_OP_CLAUSE;
    this->returnColumnName = "";

    // insert 

    this->insertResultRelationName = "";
    this->insertCol.clear();
    this->insertVal.clear();

    //update
    this->updateRelationName = "";
    this->updateColName = "";
    this->updateColValue = 0;
    this->updateConditionColName = "";
    this->updateConditionValue = 0;
    this->updateBinaryOperator = NO_BINOP_CLAUSE;

    //delete
    this->deleteRelationName = "";  
    this->deleteBinaryOperator = NO_BINOP_CLAUSE;
    this->deleteConditionColName = "";  
    this->deleteConditionValue = 0;

    //search
    this->searchRelationName = "";
    this->searchConditionColName = "";
    this->searchConditionValue = 0;
    this->searchBinaryOperator = NO_BINOP_CLAUSE;
    this->searchResultRelationName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    // cout << "filename " << fileName << "\n";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}
