#include "global.h"
#include <filesystem>

#pragma once
/**
 * @brief 
 * SYNTAX: UPDATE table_name WHERE condition SET col_name = value
 * 
 */
bool syntacticParseUPDATE()
{
    logger.log("syntacticParseUPDATE");

    
    if (tokenizedQuery.size() != 10  || tokenizedQuery[2] != "WHERE")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = UPDATE;
    parsedQuery.updateRelationName = tokenizedQuery[1];
    parsedQuery.updateConditionColName = tokenizedQuery[3];
    parsedQuery.updateConditionValue = stoi(tokenizedQuery[5]); 

    // cout<<tokenizedQuery[4]<< "->  " <<boolalpha<<(tokenizedQuery[4] == "=")<<endl;
    if (tokenizedQuery[4] == "<")
        parsedQuery.updateBinaryOperator = LESS_THAN;
    else if (tokenizedQuery[4] == ">")
        parsedQuery.updateBinaryOperator = GREATER_THAN;
    else if (tokenizedQuery[4] == "<=")
        parsedQuery.updateBinaryOperator = LEQ;
    else if (tokenizedQuery[4] == ">=")
        parsedQuery.updateBinaryOperator = GEQ;
    else if (tokenizedQuery[4] == "=")
        parsedQuery.updateBinaryOperator = EQUAL;
    else if (tokenizedQuery[4] == "!=")
        parsedQuery.updateBinaryOperator = NOT_EQUAL;
    else{
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    // cout<<"koko :- "<<parsedQuery.updateBinaryOperator <<endl;
    parsedQuery.updateColName = tokenizedQuery[7];
    parsedQuery.updateColValue = stoi(tokenizedQuery[9]);

    // cout<<parsedQuery.updateColName <<endl;
    return true;
}

bool semanticParseUPDATE()
{
    logger.log("semanticParseUPDATE");

    if (!tableCatalogue.isTable(parsedQuery.updateRelationName))
    {
        cout << "SEMANTIC ERROR: Update relation doesn't exists" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.updateColName, parsedQuery.updateRelationName))
    {
        cout << "SEMANTIC ERROR: Update column doesn't exists" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.updateConditionColName, parsedQuery.updateRelationName))
    {
        cout << "SEMANTIC ERROR: Update column doesn't exists" << endl;
        return false;
    }
        
    // cout<<parsedQuery.queryType <<endl;
    return true;
}

static bool evaluateBinOper(int value1, int value2, BinaryOperator binaryOperator)
{
    // cout<<"in function:- "<<binaryOperator<<endl;
    // cout<<value1<< "  "<<value2<<endl;
    switch (binaryOperator)
    {
    case LESS_THAN:
        // cout<<("less than");
        return (value1 < value2);
    case GREATER_THAN:
        // cout<<("greater than");
        return (value1 > value2);
    case LEQ:
        // cout<<("less than equal");
        return (value1 <= value2);
    case GEQ:
        // cout<<("greater than equal");
        return (value1 >= value2);
    case EQUAL:
        // cout<<("equal");
        return (value1 == value2);
    case NOT_EQUAL:
        // cout<<("not equal");
        return (value1 != value2);
    default:
        // cout<<("default");
        return false;
    }
}

void executeUPDATE()
{
    logger.log("executeUPDATE");

    Table table = *tableCatalogue.getTable(parsedQuery.updateRelationName);
    string fileName = "../data/temp/"  + parsedQuery.updateRelationName + "_index_" + parsedQuery.updateConditionColName + ".bpt";
    
    BPlusTree* index = new BPlusTree();
    int columnIndex = table.getColumnIndex(parsedQuery.updateConditionColName);

    int cnt = 2;

    if (!filesystem::exists(fileName)) {
        logger.log("index not available");
        
        Cursor cursor(parsedQuery.updateRelationName, 0);
        logger.log("creating index and updating records");
        
        vector<int> row = cursor.getNext();
        
        while(!row.empty())
        {
            
            bool conditionMet = evaluateBinOper(row[columnIndex], parsedQuery.updateConditionValue, parsedQuery.updateBinaryOperator);
            
            // cout<<cnt<<"   ->    "<< row[columnIndex]<< " ||  "<< parsedQuery.updateConditionValue << " ::  "<< parsedQuery.updateBinaryOperator<<endl;

            if(conditionMet) {
        
               
                int updateColIndex = table.getColumnIndex(parsedQuery.updateColName);
                row[updateColIndex] = parsedQuery.updateColValue;
        
                int currPageIndex = cursor.pageIndex;
        
                Page page = bufferManager.getPage(parsedQuery.updateRelationName, currPageIndex);
                vector<vector<int>> currBlockRecords = page.getRows();
        
                // cout<<cnt<<"   ->    "<<cursor.pagePointer<<endl;

                // // print currBlockRecords
                // for(int i = 0; i < currBlockRecords.size(); i++)
                // {
                //     for(int j = 0; j < currBlockRecords[i].size(); j++)
                //     {
                //         cout<<currBlockRecords[i][j]<<" ";
                //     }
                //     cout<<endl;
                // }
                // cout<<endl;
        
                currBlockRecords[cursor.pagePointer-1] = row;

                //   // print currBlockRecords
                //   for(int i = 0; i < currBlockRecords.size(); i++)
                //   {
                //       for(int j = 0; j < currBlockRecords[i].size(); j++)
                //       {
                //           cout<<currBlockRecords[i][j]<<" ";
                //       }
                //       cout<<endl;
                //   }
                //   cout<<endl;
        
                page.setRows(currBlockRecords);
        
                bufferManager.writePage(parsedQuery.updateRelationName, currPageIndex, currBlockRecords, currBlockRecords.size());
            }
            // Insert into index regardless of update
            index->insert(row[columnIndex], cursor.pageIndex, cursor.pagePointer);
            row = cursor.getNext();
            cnt++;
        }
        index->saveToFile(fileName);
        logger.log("created index");
    }
    else {
        logger.log("index already available");
        index = index->loadFromFile(fileName);

        Node* node = index->findLeaf(parsedQuery.updateConditionValue);
        
        unordered_map<int, vector<int>> pageRowMap;
        
        if(parsedQuery.updateBinaryOperator == LESS_THAN) {
        
            Node* temp = node;
        
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] < parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    else
                        break;
                }
                temp = temp->prev;
            }
            temp = node->next;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] < parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->next;
            }
        }
        else if(parsedQuery.updateBinaryOperator == GREATER_THAN) {
            Node* temp = node;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] > parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->next;
            }
            temp = node->prev;
            while(temp != nullptr) {
                for(int j = temp->keys.size()-1; j >= 0; j--) {
                    if(temp->keys[j] > parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->prev;
            }
        }
        else if(parsedQuery.updateBinaryOperator == EQUAL) {
            Node* temp = node;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] == parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->next;
            }
            temp = node->prev;
            while(temp != nullptr) {
                for(int j = temp->keys.size()-1; j >= 0; j--) {
                    if(temp->keys[j] == parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->prev;
            }
        }
        else if(parsedQuery.updateBinaryOperator == GEQ) {
            Node* temp = node;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] >= parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->next;
            }
            temp = node->prev;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] >= parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->prev;
            }
        }
        else if(parsedQuery.updateBinaryOperator == LEQ) {
            Node* temp = node;
            while(temp->prev != nullptr)
                temp = temp->prev;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] <= parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    else
                        break;
                }
                temp = temp->next;
            }
        }
        else if(parsedQuery.updateBinaryOperator == NOT_EQUAL) {
            Node* temp = node;
            while(temp->prev != nullptr)
                temp = temp->prev;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] != parsedQuery.updateConditionValue)
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                }
                temp = temp->next;
            }
        }

        // print pageRowMap
        // for(auto &entry : pageRowMap) {
        //     cout << "Page: " << entry.first << " Rows: ";
        //     for(auto &rowIndex : entry.second) {
        //         cout << rowIndex << " ";
        //     }
        //     cout << endl;
        // }
        // cout<<endl;

        // For every page with matching rows, update the target column
        int updateColIndex = table.getColumnIndex(parsedQuery.updateColName);

        for(auto &entry : pageRowMap) {
            Page page = bufferManager.getPage(parsedQuery.updateRelationName, entry.first);
            vector<vector<int>> rows = page.getRows();
            for(auto rowIndex : entry.second) {
                if(rowIndex < rows.size()){
                    rows[rowIndex][updateColIndex] = parsedQuery.updateColValue;
                }
            }
            page.setRows(rows);
            bufferManager.writePage(parsedQuery.updateRelationName, entry.first, rows, rows.size());
        }
        logger.log("update completed using index");

        string indexFileName = "../data/temp/" + table.tableName + "_index_" + parsedQuery.updateColName + ".bpt";
        
        if (filesystem::exists(indexFileName)) {
        
            filesystem::remove(indexFileName);
        
            Cursor cursor(table.tableName, 0);
        
            BPlusTree* index = new BPlusTree();
            vector<int> row = cursor.getNext();
            
            while(!row.empty())
            {
                index->insert(row[columnIndex], cursor.pageIndex, cursor.pagePointer - 1);
                row = cursor.getNext();
            }
            index->saveToFile(fileName);
        }
    

    }
    return;
}