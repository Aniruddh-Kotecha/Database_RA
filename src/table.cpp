#include "global.h"
#include "BPlusTree.h"
/**
 * @brief Construct a new Table:: Table object
 *
 */
// int HASH_MOD = 9;
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    logger.log(to_string(this->maxRowsPerBlock) + to_string(this->columnCount));
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    logger.log("===");
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    // logger.log("===1");

    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        if(row.size() > 0) {

            this->writeRow(row, cout);
        }
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        if(row.size() > 0) {
            this->writeRow(row, fout);
        }
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    // cout << columnName << " " << this->columnCount  << "\n";
    // logger.log(to_string(this->columnCount));
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        // cout << this->columns[columnCounter] << "\n";
        // logger.log(this->columns[columnCounter] + " " + columnName);
        if (this->columns[columnCounter] == columnName) {
            // logger.log(to_string(columnCounter));
            return columnCounter;
        }
    }
    return -1;
}
static bool comparator(const vector<int> &a, const vector<int> &b, Table &table, vector<string> &sortColumns, 
    vector<SortingStrategy> sortingStrategy) {
    for (int i = 0; i < sortColumns.size(); i++) {
        int colIdx = table.getColumnIndex(sortColumns[i]);
       
        SortingStrategy strategy = sortingStrategy[i];

        if (a[colIdx] != b[colIdx]) {

            if(strategy == ASC) {
                return a[colIdx] < b[colIdx]; 
            }
            else {
                return a[colIdx] > b[colIdx];
            }
        }
    }
    return false;
}

void sortingPhase(Table &table, int totalBlocks, vector<string> &sortColumns, 
    vector<SortingStrategy> &sortingStrategy) {
    logger.log("Running sortingPhase");
    int dm = BLOCK_COUNT;

    for(int i = 0; i < totalBlocks; i += dm) {

        int numOfBlocks = min(dm, totalBlocks - i);

        for(int j = 0; j < numOfBlocks; j++) {

            // vector<vector<int>> rows = bufferManager.getPage(table.tableName, i + j).getRows();
            Page page = bufferManager.getPage(table.tableName, i + j);
            vector<vector<int>> rows = page.getRows();
            rows.resize(page.getRowCount());
            logger.log("Page num " + to_string(i + j));
            // if((i + j) == totalBlocks - 1) {

            //     int lastBlockRows = table.rowCount % table.maxRowsPerBlock;
            //     rows.resize(lastBlockRows); 
            // }
            
            sort(rows.begin(), rows.end(), [&](const vector<int> &a, const vector<int> &b) {
                return comparator(a, b, table, sortColumns, sortingStrategy);
            });
            logger.log("rows size "  + to_string(rows.size()));
            // logger.log(to_string(i) + " " + to_string(rows.size()));   
            bufferManager.writePage(table.tableName, i + j, rows, rows.size());                    
        }
    }
}
void mergingPhase(Table &table, int blockCount, int columnCount, vector<string> &sortColumns, 
    vector<SortingStrategy> &sortingStrategy) {
    logger.log("Running Merging Phase");
    int dm = BLOCK_COUNT - 1;
    auto comp = [&table, &sortColumns, &sortingStrategy](const pair<vector<int>, int> &a,
        const pair<vector<int>, int> &b) {
        return !comparator(a.first, b.first, table, sortColumns, sortingStrategy);
    };


    for(int runs = 1; runs < blockCount; runs = runs * dm) {
        
        int resultBufferNum = blockCount;
        
        for(int start = 0; start < blockCount;  start += dm * runs){
            int runsProcessed = 0;
            
            priority_queue<pair<vector<int>, int>, vector<pair<vector<int>,  int>>, decltype(comp)> pq(comp);

            vector<Cursor> cursorVector;
            
            vector<vector<int>> resultBuffer;

            vector<int> cursorEndIndices;
            int j = 0;

            while(j < dm && start + j * runs < blockCount) {
                int cursorIdx = start + j * runs;
                
                if(cursorIdx >= blockCount) {
                    break;
                }
                // cout << cursorIdx << " ";
                int cursorEndIdx = min((cursorIdx + runs - 1), (blockCount - 1));
                // cout << cursorIdx << "  " << cursorEndIdx << "\n";
                Cursor cursor(table.tableName, cursorIdx);

                cursorVector.push_back(cursor);
                cursorEndIndices.push_back(cursorEndIdx);

                vector<int> row = cursorVector.back().getNextRowFromCurrPage();
                pq.push({row, j});
                j++;
                runsProcessed++;
            }
            if(runsProcessed <= 1) continue;
            while(!pq.empty()) {
                auto it = pq.top();
                
                pq.pop();
                vector<int> row = it.first;
                int currCursorIndex = it.second;

                resultBuffer.push_back(row);
                if(resultBuffer.size() == table.maxRowsPerBlock) {
                    bufferManager.writePage(table.tableName, resultBufferNum, resultBuffer, resultBuffer.size());
                    resultBufferNum++;
                    resultBuffer.clear();
                }

                vector<int> newRow = cursorVector[currCursorIndex].getNextRowFromCurrPage();

                if(newRow.empty()) {
                    int currentBlock = cursorVector[currCursorIndex].pageIndex;
                    if (currentBlock < cursorEndIndices[currCursorIndex]) {
                        cursorVector[currCursorIndex] = Cursor(table.tableName, currentBlock + 1);
                        newRow = cursorVector[currCursorIndex].getNextRowFromCurrPage();
                    }
                }
                if(!newRow.empty()) {
                    pq.push({newRow, currCursorIndex});
                }
            }
            if(resultBuffer.size() > 0) {
                // cout << resultBufferNum << " " <<resultBuffer.size()<<"\n";
                bufferManager.writePage(table.tableName, resultBufferNum, resultBuffer, resultBuffer.size());
                resultBufferNum++;
                resultBuffer.clear();
            }
        }
        for(int i = 0; i < blockCount; i++) {
            logger.log(to_string(blockCount + i));
            if(blockCount + i >= resultBufferNum) {
                break;
            }
            Page page1 = bufferManager.getPage(table.tableName, i);
            Page page2 = bufferManager.getPage(table.tableName, blockCount + i);
            
            page1.swapContent(page2);
            bufferManager.writeExistingTablePage(page1);
            // bufferManager.writeExistingTablePa(page1);

            // vector<vector<int>> rows = bufferManager.getPage(table.tableName, blockCount + i).getRows();
            // // cout << rows.size() << " ";
            // bufferManager.writePage(table.tableName, i, rows, rows.size());
            // logger.log("completed 1 " + to_string(i) + " " + to_string(blockCount));
            bufferManager.deleteFile(table.tableName, blockCount +  i);
            // logger.log("completed 2");   
        }
        // if(dm >= blockCount) {
        //     break;
        // }
    }
}

void Table::sortRelation() {
    
    logger.log("Table:sortRelation");
    for(auto it : parsedQuery.sortColumnNames) {
        logger.log(it);
    }
    // int t = this->rowsPerBlockCount[this->blockCount - 1];
    // cout << " t " << t  << "\n";
    sortingPhase(*this, this->blockCount, parsedQuery.sortColumnNames, parsedQuery.sortingStrategy);
    //  t = this->rowsPerBlockCount[this->blockCount - 1];
    // cout << " t " << t  << "\n";

    mergingPhase (*this, this->blockCount, this->columnCount, parsedQuery.sortColumnNames, parsedQuery.sortingStrategy);
    //  t = this->rowsPerBlockCount[this->blockCount - 1];
    
}

void Table::copyTable(Table &table){
    logger.log("Table::copyTable");
    this->columns = table.columns;
    this->distinctValuesPerColumnCount = table.distinctValuesPerColumnCount;
    this->columnCount = table.columnCount;
    this->rowCount = table.rowCount;
    this->blockCount = table.blockCount;

    this->maxRowsPerBlock = table.maxRowsPerBlock;
    this->rowsPerBlockCount = table.rowsPerBlockCount;
    this->indexed = table.indexed;
    this->indexedColumn = table.indexedColumn;
    this->indexingStrategy = table.indexingStrategy;

    for(int i = 0; i < blockCount; i++) {
        Page page = bufferManager.getPage(table.tableName, i);
        bufferManager.writePage(this->tableName, i, page.getRows(), page.getRowCount());
    }
    
}





// =========================== join ===========================

int hashFunction(int num){
    int dm = BLOCK_COUNT - 1;
    return num%dm;
}


vector<vector<int>> hashingPhase(string tableName, string columnName){

    logger.log("Hashing phase for  : " + tableName);
    int dm = BLOCK_COUNT - 1;
    Table *relation = tableCatalogue.getTable(tableName);

    int maxRow = relation->maxRowsPerBlock;
    int totalBlock = relation->blockCount;
    int blockCounter = totalBlock;

    vector<vector<int>> mappingHashToPages(dm);
    vector<vector<vector<int>>>hashTable(dm);

    int joinColumnIndex = -1;

    vector<string> allColumns = relation->columns;

    for(int i=0; i<allColumns.size() ; i++){

        if(allColumns[i] == columnName){

            joinColumnIndex = i;
            break;
        }
    }

    for (int i=0; i<totalBlock ; ++i){

        logger.log(tableName + "  -   " + to_string(i));
        
        // vector<vector<int>> currBlock = bufferManager.getPage(tableName , i).getRows();

        // if( i == totalBlock - 1){
            
        //     int lastBlockRow = relation->rowCount % (long long)maxRow;
        //     currBlock.resize(lastBlockRow);
        // }

        Page currBlock = bufferManager.getPage(tableName , i);
        vector<vector<int>> currBlockRecords = currBlock.getRows();


        for(int j=0; j<currBlock.getRowCount() ; j++){

            vector<int>currRecord =  currBlockRecords[j];
            int hash = hashFunction(currRecord[joinColumnIndex]);
            
            hashTable[hash].push_back(currRecord);

            // logger.log("for  " + to_string(currRecord[joinColumnIndex]) + "  -> hash :- " + to_string(hash));

            if(hashTable[hash].size() == maxRow){
                
                // logger.log(" ----  for    " + to_string(hash) + " block is fulll!!");
                Page hashedPage (tableName , blockCounter , hashTable[hash] , hashTable[hash].size());
                relation->rowsPerBlockCount.push_back(hashTable[hash].size());

                hashedPage.writePage();
                
                mappingHashToPages[hash].push_back(blockCounter);
                
                hashTable[hash].clear();

                ++blockCounter;
            }
        }

        
    }

    for(int i=0; i<hashTable.size(); i++){

        if(hashTable[i].size()){

            // logger.log("for  :- " + to_string(i)  + "  size :- " + to_string(hashTable[i].size()));
            Page hashedPage (tableName , blockCounter , hashTable[i] , hashTable[i].size());
            relation->rowsPerBlockCount.push_back(hashTable[i].size());

            hashedPage.writePage();
            
            mappingHashToPages[i].push_back(blockCounter); 
            hashTable[i].clear();

            ++blockCounter;
        }


    }

    relation->blockCount = blockCounter;
    
    return mappingHashToPages;
}


 
// first = outer & second = inner

void probingPhase(string firstTableName , string secondTableName , long long firstRelationOriginalBlockCount, long long secondRelationOriginalBlockCount, vector<vector<int>> firstRelationMap , vector<vector<int>> secondRelationMap){

    logger.log("Probing phase for  : " + firstTableName + "  and   " + secondTableName );
    int dm = BLOCK_COUNT - 1;
    Table* firstRelation = tableCatalogue.getTable(firstTableName);
    Table* secondRelation = tableCatalogue.getTable(secondTableName);
    Table* resulantTable = tableCatalogue.getTable(parsedQuery.joinResultRelationName);


    long long int resultRowCount = 0;

    int outerRelationJoinColIndex = -1;
    vector<string> firstRelationColumns = firstRelation->columns;

    for(int i=0; i<firstRelationColumns.size() ; i++){

        if(firstRelationColumns[i] == parsedQuery.joinFirstColumnName){

            outerRelationJoinColIndex = i;
            break;
        }
    }


    int innerRelationJoinColIndex = -1;
    vector<string> secondRelationColumns = secondRelation->columns;

    for(int i=0; i<secondRelationColumns.size() ; i++){

        if(secondRelationColumns[i] == parsedQuery.joinSecondColumnName){

            innerRelationJoinColIndex = i;
            break;
        }
    }

    vector<string>resultColumns;
    
    for(auto it:firstRelationColumns)
        resultColumns.push_back(it);
        
    for(auto it:secondRelationColumns)
        resultColumns.push_back(it);
    

    vector<vector<int>> resultBlock ;
    int maxRow = firstRelation->maxRowsPerBlock;
    int resultBlockCounter = 0;

    logger.log("-------------------  columns -----------------------------" );

    for(auto it: resultColumns)
        logger.log(it);

    logger.log("------------------------------------------------------------");

    // for each hash 
    for(int hash = 0 ; hash < dm ; ++hash){

        vector<int> firstRelationHashPaeges = firstRelationMap[hash];
        vector<int> secondRelationHashPages = secondRelationMap[hash];
// 
        // logger.log("hash :-   " + to_string(hash) + "     -   first hash size:- " + to_string(firstRelationHashPaeges.size()) + "     -  second hash size :-  " + to_string(secondRelationHashPages.size()));
        // logger.log("hash :-   " + to_string(hash) + "     -   first hash size:- " + to_string(firstRelationHashPaeges.size()) + "     -  second hash size :-  " + to_string(secondRelationHashPages.size()));
        // cout<<("hash :-   " + to_string(hash) + "     -   first hash size:- " + to_string(firstRelationHashPaeges.size()) + "     -  second hash size :-  " + to_string(secondRelationHashPages.size()))<<endl;
        
        if(!firstRelationHashPaeges.size() || !secondRelationHashPages.size())
            continue;
        
        vector<Page> outerRelationBlocks;

        // cout<<firstRelationOriginalBlockCount<<"   ->   "<<firstRelation->blockCount<<endl;

        // for outer relation blocks loop
        for(int i = 0; i < firstRelationHashPaeges.size(); i += (BLOCK_COUNT-2)) {
    
            // Load at most (BLOCK_COUNT - 2) pages from firstRelation's hash partition
            vector<Page> outerRelationBlocks;
            
            int end = min( (int)(i + (BLOCK_COUNT - 2)), (int)(firstRelationHashPaeges.size()));

            logger.log("i:-   " + to_string(i)  + "   ->end :-  " + to_string(end) );

            for(int j = i; j < end; j++) {
                logger.log("table name :- " + firstTableName  + "  hashpage index  ;- " + to_string(firstRelationHashPaeges[j]));
                Page block = bufferManager.getPage(firstTableName, firstRelationHashPaeges[j]);
                outerRelationBlocks.push_back(block);
            }

            // cout<<outerRelationBlocks.size()<<endl;

            // Process records in these outer blocks
            for(int outerBlockIndex = 0; outerBlockIndex < outerRelationBlocks.size(); outerBlockIndex++) {
        
                Page currOuterBlock = outerRelationBlocks[outerBlockIndex];
                vector<vector<int>> currOuterRecords = currOuterBlock.getRows();

                // cout<<"outerBlockIndex   :-  "<<outerBlockIndex<<endl;

                for(int outerRecordIndex = 0; outerRecordIndex < currOuterBlock.getRowCount(); outerRecordIndex++) {


                    // cout<<"outerRecordIndex   :-   "<<outerRecordIndex<<endl;
        
                    // Iterate only over relevant inner hash pages
                    for(int innerBlockIndex = 0; innerBlockIndex < secondRelationHashPages.size(); innerBlockIndex++) {
                        
                        // cout<<"innerBlockIndex   :-    "<<innerBlockIndex<<endl;

                        Page currInnerBlock = bufferManager.getPage(secondTableName, secondRelationHashPages[innerBlockIndex]); // Corrected
                        vector<vector<int>> currInnerRecords = currInnerBlock.getRows();
        
                        for(int innerRecordIndex = 0; innerRecordIndex < currInnerBlock.getRowCount(); innerRecordIndex++) {
                            
                            // cout<<"innerRecordIndex :- "<<innerRecordIndex<<endl;

                            // Check for matching join keys

                            // cout<<"value : "<<currOuterRecords[outerRecordIndex][outerRelationJoinColIndex]<<"  ->   "<<currInnerRecords[innerRecordIndex][innerRelationJoinColIndex]<<endl;
                            if(currOuterRecords[outerRecordIndex][outerRelationJoinColIndex] == 
                               currInnerRecords[innerRecordIndex][innerRelationJoinColIndex]) {
        
                                vector<int> resultRecord;
        
                                resultRecord.insert(resultRecord.end(), 
                                                    currOuterRecords[outerRecordIndex].begin(), 
                                                    currOuterRecords[outerRecordIndex].end());
                                resultRecord.insert(resultRecord.end(), 
                                                    currInnerRecords[innerRecordIndex].begin(), 
                                                    currInnerRecords[innerRecordIndex].end());
                                
                                resultBlock.push_back(resultRecord);
                            }
                            // cout<<"resultBlock :- "<<resultBlock.size()<<"  ->  maxRow :- "<<maxRow<<endl;

                            if(resultBlock.size() == maxRow) {
                                
                                resultRowCount += resultBlock.size();
                                Page resultPage(parsedQuery.joinResultRelationName, resultBlockCounter, resultBlock, resultBlock.size());
                                
                                resulantTable->rowsPerBlockCount.push_back(resultBlock.size());

                                resultPage.writePage();
                                resultBlock.clear();
                                resultBlockCounter++;
                            }
                        }
                    }
                }
            }
        }
        



    }
    // cout<<"last block write pending  & last block size is :"<<resultBlock.size()<<endl;
    if(resultBlock.size() != 0) {

        // cout<<"writing start"<<endl;

        resultRowCount += resultBlock.size();
        Page resultPage(parsedQuery.joinResultRelationName, resultBlockCounter, resultBlock, resultBlock.size());
        
        // cout<<"result block (page) object made"<<endl;

        // cout<<"resultblockcount sizze :- "<<resultBlock.size()<<endl;
        // cout<<"count start"<<endl;

        // cout<< resulantTable->sourceFileName <<endl;

        // for(auto it: resulantTable->rowsPerBlockCount)
        //     cout<<it<<endl;
        
            // cout<<"count end"<<endl;
        resulantTable->rowsPerBlockCount.push_back(resultBlock.size());

        // cout<<"done the entru of rowsperblockcount"<<endl;
        resultPage.writePage();
        
        resultBlock.clear();
        resultBlockCounter++;
    }
    
    //updating result metadata
    resulantTable->columns = resultColumns;
    resulantTable->columnCount = resultColumns.size();
    resulantTable->blockCount = resultBlockCounter;
    resulantTable->maxRowsPerBlock = maxRow;
    resulantTable->rowCount = resultRowCount;
}

void Table::joinRelation(){

    logger.log("Table:: joinRelation");

    Table* firstRelation = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table* secondRelation = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    long long firstRelationOriginalBlockCount = firstRelation->blockCount;
    long long secondRelationOriginalBlockCount = secondRelation->blockCount;


    vector<vector<int>> firstRelationMap = hashingPhase(parsedQuery.joinFirstRelationName , parsedQuery.joinFirstColumnName);
    vector<vector<int>> secondRelationMap = hashingPhase(parsedQuery.joinSecondRelationName , parsedQuery.joinSecondColumnName);

    probingPhase(parsedQuery.joinFirstRelationName , parsedQuery.joinSecondRelationName , firstRelationOriginalBlockCount , secondRelationOriginalBlockCount , firstRelationMap , secondRelationMap);
    

    
    for(auto hash: firstRelationMap)
        for(auto pageIndex : hash)
            bufferManager.deleteFile(parsedQuery.joinFirstRelationName, pageIndex);


    for(auto hash:secondRelationMap)
        for(auto pageIndex : hash)
            bufferManager.deleteFile(parsedQuery.joinSecondRelationName , pageIndex);

    firstRelation->rowsPerBlockCount.resize(firstRelationOriginalBlockCount);
    secondRelation->rowsPerBlockCount.resize(secondRelationOriginalBlockCount);

    firstRelation->blockCount = firstRelationOriginalBlockCount;
    secondRelation->blockCount = secondRelationOriginalBlockCount;

}
// ======================++++Group By ===================================== // 

void setDefaultAggregateValues(vector<int>& aggregateValues){
    aggregateValues[MAX] = INT_MIN;
    aggregateValues[MIN] = INT_MAX;
    aggregateValues[COUNT] = 0;
    aggregateValues[SUM] = 0;
    aggregateValues[AVG] = 0;
}

void updateAggregateValues(int colIndex, vector<int>& row, vector<int>& aggregateValues, AggregateOpType aggregateOpType){

    if(aggregateOpType == MIN){
        aggregateValues[MIN] = min(aggregateValues[MIN], row[colIndex]);
    }
    else if(aggregateOpType == MAX){
        aggregateValues[MAX] = max(aggregateValues[MAX], row[colIndex]);
    }
    else if(aggregateOpType == COUNT){
        aggregateValues[COUNT]++;
    }
    else if(aggregateOpType == SUM){
        aggregateValues[SUM] += row[colIndex];
    }
    else if(aggregateOpType == AVG){
        aggregateValues[COUNT]++;
        aggregateValues[SUM] += row[colIndex];
        aggregateValues[AVG] = aggregateValues[SUM]/aggregateValues[COUNT];
    }
}

string getAggregateName(AggregateOpType aggregateOpType){
    switch (aggregateOpType)
    {
    case MIN:
        return "MIN";
    case MAX:
        return "MAX";
    case COUNT:
        return "COUNT";
    case SUM:
        return "SUM";
    case AVG:
        return "AVG";    
    default:
        break;
    }
    return "";
}

void Table::groupByRelation(){
    /*  
        SYNTAX:  R <- GROUP BY col1 FROM relation1 HAVING aggOp(col2) binOp aggVal RETURN aggOp(col3)
        Assuming only 1 attribute in each of GROUP BY, HAVING and RETURN
        Variables added to parsedQuery - 
        sort the data by the groupByColumnName
        cursor to check each record of groupByTableName
        aggregate using havingAggreagateOp on column havingAggregateColumnName against 
            value in havingAggregateValue using havingBinaryOp
        Return aggregate function in returnAggreagateOp on returnColumnName
        Export into Table groupByResultTableName with return column name by aggregate function concatenated with return column name
    */

    logger.log("Table::groupByRelation");
    //Sort to be added.

    Cursor cursor = getCursor();
    vector<int> row = cursor.getNext();
    int groupByColumnIndex = this->getColumnIndex(parsedQuery.groupByColumnName);
    int groupByColumnValue = row[groupByColumnIndex];

    int aggregateColumnIndex = this->getColumnIndex(parsedQuery.havingAggregateColumnName);
    int returnColumnIndex = this->getColumnIndex(parsedQuery.returnColumnName);

    vector<string> columns;
    columns.push_back(parsedQuery.groupByColumnName);
    string newAggregateColumnName = getAggregateName(parsedQuery.returnAggreagateOp) + parsedQuery.returnColumnName;
    columns.push_back(newAggregateColumnName);

    Table* resultTable = new Table(parsedQuery.groupByResultTableName, columns);

    vector<int> aggregateValues(5);
    setDefaultAggregateValues(aggregateValues);

    while(!row.empty()){
        if(groupByColumnValue != row[groupByColumnIndex]){
            if(evaluateBinOp(aggregateValues[parsedQuery.havingAggreagateOp], parsedQuery.havingAggregateValue, parsedQuery.havingBinaryOp)){
                vector<int> result;
                result.push_back(groupByColumnValue);
                result.push_back(aggregateValues[parsedQuery.returnAggreagateOp]);
                resultTable->writeRow(result);
            }
            setDefaultAggregateValues(aggregateValues);

            groupByColumnValue = row[groupByColumnIndex];
        }
        
        updateAggregateValues(aggregateColumnIndex, row, aggregateValues, parsedQuery.havingAggreagateOp);
        if(parsedQuery.havingAggreagateOp != parsedQuery.returnAggreagateOp){
            updateAggregateValues(returnColumnIndex, row, aggregateValues, parsedQuery.returnAggreagateOp);
        }
        

        row = cursor.getNext();
    }

    if(evaluateBinOp(aggregateValues[parsedQuery.havingAggreagateOp], parsedQuery.havingAggregateValue, parsedQuery.havingBinaryOp)){
        vector<int> result;
        result.push_back(groupByColumnValue);
        result.push_back(aggregateValues[parsedQuery.returnAggreagateOp]);
        resultTable->writeRow(result);
    }
    if(resultTable->blockify())
        tableCatalogue.insertTable(resultTable);
    else {
        cout<<"Empty Table"<<endl;
        resultTable->unload();
        delete resultTable;
    }
}

// void Table::createIndex(string columnName) {
//     logger.log("Table::createIndex");

//     Table indexedTable(tableName + "_index_" + columnName); 
//     tableCatalogue.insertTable(&indexedTable);

//     BPlusTree* index = new BPlusTree(this->tableName, columnName);
    
//     Cursor cursor = this->getCursor();
//     vector<int> row = cursor.getNext();
//     int columnIndex = this->getColumnIndex(columnName);
//     logger.log("column Index" + to_string(columnIndex));
//     while(!row.empty()) {
//         logger.log("Inserting " + to_string(cursor.pageIndex) );
//         index->insert(row[columnIndex], cursor.pageIndex);
//         row = cursor.getNext();
//     }   
//     logger.log("Fetched all rows");
    
//     // Delete old index if it exists
//     if (this->indexes.find(columnName) != this->indexes.end()) {
//         delete this->indexes[columnName];
//     }
    
//     this->indexes[columnName] = index;
//     string tableName = this->tableName;
// }

// BPlusTree* Table::getBPlusTree(string columnName) {
//     logger.log("Table::getIndex");
    
//     if(this->indexes.find(columnName) == this->indexes.end()) {
//         createIndex(columnName);
//     }

//     return this->indexes[columnName];    
// }


pair<int, int> Table::insertRow(vector<string> columns, vector<int> values) {
    logger.log("Table::insertRow");
    vector<int> newRow(this->columnCount, 0);

    for (int i = 0; i < columns.size(); i++) {
        int columnIndex = this->getColumnIndex(columns[i]);
        newRow[columnIndex] = values[i];
    }

    // Handle case when there are no blocks (first row)
    if (this->blockCount == 0) {
        vector<vector<int>> newRows;
        newRows.push_back(newRow);
        this->rowsPerBlockCount.push_back(1);
        bufferManager.writePage(this->tableName, 0, newRows, 1);
        this->blockCount = 1;
        this->rowCount = 1;
        return {0, 0};
    }

    // Handle case when there are existing blocks
    Page page = bufferManager.getPage(this->tableName, this->blockCount - 1);

    if(page.getRowCount() == this->maxRowsPerBlock) {
        vector<vector<int>> newRows;

        newRows.push_back(newRow);
        this->rowsPerBlockCount.push_back(1);
        // cout << "new rows " << newRows.size() << "\n";
        bufferManager.writePage(this->tableName, this->blockCount, newRows, 1);
        this->blockCount++;
        this->rowCount++;
        return {this->blockCount - 1, 0};
    }   
    page.addRow(newRow);
    this->rowsPerBlockCount[this->blockCount - 1]++;
    this->rowCount++;
    page.writePage();
    

    return {this->blockCount - 1, this->rowsPerBlockCount[this->blockCount - 1] - 1};
}
bool evaluateOperation(int value1, int value2, BinaryOperator binaryOperator)
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

unordered_map<int, vector<int>>  Table::searchInTable(string tableName, string columnName, int value, BinaryOperator binaryOperator) {
    logger.log("Table::searchInTable");
    // cout << "sera" << "\n";
    // cout << "searchInTable " << tableName << " " << columnName;
    string fileName = "../data/temp/"  + tableName + "_index_" + columnName + ".bpt";
    // cout << "1";
    BPlusTree* index = new BPlusTree();
    // cout << "2";

    int columnIndex = this->getColumnIndex(columnName);
    // cout << columnIndex;
    unordered_map<int, vector<int>> pageRowMap;

    if (!filesystem::exists(fileName)) {
        logger.log("index not available");
        Cursor cursor(tableName, 0);
        logger.log("creating index");
        vector<int> row = cursor.getNext();
        while(!row.empty())
        {
            index->insert(row[columnIndex], cursor.pageIndex, cursor.pagePointer - 1);
            bool a = evaluateOperation(row[columnIndex], value, binaryOperator);
            if(a) {
                pageRowMap[cursor.pageIndex].push_back(cursor.pagePointer - 1);
            }
            row = cursor.getNext();
        }
        index->saveToFile(fileName);
    }
    else {
        index = index->loadFromFile(fileName);

        Node* node = index->findLeaf(value);
     
        int i = 0;
        
        if(binaryOperator == LESS_THAN) {
            Node* temp = node;
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] < value) {
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
                    if(temp->keys[j] < value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        }
        else if(binaryOperator == GREATER_THAN) {
            Node* temp = node;
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] > value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
            temp = node->prev;
            while(temp != nullptr) {
                for(int j = temp->keys.size() - 1; j >= 0; j--) {
                    if(temp->keys[j] > value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->prev;
            }
        }
        else if(binaryOperator == EQUAL) {
            Node* temp = node;
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] == value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                    temp = temp->next;
                    i = 0;
                }
                temp = node->prev;
                flag = true;
                while(temp != nullptr) {
                    for(int j = temp->keys.size() - 1; j >= 0; j--) {
                        if(temp->keys[j] == value) {
                            pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                        }
                    }
                    temp = temp->prev;
                }
            }
        }
        else if(binaryOperator == GEQ) {
            Node* temp = node;
            bool flag = true;
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] >= value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
                i = 0;
            }
            temp = node->prev;
            while (temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] >= value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->prev;
                
            }
            
        }
        else if(binaryOperator == LEQ) {
            // Start from the leftmost leaf
            Node* temp = node;
            while(temp->prev != nullptr) {
                temp = temp->prev;
            }

            // Traverse all leaves from left to right
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] <= value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    } else {
                        // Since leaves are sorted, we can break once we find a value > condition
                        break;
                    }
                }
                temp = temp->next;
            }
        }
        else if(binaryOperator == NOT_EQUAL) {
            // Start from the leftmost leaf
            Node* temp = node;
            while(temp->prev != nullptr) {
                temp = temp->prev;
            }

            // Traverse all leaves from left to right
            while(temp != nullptr) {
                for(int j = 0; j < temp->keys.size(); j++) {
                    if(temp->keys[j] != value) {
                        pageRowMap[temp->values[j].pageIndex].push_back(temp->values[j].rowIndex);
                    }
                }
                temp = temp->next;
            }
        }

    }

    return pageRowMap;
}