#include "global.h"
/**
 * @brief 
 * SYNTAX: DELETE FROM table_name WHERE condition
 * 
 */
bool syntacticParseDELETE()
{
    logger.log("syntacticParseDELETE");
    if (tokenizedQuery.size() != 7  || tokenizedQuery[1] != "FROM" || tokenizedQuery[3] != "WHERE")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = DELETE;
    parsedQuery.deleteRelationName = tokenizedQuery[2];
    parsedQuery.deleteConditionColName = tokenizedQuery[4];
    parsedQuery.deleteConditionValue = stoi(tokenizedQuery[6]);
    if (tokenizedQuery[5] == "<")
        parsedQuery.deleteBinaryOperator = LESS_THAN;
    else if (tokenizedQuery[5] == ">")
        parsedQuery.deleteBinaryOperator = GREATER_THAN;
    else if (tokenizedQuery[5] == "<=")
        parsedQuery.deleteBinaryOperator = LEQ;
    else if (tokenizedQuery[5] == ">=")
        parsedQuery.deleteBinaryOperator = GEQ;
    else if (tokenizedQuery[5] == "==")
        parsedQuery.deleteBinaryOperator = EQUAL;
    else if (tokenizedQuery[5] == "!=")
        parsedQuery.deleteBinaryOperator = NOT_EQUAL;
    else    
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    // cout << "true" << "\n";
    return true;
}

bool semanticParseDELETE()
{
    logger.log("semanticParseDELETE");

    if (!tableCatalogue.isTable(parsedQuery.deleteRelationName))
    {
        cout << "SEMANTIC ERROR: Update relation doesn't exists" << endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.deleteConditionColName, parsedQuery.deleteRelationName))
    {
        cout << "SEMANTIC ERROR: Update column doesn't exists" << endl;
        return false;
    }
        
    return true;
}
void executeDELETE()
{
    logger.log("executeDELETE");

    Table* table = tableCatalogue.getTable(parsedQuery.deleteRelationName);
    int columnIndex = table->getColumnIndex(parsedQuery.deleteConditionColName);
    
    string fileName = "../data/temp/"  + parsedQuery.deleteRelationName + "_index_" + parsedQuery.deleteConditionColName + ".bpt";
    BPlusTree* index = new BPlusTree();

    unordered_map<int, vector<int>> pageRowMap;

    if (!filesystem::exists(fileName)) {
        logger.log("index not available, creating index...");
        Cursor cursor(parsedQuery.deleteRelationName, 0);
        vector<int> row = cursor.getNext();

        while (!row.empty()) {
            index->insert(row[columnIndex], cursor.pageIndex, cursor.pagePointer - 1);
            if (evaluateBinOp(row[columnIndex], parsedQuery.deleteConditionValue, parsedQuery.deleteBinaryOperator)) {
                pageRowMap[cursor.pageIndex].push_back(cursor.pagePointer - 1);
            }
            row = cursor.getNext();
        }

        index->saveToFile(fileName);
        logger.log("index created.");
    } else {
        index = index->loadFromFile(fileName);
        Node* node = index->findLeaf(parsedQuery.deleteConditionValue);

        if (parsedQuery.deleteBinaryOperator == LESS_THAN) {
            Node* temp = node;
            while (temp != nullptr) {
                for (int j = 0; j < temp->keys.size(); j++) {
                    if (temp->keys[j] < parsedQuery.deleteConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->prev;
            }
        } else if (parsedQuery.deleteBinaryOperator == GREATER_THAN) {
            Node* temp = node;
            while (temp != nullptr) {
                for (int j = 0; j < temp->keys.size(); j++) {
                    if (temp->keys[j] > parsedQuery.deleteConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        } else if (parsedQuery.deleteBinaryOperator == EQUAL) {
            Node* temp = node;
            while (temp != nullptr) {
                for (int j = 0; j < temp->keys.size(); j++) {
                    if (temp->keys[j] == parsedQuery.deleteConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        } else if (parsedQuery.deleteBinaryOperator == GEQ) {
            Node* temp = node;
            while (temp != nullptr) {
                for (int j = 0; j < temp->keys.size(); j++) {
                    if (temp->keys[j] >= parsedQuery.deleteConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        } else if (parsedQuery.deleteBinaryOperator == LEQ) {
            Node* temp = node;
            while (temp->prev != nullptr) temp = temp->prev;
            while (temp != nullptr) {
                for (int j = 0; j < temp->keys.size(); j++) {
                    if (temp->keys[j] <= parsedQuery.deleteConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    } else break;
                }
                temp = temp->next;
            }
        } else if (parsedQuery.deleteBinaryOperator == NOT_EQUAL) {
            Node* temp = node;
            while (temp->prev != nullptr) temp = temp->prev;
            while (temp != nullptr) {
                for (int j = 0; j < temp->keys.size(); j++) {
                    if (temp->keys[j] != parsedQuery.deleteConditionValue) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        }
    }

    // Now process pageRowMap to delete rows
    for (auto& pageEntry : pageRowMap) {
        int pageIndex = pageEntry.first;
        vector<int>& rowIndices = pageEntry.second;

        sort(rowIndices.rbegin(), rowIndices.rend());  // delete from back

        Page page = bufferManager.getPage(table->tableName, pageIndex);
        vector<vector<int>> rows = page.getRows();

        vector<vector<int>> newRows;
        int deletedRowCount = 0;
        int newRowCount = 0;
        for (int i = 0; i < page.getRowCount(); i++) {
            if (find(rowIndices.begin(), rowIndices.end(), i) == rowIndices.end()) {
                newRows.push_back(rows[i]);
                newRowCount++;
            } else {
                deletedRowCount++;
            }
        }

        bufferManager.writePage(table->tableName, pageIndex, newRows, newRowCount);
        table->rowsPerBlockCount[pageIndex] = newRows.size();
        table->rowCount -= deletedRowCount;
    }
    
    for(int i = 0; i < table->columns.size(); i++) {
        string indexFileName = "../data/temp/" + table->tableName + "_index_" + table->columns[i] + ".bpt";
        if (filesystem::exists(indexFileName)) {
            filesystem::remove(indexFileName);
            Cursor cursor(table->tableName, 0);
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
}

