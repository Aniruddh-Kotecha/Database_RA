#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: {
            if(tokenizedQuery[1] == "MATRIX") {
                return semanticParseEXPORTMatrix();
            }
            return semanticParseEXPORT();
        } 
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: {
            if(tokenizedQuery[1] == "MATRIX") {
                return semanticParseLOADMatrix();
            }
            return semanticParseLOAD();
        } 
        case PRINT: {
            if(tokenizedQuery[1] == "MATRIX") {
                return semanticParsePRINTMatrix();
            }
            return semanticParsePRINT();
        
        }
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();

        case ROTATE: return semanticParseROTATE();
        case CROSSTRANSPOSE: return semanticParseCROSSTRANSPOSE();
        case CHECKANTISYM: return semanticParseCHECKANTISYM();

        case ORDERBY: return semanticParseORDERBY();

        case GROUPBY : return semanticParseGROUPBY();

        case INSERT: return semanticParseINSERT();  
        case UPDATE: return semanticParseUPDATE();
        case DELETE: return semanticParseDELETE();
        case SEARCH: return semanticParseSEARCH();
        
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}