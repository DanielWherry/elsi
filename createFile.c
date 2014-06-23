#include "createFile.h"
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//THIS FUNCTION PRINTS CREATION TIMING INFORMATION
void printCreateFile(Timing* t, int rank, char* fileSize){

	printf( "{\"rank\": %d, \"Open Time\":%f, \"Generation Time\": %f, \"Write Time\": %f, \"Close Time\": %f,\"File Size\":\"%s\"}\n",rank, t->open, t->array, t->readOrWrite, t->close, fileSize );

}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(InfoAboutFile fileInfo, long long int integers[], int rank, long long int lowerBound, int numProc, int numIORanks){
	
	int sizeOfSubComm = rank % numIORanks;	
	int* subComm;	

	if(rank % sizeOfSubComm == 0){  
		subComm = (int*) malloc(sizeof(int) * sizeOfSubComm);
	}

		

	MPI_Comm otherSubComm;	
	MPI_Group worldGroup, subGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	MPI_Group_incl(worldGroup, sizeOfSubComm, subComm, &subGroup); 
	MPI_Comm_create(MPI_COMM_WORLD, subGroup, &otherSubComm);
	
	double start, end;
	long long int i;
	int err=0;	
	Timing timerOfProcesses;
	
	long long int sizeAssignedToRank;
	long long int extraWork = fileInfo.SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToRank = (fileInfo.SIZE / numProc) + 1;
	}else{
		sizeAssignedToRank = fileInfo.SIZE / numProc;
	}

	char str[50];
	sprintf(str, ".dat");
	strcat(fileInfo.filename, str);

	MPI_File outfile;
	MPI_Status status;
	MPI_Request request[numProc];
	MPI_Offset disp =   sizeof(long long int) * rank * sizeAssignedToRank;

	if(rank == 0){
		MPI_File_delete(fileInfo.filename, MPI_INFO_NULL);

		start = MPI_Wtime();// Start timing
		err = MPI_File_open(MPI_COMM_WORLD, fileInfo.filename, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile);
		if(err){
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		end = MPI_Wtime();// End timing
		timerOfProcesses.open = end - start;

		for(i = 0; i < numProc; i++){
			MPI_Irecv(&integers, sizeAssignedToRank, MPI_LONG_LONG_INT, i+1, 0, MPI_COMM_WORLD, &request[i]);

			MPI_File_set_view(outfile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);

			start = MPI_Wtime();// Start Timing
			err = MPI_File_write(outfile, integers, sizeAssignedToRank, MPI_LONG_LONG_INT, &status);
			if(err){
				MPI_Abort(MPI_COMM_WORLD, 2);
			}
			end = MPI_Wtime();// End Timing
			timerOfProcesses.readOrWrite = end - start;
			MPI_Wait(&request[i], MPI_STATUS_IGNORE);
		}
	}

	else{
		start = MPI_Wtime();// Start Timing
		#pragma omp parallel for
			for( i = 0; i < sizeAssignedToRank; i++){
				integers[i] = lowerBound + i;
			}
		end = MPI_Wtime();// End Timing
		timerOfProcesses.array = end - start;
		MPI_Send(&integers, sizeAssignedToRank, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
	}

	start = MPI_Wtime();//Start Timing
	MPI_File_close(&outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;


	printCreateFile(&timerOfProcesses, rank, fileInfo.filesize);



}

