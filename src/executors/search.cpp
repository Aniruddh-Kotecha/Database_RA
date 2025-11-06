#include "global.h"
#include "BPlusTree.h"
#include <filesystem>

/**
 * @brief 
 * SYNTAX: res_table <- SEARCH FROM table_name WHERE condition
 * 
 */
bool syntacticParseSEARCH()
{
    logger.log("syntacticParseDELETE");
    // for(auto it : tokenizedQuery) {
    //     cout << it << " ";
    // }
    // cout <<"done" << endl;
    if (tokenizedQuery.size() != 9  || tokenizedQuery[3] != "FROM" || tokenizedQuery[5] != "WHERE" || tokenizedQuery[1] != "<-")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = SEARCH;
    parsedQuery.searchResultRelationName = tokenizedQuery[0];
    parsedQuery.searchRelationName = tokenizedQuery[4];
    parsedQuery.searchConditionColName = tokenizedQuery[6];
    parsedQuery.searchConditionValue = stoi(tokenizedQuery[8]);
    parsedQuery.searchBinaryOperator = NO_BINOP_CLAUSE;

    string binaryOperator = tokenizedQuery[7];
    // cout << binaryOperator << endl;
    if (binaryOperator == "<")
        parsedQuery.searchBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.searchBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.searchBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.searchBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.searchBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.searchBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERRR" << endl;
        return false;
    }
    return true;
}

bool semanticParseSEARCH()
{
    logger.log("semanticParseSEARCH");

    if (!tableCatalogue.isTable(parsedQuery.searchRelationName))
    {
        cout << "SEMANTIC ERROR: Search relation doesn't exists" << endl;
        return false;
    }
    if(tableCatalogue.isTable(parsedQuery.searchResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant table already exists" << endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.searchConditionColName, parsedQuery.searchRelationName))
    {
        cout << "SEMANTIC ERROR: Search column doesn't exists" << endl;
        return false;
    }
        
    return true;
}
bool evaluateBinOper(int value1, int value2, BinaryOperator binaryOperator)
{
    switch (binaryOperator)
    {
    case LESS_THAN:
        return (value1 < value2);
    case GREATER_THAN:
        return (value1 > value2);
    case LEQ:
        return (value1 <= value2);
    case GEQ:
        return (value1 >= value2);
    case EQUAL:
        return (value1 == value2);
    case NOT_EQUAL:
        return (value1 != value2);
    default:
        return false;
    }
}
void executeSEARCH()
{
    logger.log("executeSEARCH");
    Table* table = tableCatalogue.getTable(parsedQuery.searchRelationName);
    // this->searchIn();
    
    // cout << "here" << "\n";
    Table* resultantTable = new Table(parsedQuery.searchResultRelationName, table->columns);
    
    string fileName = "../data/temp/"  + parsedQuery.searchRelationName + "_index_" + parsedQuery.searchConditionColName + ".bpt";
    BPlusTree* index = new BPlusTree();


    int columnIndex = table->getColumnIndex(parsedQuery.searchConditionColName);
    // cout << "columnIndex " << columnIndex << endl;
    // cout << fileName << "\n";
    if (!filesystem::exists(fileName)) {

        logger.log("index not available");
        Cursor cursor(parsedQuery.searchRelationName, 0);
        logger.log("creating index");
        vector<int> row = cursor.getNext();
        while(!row.empty())
        {
            index->insert(row[columnIndex], cursor.pageIndex, cursor.pagePointer - 1);
            bool a = evaluateBinOper(row[columnIndex], parsedQuery.searchConditionValue, parsedQuery.searchBinaryOperator);
            if(a) {
                resultantTable->writeRow(row);
            }
            row = cursor.getNext();
        }
        index->saveToFile(fileName);
        // index->printLeafNodes();
        // index->generateDotFile("index1.dot");
        logger.log("created index");
    }
    else {
        index = index->loadFromFile(fileName);
        // index->generateDotFile("index.dot");
        // index->printLeafNodes();

        Node* node = index->findLeaf(parsedQuery.searchConditionValue);
     
        int i = 0;
        unordered_map<int, vector<int>> pageRowMap;
        
        if(parsedQuery.searchBinaryOperator == LESS_THAN) {
            Node* temp = node;
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] < parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    else {
                        break;
                    }
                }
                temp = temp->prev;
            }
            temp = node->next;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] < parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    // else {
                    //     flag = false;
                    //     break;
                    // }
                }
                // if(!flag) {
                //     break;
                // }
                temp = temp->next;
            }
        }
        else if(parsedQuery.searchBinaryOperator == GREATER_THAN) {
            Node* temp = node;
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] > parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
            temp = node->prev;
            while(temp != nullptr) {
                for(int j = temp->keys.size() - 1; j >= 0; j--) {
                    if(temp->keys[j] > parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    // else {
                    //     flag = false;
                    //     break;
                    // }
                }
                // if(!flag) {
                //     break;
                // }
                temp = temp->prev;
            }
        }
        else if(parsedQuery.searchBinaryOperator == EQUAL) {
            Node* temp = node;
            // while(i < temp->keys.size() && temp->keys[i] < parsedQuery.searchConditionValue) {
            //     i++;
            // }
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    // cout << temp->keys[j] << " ";
                    if(temp->keys[j] == parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    // else {
                    //     flag = false;
                    //     break;
                    // }
                }
                // if(!flag) break;
                temp = temp->next;
                i = 0;
            }
            temp = node->prev;
            flag = true;
            while(temp != nullptr) {
                for(int j = temp->keys.size() - 1; j >= 0; j--) {
                    if(temp->keys[j] == parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    // else {
                    //     flag = false;
                    //     break;
                    // }
                    // else if(temp->keys[j] < parsedQuery.searchConditionValue) {
                    //     break;
                    // }
                }
                // if(!flag) break;

                temp = temp->prev;
            }
        }
        else if(parsedQuery.searchBinaryOperator == GEQ) {
            Node* temp = node;
            // while(i < temp->keys.size() && temp->keys[i] < parsedQuery.searchConditionValue) {
            //     i++;
            // }
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] >= parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
                i = 0;
            }
            temp = node->prev;
            while (temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] >= parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    // else {
                    //     flag = false;
                    // }
                }
                // if(!flag) break;
                temp = temp->prev;
                
            }
            
        }
        else if(parsedQuery.searchBinaryOperator == LEQ) {
            // Start from the leftmost leaf
            Node* temp = node;
            while(temp->prev != nullptr) {
                temp = temp->prev;
            }

            // Traverse all leaves from left to right
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] <= parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    } else {
                        // Since leaves are sorted, we can break once we find a value > condition
                        break;
                    }
                }
                temp = temp->next;
            }
        }
        else if(parsedQuery.searchBinaryOperator == NOT_EQUAL) {
            // Start from the leftmost leaf
            Node* temp = node;
            while(temp->prev != nullptr) {
                temp = temp->prev;
            }

            // Traverse all leaves from left to right
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] != parsedQuery.searchConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        }
        for(auto it : pageRowMap) {
            Page page = bufferManager.getPage(parsedQuery.searchRelationName, it.first);
            for(auto it1 : it.second) {
                vector<int> row = page.getRows()[it1];
                resultantTable->writeRow(row);
            } 
        
        }
    }
    if(resultantTable->blockify()) {
        tableCatalogue.insertTable(resultantTable);
    }
    


    return;
}



        
        
        
        
        
        
        