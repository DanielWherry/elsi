#include "createFile.h"
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "omp.h"

#define TestForError(err,errID) if(err){ MPI_Abort(MPI_COMM_WORLD, errID); }
#define Timer(function,timer) start = MPI_Wtime(); function; end = MPI_Wtime(); timer = end - start;

//THIS FUNCTION PRINTS CREATION TIMING INFORMATION
void printCreateFile(Timing* t, int rank, char* fileSize){

	printf( "{\"rank\": %d, \"Open Time\":%f, \"Generation Time\": %f, \"Write Time\": %f, \"Close Time\": %f,\"File Size\":\"%s\"}\n",rank, t->openTime, t->generateArrayTime, t->readOrWriteTime, t->closeTime, fileSize );

}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(InfoAboutFile fileInfo, long long int* integers, int rank, long long int lowerBound, int numProc, int numIORanks){
	
	double start, end;
	int err = 0, sizeOfComm;
	Timing timerOfProcesses;
	MpiInfo mpiInfo;
	rootOrNot rootChoice = notRoot;
	MPI_File outfile;
	MPI_Status status;
	MPI_Offset displacement;
	MPI_Comm subComm, ioComm;	
	MPI_Group worldGroup, subGroup, ioGroup;

	setMpiInfo(&mpiInfo, numProc, numIORanks, rank, fileInfo.SIZE);

	Timer(
		setIntegerArray(mpiInfo.sizeAssignedToRank, lowerBound, integers), 
		timerOfProcesses.generateArrayTime
	);

	if(rank == mpiInfo.rootOfGroup){
		rootChoice = root;
	}

	setIOArray(&mpiInfo, numIORanks); 
	setSubCommArray(&mpiInfo, rank);
	sizeOfComm = setSizeOfComm(mpiInfo);
	setWriteArray(&mpiInfo, sizeOfComm, rootChoice);		
	
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	
	TestForError(MPI_Group_incl(worldGroup, numIORanks, mpiInfo.ioArray, &ioGroup),0)
	TestForError(MPI_Comm_create(MPI_COMM_WORLD, ioGroup, &ioComm),1)	
	
	TestForError(MPI_Group_incl(worldGroup, sizeOfComm, mpiInfo.subCommArray, &subGroup),2)
	TestForError(MPI_Comm_create(MPI_COMM_WORLD, subGroup, &subComm),3)
	
	TestForError(MPI_Gather(integers, mpiInfo.sizeAssignedToRank, MPI_LONG_LONG_INT, mpiInfo.integersToWrite, mpiInfo.receiveCount, MPI_LONG_LONG_INT, 0, subComm),4)

	if(rootChoice == root){
		setFileName(&fileInfo);
		
		displacement = setDisplacementForFileView(rank, mpiInfo.sizeAssignedToRank);
		
		Timer(
			TestForError(MPI_File_open(ioComm, fileInfo.filename, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile),5),
			timerOfProcesses.openTime
		);
		
		MPI_File_set_view(outfile, displacement, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);
		
		Timer(
			TestForError(MPI_File_write(outfile, mpiInfo.integersToWrite, mpiInfo.sizeAssignedToRank, MPI_LONG_LONG_INT, &status),6),
			timerOfProcesses.readOrWriteTime
		);
	
		Timer(
			MPI_File_close(&outfile),
			timerOfProcesses.closeTime
		);
	
		printCreateFile(&timerOfProcesses, rank, fileInfo.filesize);
	}

	free(mpiInfo.integersToWrite);
	free(mpiInfo.subCommArray);
	free(mpiInfo.ioArray);
}

long long int setDisplacementForFileView(int rank, long long int size){
	return sizeof(long long int) * rank * size;
}

void setFileName(InfoAboutFile* fileInfo){
	char str[50];
	sprintf(str, ".dat");
	strcat(fileInfo->filename, str);

}
void setWriteArray(MpiInfo* mpiInfo, int sizeOfComm, rootOrNot rootChoice){
	if(rootChoice == root){
		mpiInfo->integersToWrite = (long long int*) malloc(sizeof(long long int) * mpiInfo->sizeAssignedToRank * sizeOfComm); 
		mpiInfo->receiveCount = mpiInfo->sizeAssignedToRank;
	}else{
		mpiInfo->integersToWrite = NULL;
		mpiInfo->receiveCount = 0;
	}
}
int setSizeOfComm(MpiInfo mpiInfo){
	if(mpiInfo.extraWork == 0){
		return mpiInfo.sizeOfNormalSubComm;
	}else if(mpiInfo.groupID < mpiInfo.switchSubCommLength){
		return mpiInfo.sizeOfLargeSubComm;
	}else if(mpiInfo.groupID >= mpiInfo.switchSubCommLength){
		return mpiInfo.sizeOfSmallSubComm;
}
	
}
//
long long int setSizeAssignedToRank(long long int size, int numProc, MpiInfo mpiInfo, int rank){
	if(rank < mpiInfo.extraWork){
		return  (size / numProc) + 1;
	}else{
		return size / numProc;
	}
}
//
void setIntegerArray(long long int size, long long int lowerBound, long long int* integers){
	long long int i;
	
	#pragma omp parallel for
		for( i = 0; i < size; i++){
			integers[i] = lowerBound + i;
		}
}
//
void setMpiInfo(MpiInfo* mpiInfo, int numProc, int numIORanks, int rank, long long int size){ 
	mpiInfo->sizeOfNormalSubComm = numProc / numIORanks;
	mpiInfo->sizeOfLargeSubComm = (numProc / numIORanks) + 1;
	mpiInfo->sizeOfSmallSubComm = mpiInfo->sizeOfNormalSubComm;
	mpiInfo->extraWork = numProc % numIORanks;	
	mpiInfo->sizeAssignedToRank = setSizeAssignedToRank(size, numProc, *mpiInfo, rank);	
	mpiInfo->switchSubCommLength = numProc + (numIORanks * (1 - (mpiInfo->sizeOfLargeSubComm)));
	mpiInfo->littleSize = 0;
	setGroupID(rank, mpiInfo);		
	setRootOfGroup(rank, mpiInfo);	
}
void setGroupID(int rank, MpiInfo* mpiInfo){
	if(mpiInfo->extraWork == 0){	
		mpiInfo->groupID = rank / mpiInfo->sizeOfNormalSubComm;
	}else{
		mpiInfo->groupID = rank / (mpiInfo->sizeOfLargeSubComm);
 	}
}		
void setRootOfGroup(int rank, MpiInfo* mpiInfo){
	if(mpiInfo->extraWork == 0){
		mpiInfo->rootOfGroup = mpiInfo->groupID * (mpiInfo->sizeOfNormalSubComm);
	}else if(mpiInfo->groupID < mpiInfo->switchSubCommLength){
		mpiInfo->rootOfGroup = mpiInfo->groupID * (mpiInfo->sizeOfLargeSubComm);
	}else if(mpiInfo->groupID >= mpiInfo->switchSubCommLength){
		int numLargeSubComms = mpiInfo->switchSubCommLength;
		int numRanksInLargeComms = mpiInfo->switchSubCommLength * mpiInfo->sizeOfLargeSubComm;
		int modifiedRank = rank - numRanksInLargeComms;
		int modifiedGroupID = modifiedRank / mpiInfo->sizeOfSmallSubComm;
		mpiInfo->groupID = modifiedGroupID + numLargeSubComms;
		mpiInfo->rootOfGroup = modifiedGroupID * mpiInfo->sizeOfSmallSubComm + numRanksInLargeComms;
	}
}
void setIOArray(MpiInfo* mpiInfo, int numIORanks){
	int i;
	mpiInfo->ioArray = (int*) malloc(sizeof(int) * numIORanks);

	if(mpiInfo->extraWork == 0){
		for(i = 0; i < numIORanks; i++){
			mpiInfo->ioArray[i] = i * mpiInfo->sizeOfNormalSubComm;
		}
	}else{
		for(i = 0; i < mpiInfo->switchSubCommLength; i++){
			mpiInfo->ioArray[i] = i * (mpiInfo->sizeOfLargeSubComm);
		}
		for(i = mpiInfo->switchSubCommLength; i < numIORanks; i++){
			int j = i - mpiInfo->switchSubCommLength;
			mpiInfo->ioArray[i] = i * (mpiInfo->sizeOfLargeSubComm) - j;
		}
	}
}


void setSubCommArray(MpiInfo* mpiInfo, int rank){
	int i;

	if(mpiInfo->extraWork == 0){
		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfNormalSubComm);

		for(i = 0; i < mpiInfo->sizeOfNormalSubComm; i++){
			mpiInfo->subCommArray[i] = mpiInfo->rootOfGroup + i;
		}
	}else if(mpiInfo->groupID < mpiInfo->switchSubCommLength){
		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfLargeSubComm);

		for(i = 0; i < mpiInfo->sizeOfLargeSubComm; i++){
			mpiInfo->subCommArray[i] = i + mpiInfo->rootOfGroup;
		}
	}else if(mpiInfo->groupID >= mpiInfo->switchSubCommLength){
		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfSmallSubComm);

		for(i = 0; i < mpiInfo->sizeOfSmallSubComm; i++){
			mpiInfo->subCommArray[i] = mpiInfo->rootOfGroup + i;
		}
	}
}













