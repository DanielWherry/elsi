#include "createFile.h"
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "omp.h"

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
	integers = setIntegerArray(mpiInfo.sizeAssignedToRank, lowerBound, integers);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;
	
	if(rank % mpiInfo.sizeOfSubComm == 0){
		rootChoice = root;
	}

	setSubCommArray(&mpiInfo, rank);
	
	
	err = MPI_Group_incl(worldGroup, mpiInfo.sizeOfSubComm, mpiInfo.subCommArray, &subGroup);
	if(err){
		MPI_Abort(MPI_COMM_WORLD,12);
	}
	err = MPI_Comm_create(MPI_COMM_WORLD, subGroup, &subComm);
	if(err){
		MPI_Abort(MPI_COMM_WORLD,11);
	}
		
	setIOArray(&mpiInfo, numIORanks); 
		
	err = MPI_Group_incl(worldGroup, numIORanks, mpiInfo.ioArray, &ioGroup);
	if(err){
		MPI_Abort(MPI_COMM_WORLD,15);
	}

	err = MPI_Comm_create(MPI_COMM_WORLD, ioGroup, &ioComm);
	if(err){
		MPI_Abort(MPI_COMM_WORLD,14);
	}
	
	if(rootChoice == root){
		mpiInfo.integersToWrite = (long long int*) malloc(sizeof(long long int) * mpiInfo.sizeAssignedToRank * mpiInfo.sizeOfSubComm); 
		mpiInfo.receiveCount = mpiInfo.sizeAssignedToRank;
	}	
	
	err = MPI_Gather(integers, mpiInfo.sizeAssignedToRank, MPI_LONG_LONG_INT, mpiInfo.integersToWrite, mpiInfo.receiveCount, MPI_LONG_LONG_INT, 0, subComm);	
	if(err){
		MPI_Abort(MPI_COMM_WORLD,10);
	}


	if(rootChoice == root){
		char str[50];
		sprintf(str, ".dat");
		strcat(fileInfo.filename, str);
	
		mpiInfo.littleSize = sizeof(mpiInfo.integersToWrite) / sizeof(mpiInfo.integersToWrite[0]);		

		MPI_Offset disp = sizeof(long long int) * rank * mpiInfo.littleSize;
		start = MPI_Wtime();// Start timing
		err = MPI_File_open(ioComm, fileInfo.filename, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile);
		if(err){
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		end = MPI_Wtime();// End timing
		timerOfProcesses.open = end - start;
	
		MPI_File_set_view(outfile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);
		
		start = MPI_Wtime();// Start Timing
		err = MPI_File_write(outfile, mpiInfo.integersToWrite, mpiInfo.littleSize, MPI_LONG_LONG_INT, &status);
		if(err){
			MPI_Abort(MPI_COMM_WORLD, 2);
		}
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
long long int setSizeAssignedToRank(int size, int numProc, MpiInfo mpiInfo, int rank){
	
	if(rank < mpiInfo.extraWork){
		return  (size / numProc) + 1;
	}else{
		return size / numProc;
	}
}
//
long long int* setIntegerArray(long long int size, long long int lowerBound, long long int* integers){
	long long int i;
	
	#pragma omp parallel for
		for( i = 0; i < size; i++){
			integers[i] = lowerBound + i;
		}
	return integers;
}
//
void setMpiInfo(MpiInfo* mpiInfo, int numProc, int numIORanks, int rank, int size){ 
	
	mpiInfo->sizeOfSubComm = numProc / numIORanks;
	mpiInfo->extraWork = numProc % numIORanks;	
		
	if(mpiInfo->extraWork == 0){	
		mpiInfo->rootRank = rank / mpiInfo->sizeOfSubComm;
		mpiInfo->tempRank = mpiInfo->rootRank * mpiInfo->sizeOfSubComm; 
	}else{
		mpiInfo->rootRank = rank / (mpiInfo->sizeOfSubComm + 1);
		mpiInfo->tempRank = mpiInfo->rootRank * (mpiInfo->sizeOfSubComm + 1);
 	}
	mpiInfo->sizeAssignedToRank = setSizeAssignedToRank(size, numProc, *mpiInfo, rank);	
	mpiInfo->offset = mpiInfo->rootRank - abs(mpiInfo->extraWork - mpiInfo->sizeOfSubComm);
	mpiInfo->switchSubCommLength = abs(mpiInfo->extraWork - mpiInfo->sizeOfSubComm);
	mpiInfo->tempIORanks = 0;
	mpiInfo->littleSize = 0;
	mpiInfo->receiveCount = 0; 

}
void setIOArray(MpiInfo* mpiInfo, int numIORanks){
	mpiInfo->ioArray = (int*) malloc(sizeof(int) * numIORanks);
	int i;

	if(mpiInfo->extraWork == 0){
		for(i = 0; i < numIORanks; i++){
			mpiInfo->ioArray[i] = mpiInfo->tempIORanks;
			mpiInfo->tempIORanks += mpiInfo->sizeOfSubComm;
		}
		
	}else{
		for(i = 0; i < mpiInfo->switchSubCommLength + 1; i++){
			mpiInfo->ioArray[i] = mpiInfo->tempIORanks;
			mpiInfo->tempIORanks += (mpiInfo->sizeOfSubComm);
			printf("ioArray: %d, rootRank: %d\n", mpiInfo->ioArray[i], mpiInfo->rootRank);
		}
		for(i = mpiInfo->switchSubCommLength + 1; i < numIORanks; i++){
			mpiInfo->ioArray[i] = mpiInfo->tempIORanks;
			mpiInfo->tempIORanks += (mpiInfo->sizeOfSubComm - 1);
			printf("ioArray: %d, rootRank: %d\n", mpiInfo->ioArray[i], mpiInfo->rootRank);
		}
	}
	
}
void setSubCommArray(MpiInfo* mpiInfo, int rank){
	int i;
	if(mpiInfo->extraWork == 0){
			
		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfSubComm);
		mpiInfo->rootRank = rank / (mpiInfo->sizeOfSubComm);
		mpiInfo->tempRank = mpiInfo->rootRank * (mpiInfo->sizeOfSubComm);

		for(i = 0; i < mpiInfo->sizeOfSubComm; i++){
			mpiInfo->subCommArray[i] = mpiInfo->tempRank;
			(mpiInfo->tempRank)++;
			//printf("subArray: %d, mpiInfo.rootRank: %d, mpiInfo.offset: %d\n", mpiInfo->subCommArray[i], mpiInfo->rootRank, mpiInfo->offset );
		}
	
	} else if(mpiInfo->rootRank < abs(mpiInfo->extraWork - mpiInfo->sizeOfSubComm)){
			
		(mpiInfo->sizeOfSubComm)++;
		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfSubComm);
		mpiInfo->rootRank = rank / (mpiInfo->sizeOfSubComm);
		mpiInfo->tempRank = mpiInfo->rootRank * (mpiInfo->sizeOfSubComm);

		for(i = 0; i < mpiInfo->sizeOfSubComm; i++){
			mpiInfo->subCommArray[i] = mpiInfo->tempRank;
			(mpiInfo->tempRank)++;
			//printf("subArrayLesser: %d, mpiInfo.rootRank: %d, mpiInfo.offset: %d\n", mpiInfo->subCommArray[i], mpiInfo->rootRank, mpiInfo->offset );
		}

	}else if(mpiInfo->rootRank >= abs(mpiInfo->extraWork - mpiInfo->sizeOfSubComm)){
		
		mpiInfo->subCommArray = (int*) malloc(sizeof(int) * mpiInfo->sizeOfSubComm);

		mpiInfo->rootRank = rank / (mpiInfo->sizeOfSubComm + 1);
		mpiInfo->tempRank = (mpiInfo->rootRank * (mpiInfo->sizeOfSubComm + 1)) - mpiInfo->offset;
		
		for(i = 0; i < mpiInfo->sizeOfSubComm; i++){
			mpiInfo->subCommArray[i] = mpiInfo->tempRank;
			(mpiInfo->tempRank)++;
			//printf("subArrayGreater: %d, mpiInfo.rootRank: %d, mpiInfo.offset: %d\n", mpiInfo->subCommArray[i], mpiInfo->rootRank, mpiInfo->offset );
		}
	}



}
