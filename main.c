#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <getopt.h>
#include <omp.h>

typedef struct {
	double open;
	double array;
	double readOrWrite;
	double close;
	} Timing;

void setBoundsForRanks(int, int, long long int, long long int*, long long int*);
void createFile(char*, long long int, long long int*, int , long long int, long long int, int, char*);	
void verifyFile(char*, long long int*, int, long long int, long long int, long long int, int, char*);
void printCreateFile(Timing*, int, char*);
void printVerifyFile(Timing*, int, char*);
long long int setSize(char*);

int main(int argc, char ** argv){

	int rank, numProc; 
	long long int lowerBound, upperBound;

	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	long long int SIZE = 0;
	int opt = 0;

	char filename[50];
	char filesize[50]; 

	typedef enum {
		create,
		verify,
		nothing
	} Choice;

	Choice createOrVerify = nothing;

	struct option long_options[] = {
		{"size", required_argument, 0, 's' },
        	{"create", required_argument, 0, 'c' },
		{"verify", required_argument, 0, 'v' },
		{NULL,0,0,0}
	};



	int long_index = 0;


	while(( opt = getopt_long(argc, argv,"s:c:v:",long_options, &long_index)) != -1){
		switch(opt){
			case 's' :
				
				strcpy(filesize,optarg);
                                SIZE = setSize(optarg);
				break;

			case 'c' :	

				createOrVerify = create;
				strcpy(filename,optarg); 

				break;

			case 'v' : 

				createOrVerify = verify;					
				strcpy(filename,optarg); 

				break;

		}


	}
		setBoundsForRanks(rank,numProc, SIZE, &lowerBound, &upperBound);
		long long int sizeForRank = upperBound - lowerBound + 1;
		long long int* integers = (long long int*) malloc(sizeForRank * sizeof(long long int));

		if(createOrVerify == create){

			createFile(filename, SIZE, integers, rank, lowerBound, upperBound, numProc, filesize);

		}else if(createOrVerify == verify){

			verifyFile(filename, integers, rank, lowerBound, upperBound, SIZE, numProc, filesize);

		}else{
			printf("You have made a mistake!! Did you forget an option?\n");
		}

	MPI_Finalize();	
	free(integers);
	return 0;

}

long long int setSize(char* commandLineArgument){
	
	long long int size, sizeOfArray;
	char* sizeInString;
	if(strchr(commandLineArgument, 'K') != NULL){
		sizeInString = strtok(commandLineArgument, "K");
		size = atoi(sizeInString);
		size *= 1024;// Right hand side is #bytes in Kilobyte
		sizeOfArray = size / 8;	
	}
	else if(strchr(commandLineArgument, 'M') != NULL){
		sizeInString = strtok(commandLineArgument, "M");
		size = atoi(sizeInString);
		size *= 1048576;// Right hand side is #bytes in Megabyte
		sizeOfArray = size / 8; //////
	}
	else if(strchr(commandLineArgument, 'G') != NULL){
		sizeInString = strtok(commandLineArgument, "G");
		size = atoi(sizeInString);
		size *= 1073741824;// Right hand side is #bytes in Gigabyte
		sizeOfArray = size / 8;
	}
	else if(strchr(commandLineArgument, 'T') != NULL){
		sizeInString = strtok(commandLineArgument, "T");
		size = atoi(sizeInString);
		size *= 1099511627776; // Right hand side is #bytes in Terabyte
		sizeOfArray = size / 8;
	}
	else if(strchr(commandLineArgument, 'B') != NULL){
		sizeInString = strtok(commandLineArgument, "B");
		size = atoi(sizeInString);
		sizeOfArray = size / 8;
	}


	return sizeOfArray;
}

void setBoundsForRanks(int rank, int numProc, long long int arraySize, long long int* lowerBound, long long int* upperBound){

	long long int baseAmount = arraySize / numProc;
	long long int extraWork = arraySize % numProc;
	long long int finalAmount;

	if(rank < extraWork){
		finalAmount = baseAmount + 1;
		*lowerBound = finalAmount * rank;
		*upperBound = *lowerBound + baseAmount;
	}else{
		finalAmount = baseAmount;
		*lowerBound = finalAmount * rank + extraWork;
		*upperBound = *lowerBound + (baseAmount - 1);
	}


}


//THIS FUNCTION PRINTS CREATION TIMING INFORMATION
void printCreateFile(Timing* t, int rank, char* fileSize){


	printf( "{\"rank\": %d, \"Open Time\":%f, \"Generation Time\": %f, \"Write Time\": %f, \"Close Time\": %f,\"File Size\":\"%s\"}\n",rank, t->open, t->array, t->readOrWrite, t->close, fileSize );

}
//THIS FUNCTION PRINTS VERIFICATION TIMING INFORMATION
void printVerifyFile(Timing* t, int rank, char* fileSize){
	

	printf( "{\"rank\": %d, \"Open Time\":%f, \"Verify Time\": %f, \"Read Time\": %f, \"Close Time\": %f, \"File Size\": \"%s\"}\n",rank, t->open, t->array, t->readOrWrite, t->close, fileSize);

}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(char filename[], long long int SIZE, long long int integers[], int rank, long long int lowerBound, long long int upperBound, int numProc, char* fileSize){	
	double start, end;
	Timing timerOfProcesses;
	int err=0;	

	long long int sizeAssignedToEachRank;
	long long int extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}
	
	char str[20];
	sprintf(str, ".dat");
	strcat(filename, str);
	
	MPI_File outfile;
	MPI_Status status;
	MPI_Request request;
	MPI_Offset disp =  2 * sizeof(MPI_LONG_LONG_INT) * rank * sizeAssignedToEachRank;
	MPI_File_delete(filename, MPI_INFO_NULL);

	start = MPI_Wtime();// Start timing
	err = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile);
	if(err){
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	end = MPI_Wtime();// End timing
	timerOfProcesses.open = end - start;

	omp_set_num_threads(16);	
	start = MPI_Wtime();// Start Timing
	long long int i;
	#pragma omp parallel for 
		for( i = 0; i < sizeAssignedToEachRank; i++){
			integers[i] = lowerBound + i;
		}
	
	
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;
	
	//MPI_File_seek(outfile, disp, MPI_SEEK_CUR);
	MPI_File_set_view(outfile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);

	start = MPI_Wtime();// Start Timing
	err = MPI_File_write(outfile, integers, sizeAssignedToEachRank, MPI_LONG_LONG_INT, &status);
	if(err){
		MPI_Abort(MPI_COMM_WORLD, 2);
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;
	
	start = MPI_Wtime();//Start Timing
	MPI_File_close(&outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;


	printCreateFile(&timerOfProcesses, rank, fileSize);



}
//THIS FUNCTION OPENS AN EXISTING FILE AND CHECKS THE DATA IN IT TO MAKE SURE THAT IT IS CORRECT
void verifyFile(char filename[], long long int integers[], int rank, long long int lowerBound, long long int upperBound, long long int SIZE, int numProc, char* fileSize){
	double start, end;
	int err = 0;
	Timing timerOfProcesses;
	
	long long int sizeAssignedToEachRank;
	long long int extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}

	typedef enum{
		same,
		notsame
	} resultOfVerifyTest;

	resultOfVerifyTest result = same;
	
	char str[20];
	sprintf(str, ".dat");
	strcat(filename, str);

	MPI_File infile;
	MPI_Status status;
	MPI_Offset disp = 2 * sizeof(MPI_LONG_LONG_INT) * sizeAssignedToEachRank * rank;

	
	start = MPI_Wtime();//Start Timing
	err = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &infile);
	if(err){
		MPI_Abort(MPI_COMM_WORLD, 4);
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.open = end - start;
	
	 MPI_File_set_view( infile, disp, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL );

	start = MPI_Wtime();//Start Timing
	err = MPI_File_read(infile, integers, sizeAssignedToEachRank, MPI_LONG_LONG_INT, &status);
	if(err){
		MPI_Abort(MPI_COMM_WORLD, 5);
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;

	start = MPI_Wtime();// Start Timing
	long long int i;
	for( i = 0; i < sizeAssignedToEachRank; i++){
		if(integers[i] != (lowerBound + i)){
			end = MPI_Wtime();// End Timing if files not same
			timerOfProcesses.array = end - start;

			start = MPI_Wtime();// Start timing
			MPI_File_close(&infile);
			end = MPI_Wtime();// End Timing
			timerOfProcesses.close = end - start;

			printf("\nThe files are not the same!!\n");	
			printVerifyFile(&timerOfProcesses, rank, fileSize);	

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

	printVerifyFile(&timerOfProcesses, rank, fileSize);
	}	


}
