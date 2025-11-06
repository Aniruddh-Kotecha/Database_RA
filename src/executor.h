#pragma once
#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();

void executeLOADMatrix();
void executeEXPORTMatrix();
void executePRINTMatrix();
void executeROTATE();
void executeCROSSTRANSPOSE();
void executeCHECKANTISYM();
void executeORDERBY();
void executeGROUPBY();

void executeINSERT();
void executeUPDATE();
void executeDELETE();
void executeSEARCH();

    
bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);