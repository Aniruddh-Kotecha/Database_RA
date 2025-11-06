#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 *  
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * SORT 〈table-name〉 BY 〈col1〉, 〈col2〉, 〈col3〉 IN 〈ASC|DESC〉,
    〈ASC|DESC〉, 〈ASC|DESC〉
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    // if(tokenizedQuery.size()!= 8 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN"){
    //     cout<<"SYNTAX ERROR"<<endl;
    //     return false;
    // }
    // for(auto it : tokenizedQuery) {
    //     cout << it << "\n";
    // }
    if(tokenizedQuery[2] != "BY") {
        cout << "SYNTAX ERROR HERE" << endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    int i = 3;
    while(tokenizedQuery[i] != "IN" && i < tokenizedQuery.size()) {
        
        parsedQuery.sortColumnNames.push_back(tokenizedQuery[i]);
        i++;
    }

    if(i == tokenizedQuery.size()) {
        cout << "SYNTAX ERROR IN not present" << endl;
     
           return false;
    }
    
    i++;

    while(i < tokenizedQuery.size()) {
        if(tokenizedQuery[i] == "ASC") {
            parsedQuery.sortingStrategy.push_back(ASC);
        }
        else if(tokenizedQuery[i] == "DESC") {
            parsedQuery.sortingStrategy.push_back(DESC);
        }
        else {
            cout << "SYNTAX ERROR INVALID Strategy " << tokenizedQuery[i] << endl;
            return false;
        }
        i++;
    }
    if(parsedQuery.sortColumnNames.size() != parsedQuery.sortingStrategy.size()) {
        cout << "SYNTAX ERROR INVALID SIZE" << endl;
        return false;
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }
    for(int i = 0; i < parsedQuery.sortColumnNames.size(); i++) {
        if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnNames[i], parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    
    table->sortRelation();
    return;
}