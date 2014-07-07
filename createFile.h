#ifndef CREATEFILE_H_INCLUDED
#define CREATEFILE_H_INCLUDED
#include "benchmarkFunctions.h"

typedef struct {
	long long int* integersToWrite;
	long long int sizeAssignedToRank;
	long long int receiveCount;
	long long int littleSize;
	int switchSubCommLength;
	int sizeOfSubComm;	
	int* subCommArray;
	int* ioArray;
	int tempIORanks;
	int extraWork;
	int rootRank;
	int tempRank;
	int offset;
	} MpiInfo;

void createFile(InfoAboutFile fileInfo, long long int* , int , long long int, int, int numIORanks);	
void printCreateFile(Timing*, int, char*);
long long int setSizeAssignedToRank(int size, int numProc, MpiInfo mpiInfo, int rank);
long long int* setIntegerArray(long long int size, long long int lowerBound, long long int* integers);
void setMpiInfo(MpiInfo* mpiInfo, int numProc, int numIORanks, int rank, int size);
void setIOArray(MpiInfo* mpiInfo, int numIORanks);
void setSubCommArray(MpiInfo* mpiInfo, int rank);

#endif
