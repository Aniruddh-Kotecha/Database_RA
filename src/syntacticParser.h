#pragma once

#include "tableCatalogue.h"
#include "matrixCatalogue.h"
#include "BPlusTree.h"
#include "global.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    ROTATE,
    CROSSTRANSPOSE,
    CHECKANTISYM,
    ORDERBY,
    UNDETERMINED,
    GROUPBY, 
    INSERT,
    UPDATE,
    DELETE,
    SEARCH
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum AggregateOpType
{
    MAX,
    MIN,
    COUNT,
    SUM,
    AVG,
    NO_AGGREGATE_OP_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    vector<SortingStrategy> sortingStrategy;
    string sortResultRelationName = "";
    vector<string> sortColumnNames;
    string sortRelationName = "";

    string sourceFileName = "";


    // group by

    string groupByResultTableName = "";
    string groupByColumnName = "";
    string groupByTableName = "";
    AggregateOpType havingAggreagateOp = NO_AGGREGATE_OP_CLAUSE;
    string havingAggregateColumnName = "";
    BinaryOperator havingBinaryOp = NO_BINOP_CLAUSE;
    int havingAggregateValue = 0;
    AggregateOpType returnAggreagateOp = NO_AGGREGATE_OP_CLAUSE;
    string returnColumnName = "";    



    //: Matrix variables
    
    string exportMatrixName = "";

    string loadMatrixName = "";

    string printMatrixName = "";

    string rotateMatrixName = "";

    string crossTransposeFirstMatrixName = "";
    string crossTransposeSecondMatrixName = "";


    string antiSymFirstMatrixName = "";
    string antiSymSecondMatrixName = "";

    string orderByResultantTableName = "";
    string orderByColumnName = "";
    string orderByRelationName = "";
    SortingStrategy orderBySortingStrategy = NO_SORT_CLAUSE;

    // insert 
    string insertResultRelationName = "";
    vector<string> insertCol;
    vector<int> insertVal;

    // update
    string updateRelationName = "";
    string updateColName = "";
    int updateColValue = 0;
    string updateConditionColName = "";
    BinaryOperator updateBinaryOperator = NO_BINOP_CLAUSE;
    int updateConditionValue = 0;


    // delete
    string deleteRelationName = "";
    string deleteConditionColName = "";
    BinaryOperator deleteBinaryOperator = NO_BINOP_CLAUSE;
    int deleteConditionValue = 0;

    // SEARCH
    string searchRelationName = "";
    string searchConditionColName = "";
    string searchResultRelationName = "";
    BinaryOperator searchBinaryOperator = NO_BINOP_CLAUSE;
    int searchConditionValue = 0;

    ParsedQuery();
    void clear();
};

//res_table <- SEARCH FROM table_name WHERE condition
bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();

bool syntacticParseEXPORTMatrix();
bool syntacticParseLOADMatrix();
bool syntacticParsePRINTMatrix();
bool syntacticParseROTATE();
bool syntacticParseCROSSTRANSPOSE();
bool syntacticParseCHECKANTISYM();

bool syntacticParseGROUPBY();
bool syntacticParseORDERBY();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);


bool syntacticParseINSERT();
bool syntacticParseUPDATE();
bool syntacticParseDELETE();
bool syntacticParseSEARCH();
