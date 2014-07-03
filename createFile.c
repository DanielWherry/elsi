#include "createFile.h"
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//THIS FUNCTION PRINTS CREATION TIMING INFORMATION
void printCreateFile(Timing* t, int rank, char* fileSize){

	printf( "{\"rank\": %d, \"Open Time\":%f, \"Generation Time\": %f, \"Write Time\": %f, \"Close Time\": %f,\"File Size\":\"%s\"}\n",rank, t->open, t->array, t->readOrWrite, t->close, fileSize );

}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(InfoAboutFile fileInfo, long long int integers[], int rank, long long int lowerBound, int numProc, int numIORanks){
	
	double start, end;
	long long int i;
	int err=0;	
	Timing timerOfProcesses;
	rootOrNot rootChoice = notRoot;	


	long long int sizeAssignedToRank;
	long long int extraWork = fileInfo.SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToRank = (fileInfo.SIZE / numProc) + 1;
	}else{
		sizeAssignedToRank = fileInfo.SIZE / numProc;
	}


	start = MPI_Wtime();// Start Timing
	#pragma omp parallel for
		for( i = 0; i < sizeAssignedToRank; i++){
			integers[i] = lowerBound + i;
		}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;

	
	MPI_File outfile;
	MPI_Status status;
	MPI_Comm subComm, ioComm;	
	MPI_Group worldGroup, subGroup, ioGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	int sizeOfSubComm = numProc / numIORanks;	
	int* subCommArray;
	int ioArray[numIORanks];
	extraWork = numProc % numIORanks;
	int rootRank;
	int tempRank;
	int offset;
	if(extraWork == 0){	
		rootRank = rank / sizeOfSubComm;
		tempRank = rootRank * sizeOfSubComm; 
	}else{
		rootRank = rank / (sizeOfSubComm + 1);
		tempRank = rootRank * (sizeOfSubComm + 1);
 	}
	int tempIORanks = 0;
	
	if(rank % sizeOfSubComm == 0){
		rootChoice = root;
	}

	if(rootRank >= extraWork - sizeOfSubComm){
		subCommArray = (int*) malloc(sizeof(int) * sizeOfSubComm);
		offset = rootRank - (extraWork - sizeOfSubComm);

		if(rootRank > extraWork - sizeOfSubComm){
			rootRank = rank / sizeOfSubComm;
			tempRank = (rootRank * (sizeOfSubComm + 1)) - offset;
		}
		
		for(i = 0; i < sizeOfSubComm; i++){
			subCommArray[i] = tempRank;
			tempRank++;
			printf("CommArray: %d, rootRank: %d, offset: %d\n", subCommArray[i], rootRank, offset );
		}

		err = MPI_Group_incl(worldGroup, sizeOfSubComm, subCommArray, &subGroup);
		if(err){
			MPI_Abort(MPI_COMM_WORLD,12);
		}
		
		for(i = 0; i < numIORanks; i++){
			ioArray[i] = tempIORanks;
			tempIORanks += (sizeOfSubComm + 1);
		}
		
		err = MPI_Group_incl(worldGroup, numIORanks, ioArray, &ioGroup);
		if(err){
			MPI_Abort(MPI_COMM_WORLD,15);
		}
	
	}else if(rootRank < extraWork - sizeOfSubComm){
			
		subCommArray = (int*) malloc(sizeof(int) * sizeOfSubComm + 1);
		rootRank = rank / (sizeOfSubComm + 1);
		tempRank = rootRank * (sizeOfSubComm + 1);

		for(i = 0; i < sizeOfSubComm + 1; i++){
			subCommArray[i] = tempRank;
			tempRank++;
		}

		err = MPI_Group_incl(worldGroup,sizeOfSubComm + 1, subCommArray, &subGroup);
		if(err){
			MPI_Abort(MPI_COMM_WORLD,20);
		}
	
		for(i = 0; i < numIORanks; i++){
			ioArray[i] = tempIORanks;
			tempIORanks += sizeOfSubComm + 1;
		}
		
		err = MPI_Group_incl(worldGroup, sizeof(ioArray)/sizeof(ioArray[0]), ioArray, &ioGroup);
		if(err){
			MPI_Abort(MPI_COMM_WORLD,15);
		}
	
	}
	
	

            	
	err = MPI_Comm_create(MPI_COMM_WORLD, subGroup, &subComm);
	if(err){
		MPI_Abort(MPI_COMM_WORLD,11);
	}
	
	err = MPI_Comm_create(MPI_COMM_WORLD, ioGroup, &ioComm);
	if(err){
		MPI_Abort(MPI_COMM_WORLD,14);
	}
	
	long long int* integersToWrite = NULL;
	long long int receiveCount = 0;
	if(rootChoice == root){
		integersToWrite = (long long int*) malloc(sizeof(long long int) * sizeAssignedToRank * sizeOfSubComm); 
		receiveCount = sizeAssignedToRank;
	}	
	
	err = MPI_Gather(integers, sizeAssignedToRank, MPI_LONG_LONG_INT, integersToWrite, receiveCount, MPI_LONG_LONG_INT, 0, subComm);	
	if(err){
		MPI_Abort(MPI_COMM_WORLD,10);
	}


	if(rootChoice == root){
		char str[50];
		sprintf(str, ".dat");
		strcat(fileInfo.filename, str);
	
		long long int littleSize = sizeof(integersToWrite) / sizeof(integersToWrite[0]);		

		MPI_Offset disp = sizeof(long long int) * rank * littleSize;
		start = MPI_Wtime();// Start timing
		err = MPI_File_open(ioComm, fileInfo.filename, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile);
		if(err){
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		end = MPI_Wtime();// End timing
		timerOfProcesses.open = end - start;
	
		MPI_File_set_view(outfile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);
		
		start = MPI_Wtime();// Start Timing
		err = MPI_File_write(outfile, integersToWrite, littleSize, MPI_LONG_LONG_INT, &status);
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
	


	free(integersToWrite);
	free(subCommArray);


}

