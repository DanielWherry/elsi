#include "verifyFile.h"
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//THIS FUNCTION PRINTS VERIFICATION TIMING INFORMATION
void printVerifyFile(Timing* t, int rank, char* fileSize){

	printf( "{\"rank\": %d, \"Open Time\":%f, \"Verify Time\": %f, \"Read Time\": %f, \"Close Time\": %f, \"File Size\": \"%s\"}\n",rank, t->open, t->array, t->readOrWrite, t->close, fileSize);

}
//THIS FUNCTION OPENS AN EXISTING FILE AND CHECKS THE DATA IN IT TO MAKE SURE THAT IT IS CORRECT
void verifyFile(InfoAboutFile fileInfo, long long int integers[], int rank, long long int lowerBound, int numProc, int numIORanks){

	double start, end;
	int err = 0;
	Timing timerOfProcesses;

	long long int sizeAssignedToRank;
	long long int extraWork = fileInfo.SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToRank = (fileInfo.SIZE / numProc) + 1;
	}else{
		sizeAssignedToRank = fileInfo.SIZE / numProc;
	}

	resultOfVerifyTest result = same;

	char str[20];
	sprintf(str, ".dat");
	strcat(fileInfo.filename, str);

	MPI_File infile;
	MPI_Status status;
	MPI_Offset disp =  sizeof(long long int) * sizeAssignedToRank * rank;


	start = MPI_Wtime();//Start Timing
	err = MPI_File_open(MPI_COMM_WORLD, fileInfo.filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &infile);
	if(err){
		MPI_Abort(MPI_COMM_WORLD, 4);
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.open = end - start;

	MPI_File_set_view( infile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL );

	start = MPI_Wtime();//Start Timing
	err = MPI_File_read(infile, integers, sizeAssignedToRank, MPI_LONG_LONG_INT, &status);
	if(err){
		MPI_Abort(MPI_COMM_WORLD, 5);
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;

	start = MPI_Wtime();// Start Timing
	long long int i;
	for( i = 0; i < sizeAssignedToRank; i++){
		if(integers[i] != (lowerBound + i)){
			end = MPI_Wtime();// End Timing if files not same
			timerOfProcesses.array = end - start;

			start = MPI_Wtime();// Start timing
			MPI_File_close(&infile);
			end = MPI_Wtime();// End Timing
			timerOfProcesses.close = end - start;

			printf("\nThe files are not the same!!\n");	
			printVerifyFile(&timerOfProcesses, rank, fileInfo.filesize);	

			result = notsame;			

	 		break;

		}
	}
	if(result == same){
	end = MPI_Wtime();// End Timing if files same
	timerOfProcesses.array = end - start;

	start = MPI_Wtime();// Start Timing
	MPI_File_close(&infile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;

	printVerifyFile(&timerOfProcesses, rank, fileInfo.filesize);
	}	


}
