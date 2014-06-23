#ifndef VERIFYFILE_H_INCLUDED
#define VERIFYFILE_H_INCLUDED
#include "benchmarkFunctions.h"

typedef enum{
	same,
	notsame
	} resultOfVerifyTest;

void verifyFile(InfoAboutFile fileInfo, long long int*, int, long long int, int, int numIORanks);

void printVerifyFile(Timing*, int, char*);


#endif
