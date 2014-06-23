#ifndef CREATEFILE_H_INCLUDED
#define CREATEFILE_H_INCLUDED
#include "benchmarkFunctions.h"

void createFile(InfoAboutFile fileInfo, long long int* , int , long long int, int, int numIORanks);	

void printCreateFile(Timing*, int, char*);

#endif
