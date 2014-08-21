#ifndef CREATEFILE_H_INCLUDED
#define CREATEFILE_H_INCLUDED
#include "benchmarkFunctions.h"
#include "mpi.h"

typedef struct {
	long long int* integersToWrite;
	long long int sizeAssignedToRank;
	long long int receiveCount;
	long long int otherExtraWork;
	int switchSubCommLength;
	int sizeOfNormalSubComm;
	int sizeOfLargeSubComm;
	int sizeOfSmallSubComm;	
	int* subCommArray;
	int* ioArray;
	int extraWork;
	int groupID;
	int rootOfGroup;
	} MpiInfo;

void createFile(InfoAboutFile fileInfo, long long int* integers, int , long long int, int, int numIORanks);	
void printCreateFile(Timing*, int, char*);
int setSizeOfComm(MpiInfo mpiInfo);
long long int setSizeAssignedToRank(long long int size, int numProc, MpiInfo mpiInfo, int rank);
void setWriteArray(MpiInfo* mpiInfo, int sizeOfComm, rootOrNot rootChoice);
void setIntegerArray(long long int size, long long int lowerBound, long long int* integers);
void setMpiInfo(MpiInfo* mpiInfo, int numProc, int numIORanks, int rank, long long int size);
void setIOArray(MpiInfo* mpiInfo, int numIORanks);
void setSubCommArray(MpiInfo* mpiInfo, int rank);
MPI_Offset setDisplacementForFileView(MpiInfo mpiInfo, int sizeOfComm, int rank);
void setGroupID(int rank, MpiInfo* mpiInfo);
void setRootOfGroup(int rank, MpiInfo* mpiInfo);

#endif
