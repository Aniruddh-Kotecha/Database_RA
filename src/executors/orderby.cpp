#include "global.h"


/**
 * @brief 
 * SYNTAX:  Result <- ORDER BY attribute ASC|DESC ON table
 */
bool syntacticParseORDERBY()
{
    logger.log("syntacticParseORDERBY");
    // cout << tokenizedQuery.size()  << " " << tokenizedQuery[3] << " " << tokenizedQuery[6] << endl;
    if( tokenizedQuery.size() != 8 || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON"){

        cout << "SYNTAX ERROR " << endl;
        return false;
    }
    

    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderByResultantTableName = tokenizedQuery[0];
    // parsedQuery.orderByColumnName = tokenizedQuery[4];
    parsedQuery.sortColumnNames.push_back(tokenizedQuery[4]);
    
    if(tokenizedQuery[5] == "ASC")
        parsedQuery.sortingStrategy.push_back(ASC);
    
    else if(tokenizedQuery[5] == "DESC")
        parsedQuery.sortingStrategy.push_back(DESC);

    
    else{
        cout << "SYNTAX ERROR : order by sorting strategy is not valid!!!" << endl;
        parsedQuery.orderBySortingStrategy = NO_SORT_CLAUSE;
        return false;
    }


    parsedQuery.orderByRelationName = tokenizedQuery[7];

    return true;
}



bool semanticParseORDERBY()
{
    logger.log("semanticParseORDERBY");

    if (tableCatalogue.isTable(parsedQuery.orderByResultantTableName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.orderByRelationName)){
        cout << "SEMANTIC ERROR: order by table does not exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnNames[0] , parsedQuery.orderByRelationName)){
        cout << "SEMANTIC ERROR: order by column does not exists" << endl;
        return false;
    }

    return true;
}

void executeORDERBY()
{
    logger.log("executeORDERBY");
    Table* table = tableCatalogue.getTable(parsedQuery.orderByRelationName);

    Table* resultantTable = new Table(parsedQuery.orderByResultantTableName);

    resultantTable->copyTable(*table);   
    tableCatalogue.insertTable(resultantTable);
    resultantTable->sortRelation();





    return;
}