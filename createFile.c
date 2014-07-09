#include "createFile.h"
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "omp.h"

#define TestForError(err,errID) if(err){ MPI_Abort(MPI_COMM_WORLD, errID); }

//THIS FUNCTION PRINTS CREATION TIMING INFORMATION
void printCreateFile(Timing* t, int rank, char* fileSize){

	printf( "{\"rank\": %d, \"Open Time\":%f, \"Generation Time\": %f, \"Write Time\": %f, \"Close Time\": %f,\"File Size\":\"%s\"}\n",rank, t->open, t->array, t->readOrWrite, t->close, fileSize );

}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 

void createFile(InfoAboutFile fileInfo, long long int* integers, int rank, long long int lowerBound, int numProc, int numIORanks){
	
	double start, end;
	long long int i;
	int err=0;	
	Timing timerOfProcesses;
	MpiInfo mpiInfo;
	rootOrNot rootChoice = notRoot;

	setMpiInfo(&mpiInfo, numProc, numIORanks, rank, fileInfo.SIZE);
	
	MPI_File outfile;
	MPI_Status status;
	MPI_Comm subComm, ioComm;	
	MPI_Group worldGroup, subGroup, ioGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

	start = MPI_Wtime();// Start Timing
	setIntegerArray(mpiInfo.sizeAssignedToRank, lowerBound, integers);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;
	
	if(rank % mpiInfo.sizeOfNormalSubComm == 0){
		rootChoice = root;
	}

	setIOArray(&mpiInfo, numIORanks); 
	
	//printf("rank: %d, third\n", rank );	
	err = MPI_Group_incl(worldGroup, numIORanks, mpiInfo.ioArray, &ioGroup);
	TestForError(err,0);
	
	//printf("rank: %d, 4th\n", rank );

	err = MPI_Comm_create(MPI_COMM_WORLD, ioGroup, &ioComm);
	TestForError(err,1);
	
	setSubCommArray(&mpiInfo, rank);
	
	//printf("rank: %d, first\n", rank );
	err = MPI_Group_incl(worldGroup, mpiInfo.sizeOfNormalSubComm, mpiInfo.subCommArray, &subGroup);
	TestForError(err,2);

	//printf("rank: %d, second\n", rank );
	err = MPI_Comm_create(MPI_COMM_WORLD, subGroup, &subComm);
	TestForError(err,3);
	
	if(rootChoice == root){
		mpiInfo.integersToWrite = (long long int*) malloc(sizeof(long long int) * mpiInfo.sizeAssignedToRank); 
		mpiInfo.receiveCount = mpiInfo.sizeAssignedToRank;
	}	
	
	err = MPI_Gather(integers, mpiInfo.sizeAssignedToRank, MPI_LONG_LONG_INT, mpiInfo.integersToWrite, mpiInfo.receiveCount, MPI_LONG_LONG_INT, 0, subComm);	
	TestForError(err,4);


	if(rootChoice == root){
		char str[50];
		sprintf(str, ".dat");
		strcat(fileInfo.filename, str);
	
		mpiInfo.littleSize = sizeof(mpiInfo.integersToWrite) / sizeof(mpiInfo.integersToWrite[0]);		

		MPI_Offset disp = sizeof(long long int) * rank * mpiInfo.sizeAssignedToRank;
		start = MPI_Wtime();// Start timing
		err = MPI_File_open(ioComm, fileInfo.filename, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile);
		TestForError(err,5);
		end = MPI_Wtime();// End timing
		timerOfProcesses.open = end - start;
	
		MPI_File_set_view(outfile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);
		
		start = MPI_Wtime();// Start Timing
		err = MPI_File_write(outfile, mpiInfo.integersToWrite, mpiInfo.littleSize, MPI_LONG_LONG_INT, &status);
		TestForError(err,6);
		end = MPI_Wtime();// End Timing
		timerOfProcesses.readOrWrite = end - start;
	
		start = MPI_Wtime();//Start Timing
		MPI_File_close(&outfile);
		end = MPI_Wtime();// End Timing

		timerOfProcesses.close = end - start;
	
		printCreateFile(&timerOfProcesses, rank, fileInfo.filesize);
		
	}
	


	free(mpiInfo.integersToWrite);
	free(mpiInfo.subCommArray);
	free(mpiInfo.ioArray);


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
		
	if(mpiInfo->extraWork == 0){	
		mpiInfo->groupID = rank / mpiInfo->sizeOfNormalSubComm;
	}else{
		mpiInfo->groupID = rank / (mpiInfo->sizeOfLargeSubComm);
 	}
	mpiInfo->sizeAssignedToRank = setSizeAssignedToRank(size, numProc, *mpiInfo, rank);	
	mpiInfo->switchSubCommLength = numProc + (numIORanks * (1 - (mpiInfo->sizeOfLargeSubComm)));
	mpiInfo->littleSize = 0;
	mpiInfo->receiveCount = 0; 

}
void setIOArray(MpiInfo* mpiInfo, int numIORanks){
	mpiInfo->ioArray = (int*) malloc(sizeof(int) * numIORanks);
	int i, tempIORanks = 0;

	if(mpiInfo->extraWork == 0){
		for(i = 0; i < numIORanks; i++){
			mpiInfo->ioArray[i] = tempIORanks;
			tempIORanks += mpiInfo->sizeOfNormalSubComm;
		}

	}else{
		for(i = 0; i < mpiInfo->switchSubCommLength; i++){
			mpiInfo->ioArray[i] = i * (mpiInfo->sizeOfLargeSubComm);
			//printf("ioArray: %d, groupID: %d, size: %d, iterator: %d, switch: %d\n", mpiInfo->ioArray[i], mpiInfo->groupID, mpiInfo->sizeOfSubComm + 1, i, mpiInfo->switchSubCommLength);
		}

		for(i = mpiInfo->switchSubCommLength; i < numIORanks; i++){
			int j = i - mpiInfo->switchSubCommLength;
			mpiInfo->ioArray[i] = i * (mpiInfo->sizeOfLargeSubComm);
			//printf("ioArray: %d, groupID: %d, j: %d\n", mpiInfo->ioArray[i], mpiInfo->groupID, j);
		}
	}

}


void setSubCommArray(MpiInfo* mpiInfo, int rank){
	int i;
	if(mpiInfo->extraWork == 0){

		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfNormalSubComm);
		int rootOfGroup = mpiInfo->groupID * (mpiInfo->sizeOfNormalSubComm);

		for(i = 0; i < mpiInfo->sizeOfNormalSubComm; i++){
			mpiInfo->subCommArray[i] = rootOfGroup + i;
			//printf("subArray: %d, mpiInfo.groupID: %d, mpiInfo.offset: %d\n", mpiInfo->subCommArray[i], mpiInfo->groupID, mpiInfo->offset );
		}

	}else if(mpiInfo->groupID < mpiInfo->switchSubCommLength){

		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfLargeSubComm);
		int rootOfGroup = mpiInfo->groupID * (mpiInfo->sizeOfLargeSubComm);

		for(i = 0; i < mpiInfo->sizeOfLargeSubComm; i++){
			mpiInfo->subCommArray[i] = i + rootOfGroup;
			printf("subArrayGreater: %d, mpiInfo.groupID: %d, mpiInfo.switchSubCommLength: %d, rank: %d\n", mpiInfo->subCommArray[i], mpiInfo->groupID, mpiInfo->switchSubCommLength, rank);
		}

	}else if(mpiInfo->groupID >= mpiInfo->switchSubCommLength){

		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfSmallSubComm);
		int offset = mpiInfo->groupID - mpiInfo->sizeOfSmallSubComm;
		int rootOfGroup = (mpiInfo->groupID * (mpiInfo->sizeOfLargeSubComm)) - offset;

		for(i = 0; i < mpiInfo->sizeOfSmallSubComm; i++){
			mpiInfo->subCommArray[i] = rootOfGroup + i;
			printf("subArrayLesser: %d, mpiInfo.groupID: %d, mpiInfo.switch: %d, rank: %d, offset: %d\n", mpiInfo->subCommArray[i], mpiInfo->groupID, mpiInfo->switchSubCommLength, rank, offset);
		}
		(mpiInfo->sizeOfNormalSubComm)--;
	}



}













