#include "global.h"


/**
 * @brief 
 * SYNTAX:  R <- GROUPBY relation_name1, relation_name2 ON col1, col2
 */
bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");

    // cout<<tokenizedQuery.size()<<endl;

    // cout<< "3 :- " << tokenizedQuery[3] <<"  "<< boolalpha<< (tokenizedQuery[3] != "BY")<<endl;
    // cout<< "5 :- " << tokenizedQuery[5] <<"  "<< boolalpha<< (tokenizedQuery[5] != "FROM")<<endl;
    // cout<< "7 :- " << tokenizedQuery[7] <<"  "<< boolalpha<< (tokenizedQuery[7] != "HAVING")<<endl;
    // cout<< "11 :- " << tokenizedQuery[11] <<"  "<< boolalpha<< (tokenizedQuery[11] != "RETURN")<<endl;

    if( tokenizedQuery.size() != 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN"){

        cout << "SYNTAX ERROR " << endl;
        return false;
    }
    
    // cout<<"DONEEE  "<<endl;

    parsedQuery.queryType = GROUPBY;
    
    parsedQuery.groupByResultTableName = tokenizedQuery[0];
    parsedQuery.groupByColumnName = tokenizedQuery[4];
    parsedQuery.sortColumnNames.push_back(parsedQuery.groupByColumnName);
    parsedQuery.sortingStrategy.push_back(ASC);
    
    parsedQuery.groupByTableName = tokenizedQuery[6];

    // regex pattern(R"((?i)\b(MIN|MAX|COUNT|AVG|SUM)\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\))");
    std::regex pattern(R"(\b(MIN|MAX|COUNT|AVG|SUM)\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\))", std::regex_constants::icase);

    smatch match;   
    string havingOp;

    if (regex_match(tokenizedQuery[8], match, pattern)) {
        havingOp = match[1];  // Extract the aggregate name
        parsedQuery.havingAggregateColumnName = match[2];    // Extract the column name
    }

    if (havingOp == "MIN")
        parsedQuery.havingAggreagateOp = MIN;

    else if(havingOp == "MAX")
        parsedQuery.havingAggreagateOp = MAX;

    else if(havingOp == "COUNT")
        parsedQuery.havingAggreagateOp = COUNT;

    else if(havingOp == "SUM")
        parsedQuery.havingAggreagateOp = SUM;

    else if(havingOp == "AVG")
        parsedQuery.havingAggreagateOp = AVG;
    
    else{
        cout<<"SYNTAX ERROR in HAVING Aggreagte operator" <<endl;
        parsedQuery.havingAggreagateOp = NO_AGGREGATE_OP_CLAUSE;
        return false;
    }

    string binaryOperator = tokenizedQuery[9];

    if (binaryOperator == "<")
        parsedQuery.havingBinaryOp = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.havingBinaryOp = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.havingBinaryOp = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.havingBinaryOp = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.havingBinaryOp = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.havingBinaryOp = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR in having Binary operator" << endl;
        return false;
    }

    parsedQuery.havingAggregateValue = stoi(tokenizedQuery[10]);

    string returnOp;

    if (regex_match(tokenizedQuery[12], match, pattern)) {
        returnOp = match[1];  // Extract the aggregate name
        parsedQuery.returnColumnName = match[2];    // Extract the column name
        // return true;
    }

    if (returnOp == "MIN")
        parsedQuery.returnAggreagateOp = MIN;

    else if(returnOp == "MAX")
        parsedQuery.returnAggreagateOp = MAX;

    else if(returnOp == "COUNT")
        parsedQuery.returnAggreagateOp = COUNT;

    else if(returnOp == "SUM")
        parsedQuery.returnAggreagateOp = SUM;

    else if(returnOp == "AVG")
        parsedQuery.returnAggreagateOp = AVG;
    
    else{
        cout<<"SYNTAX ERROR in RETURN Aggreagte operator" <<endl;
        parsedQuery.returnAggreagateOp = NO_AGGREGATE_OP_CLAUSE;
        return false;
    }




    return true;

}



bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");

    if (tableCatalogue.isTable(parsedQuery.groupByResultTableName ))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.groupByTableName)){
        cout << "SEMANTIC ERROR: Group by table is not exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupByColumnName , parsedQuery.groupByTableName)){
        cout << "SEMANTIC ERROR: Group by column is not exists" << endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.havingAggregateColumnName , parsedQuery.groupByTableName)){
        cout << "SEMANTIC ERROR: Having by aggregate column in not exist" <<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.returnColumnName , parsedQuery.groupByTableName)){
        cout << "SEMANTIC ERROR: Return aggregate column is not exists" <<endl;
        return false;
    }

    return true;
}

void executeGROUPBY()
{

    logger.log("executeGROUPBY");
    Table* table = tableCatalogue.getTable(parsedQuery.groupByTableName);
    table->sortRelation();
    table->groupByRelation();


    return;
}