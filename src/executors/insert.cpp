#include "global.h"
/**
 * @brief 
 * SYNTAX: INSERT INTO table_name (col1 = val1, col2 = val2, col3 = val3 …)
 * 
 */
bool syntacticParseINSERT()
{
    logger.log("syntacticParseINSERT");
    // for(int i =  0; i < tokenizedQuery.size(); i++) {
    //     cout << tokenizedQuery[i] << "\n";
    // }
    // cout << tokenizedQuery.size() << endl; 
    if (tokenizedQuery.size() < 7  || tokenizedQuery[1] != "INTO")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INSERT;
    parsedQuery.insertResultRelationName = tokenizedQuery[2];

    int i = 4;
    
    // PARSE col1 = val1, col2 = val2 ... 

    while(i < tokenizedQuery.size() - 1) {
        parsedQuery.insertCol.push_back(tokenizedQuery[i]);


        if(i + 1 >= tokenizedQuery.size() || tokenizedQuery[i + 1] != "=") {
            cout << "SYNTAX ERROR" << endl;
            return false;           
        }
        
        if(i + 2 >= tokenizedQuery.size()) {
            cout << "SYNTAX ERROR" << endl;
            return false;                    
        }
        
        parsedQuery.insertVal.push_back(stoi(tokenizedQuery[i + 2]));
        
        i+=3;
    }
    return true;
}

bool semanticParseINSERT()
{
    logger.log("semanticParseINSERT");

    if (!tableCatalogue.isTable(parsedQuery.insertResultRelationName))
    {
        cout << "SEMANTIC ERROR: Insert relation doesn't exists" << endl;
        return false;
    }
    // cout << "semanticparse" << "\n";

    for(int i = 0; i < parsedQuery.insertCol.size(); i++) {
        // cout << parsedQuery.insertCol[i] << "\n";
        if(!tableCatalogue.isColumnFromTable(parsedQuery.insertCol[i], parsedQuery.insertResultRelationName)) {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
        }
    }
    // cout << "here";
    
    return true;
}

void executeINSERT()
{
    logger.log("executeINSERT");
    // cout << "table name " << parsedQuery.insertResultRelationName << "\n";
    Table *table = tableCatalogue.getTable(parsedQuery.insertResultRelationName);
    // cout << "fetched" << "\n";
    // logger.log("fetched ");
    pair<int, int> temp = table->insertRow(parsedQuery.insertCol, parsedQuery.insertVal);
    int pageNum = temp.first;
    int rowNum = temp.second;

    for(int i = 0; i < parsedQuery.insertCol.size(); i++) {
        string indexFileName = "../data/temp/" +  table->tableName + "_index_" + parsedQuery.insertCol[i] + ".bpt";

        BPlusTree* index = new BPlusTree();
        index = index->loadFromFile(indexFileName);
        if(index == NULL) {
            continue;
        }
        // cout << "\n";
        // cout << "pagen " << pageNum << " : " << rowNum << "\n";
        index->insert(parsedQuery.insertVal[i], pageNum, rowNum);
        index->saveToFile(indexFileName);
    }
    return;
}