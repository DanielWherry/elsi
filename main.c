#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <getopt.h>

typedef struct {
	double open;
	double array;
	double readOrWrite;
	double close;
	} Timing;

void setBoundsForRanks(int, int, unsigned long long int, unsigned long long int*, unsigned long long int*);
void createFile(char*, unsigned long long int, unsigned long long int*, int , unsigned long long int, unsigned long long int, int, char*);	
void verifyFile(char*, unsigned long long int*, int, unsigned long long int, unsigned long long int, unsigned long long int, int, char*);
void printCreateFile(Timing*, int, char*);
void printVerifyFile(Timing*, int, char*);
unsigned long long int setSize(char*);

int main(int argc, char ** argv){

	int rank, numProc; 
	unsigned long long int lowerBound, upperBound;

	MPI_File outfile;	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	unsigned long long int SIZE = 0;
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
		unsigned long long int sizeForRank = upperBound - lowerBound + 1;
		unsigned long long int* integers = (unsigned long long int*) malloc(sizeForRank * sizeof(unsigned long long int));

		if(createOrVerify == create){

			createFile(filename, SIZE, integers, rank, lowerBound, upperBound, numProc, filesize);

		}else if(createOrVerify == verify){

			verifyFile(filename, integers, rank, lowerBound, upperBound, SIZE, numProc, filesize);

		}else{
			printf("You have made a mistake!! Did you forget an option?\n");
		}

	free(integers);
	MPI_Finalize();	
	return 0;

}

unsigned long long int setSize(char* commandLineArgument){
	
	unsigned long long int size, sizeOfArray;
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
		sizeOfArray = size / 8;
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

void setBoundsForRanks(int rank, int numProc, unsigned long long int arraySize, unsigned long long int* lowerBound, unsigned long long int* upperBound){

	unsigned long long int baseAmount = arraySize / numProc;
	unsigned long long int extraWork = arraySize % numProc;
	unsigned long long int finalAmount;

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
void createFile(char filename[], unsigned long long int SIZE, unsigned long long int integers[], int rank, unsigned long long int lowerBound, unsigned long long int upperBound, int numProc, char* fileSize){	
	double start, end;

	Timing timerOfProcesses;

	unsigned long long int Pstart = (SIZE /numProc) * rank;
	MPI_Offset offset = sizeof(unsigned long long int) * Pstart;
	MPI_File outfile;
	MPI_Status status;
	start = MPI_Wtime();// Start timing
	MPI_File_open(MPI_COMM_WORLD, filename,MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &outfile);
	end = MPI_Wtime();// End timing
	timerOfProcesses.open = end - start;

	unsigned long long int sizeAssignedToEachRank;
	unsigned long long int extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}

	start = MPI_Wtime();// Start Timing
	unsigned long long int i;
	for( i = 0; i < sizeAssignedToEachRank; i++){
		integers[i] = lowerBound + i;
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;

	start = MPI_Wtime();// Start Timing
	MPI_File_write(outfile, integers, SIZE/numProc, MPI_LONG_LONG_INT, &status);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;

	start = MPI_Wtime();//Start Timing
	MPI_File_close(&outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;

	printCreateFile(&timerOfProcesses, rank, fileSize);



}
//THIS FUNCTION OPENS AN EXISTING FILE AND CHECKS THE DATA IN IT TO MAKE SURE THAT IT IS CORRECT
void verifyFile(char filename[], unsigned long long int integers[], int rank, unsigned long long int lowerBound, unsigned long long int upperBound, unsigned long long int SIZE, int numProc, char* fileSize){
	double start, end;

	Timing timerOfProcesses;

	typedef enum{
		same,
		notsame
	} resultOfVerifyTest;

	resultOfVerifyTest result = same;
	char str[20];
	sprintf(str, "%d.dat", rank);
	strcat(filename, str);

	start = MPI_Wtime();//Start Timing
	FILE *infile;	
	infile = fopen(filename,"r");
	end = MPI_Wtime();// End Timing
	timerOfProcesses.open = end - start;

	unsigned long long int sizeAssignedToEachRank;
	unsigned long long int extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}


	start = MPI_Wtime();//Start Timing
	fread(integers, sizeof(unsigned long long int), sizeAssignedToEachRank, infile);

	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;

	start = MPI_Wtime();// Start Timing
	unsigned long long int i;
	for( i = 0; i < sizeAssignedToEachRank; i++){
		if(integers[i] != (lowerBound + i)){
			end = MPI_Wtime();// End Timing if files not same
			timerOfProcesses.array = end - start;

			start = MPI_Wtime();// Start timing
			fclose(infile);
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
	fclose(infile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;

	printVerifyFile(&timerOfProcesses, rank, fileSize);
	}	


}
