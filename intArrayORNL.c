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

void setBoundsForRanks(int, int, int, int*, int*);
void createFile(char*, int, int*, int, int, int, int);
void verifyFile(char*, int*, int, int, int);
void printCreateFile(Timing* , int, int);
void printVerifyFile(Timing*, int);


int main(int argc, char ** argv){
	
	int rank, numProc, lowerBound, upperBound;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);


	int SIZE = 0, opt = 0;

	char filename[50]; 

	typedef enum {
		create,
		verify
	} Choice;

	Choice createOrVerify;

	struct option long_options[] = {
		{"numElements", required_argument, 0, 'n' },
        	{"create", required_argument, 0, 'c' },
		{"verify", required_argument, 0, 'v' },
		{NULL,0,0,0}
	};


	
	int long_index = 0;


	while(( opt = getopt_long(argc, argv,"n:c:v:",long_options, &long_index)) != -1){
		switch(opt){
			case 'n' : 
				
				SIZE = atoi(optarg);
				
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
		setBoundsForRanks(rank, numProc, SIZE, &lowerBound, &upperBound); 
		int* integers = (int*) malloc(((upperBound-lowerBound)+1) * sizeof(int));

		if(createOrVerify == create){
			
			createFile(filename, SIZE, integers, rank, lowerBound, upperBound, numProc);
			
		}else if(createOrVerify == verify){

			verifyFile(filename, integers, rank, lowerBound, upperBound);

		}else{
			free(integers);
			printf("You have made a mistake!! Did you forget an option?\n");
		}
	MPI_Finalize();	
	return 0;

}


void setBoundsForRanks(int rank, int numProc, int arraySize, int* lowerBound, int* upperBound){

	int baseAmount = arraySize / numProc;
	int extraWork = arraySize % numProc;
	int finalAmount;

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
void printCreateFile(Timing* t, int size, int rank){

	printf("\nCreated\n");
	printf("Time taken to open file for writing from rank %d: %f seconds.\n", rank ,t->open);
	printf("Time taken to create array with %d entries from rank %d: %f seconds.\n", size, rank, t->array);
	printf("Time taken to write entries of the array to the file from rank %d: %f seconds.\n", rank, t->readOrWrite);
	printf("Time taken to close file from rank %d: %f seconds.\n\n", rank, t->close);


}
//THIS FUNCTION PRINTS VERIFICATION TIMING INFORMATION
void printVerifyFile(Timing* t, int rank){
		
	printf("Time taken to open file for reading from rank %d: %f seconds.\n", rank, t->open);
	printf("Time taken to create array whose entries are the values in the file from rank %d: %f seconds.\n", rank, t->readOrWrite);
	printf("Time taken to verify entries in the file from rank %d: %f seconds.\n", rank, t->array);
	printf("Time taken to close file from rank %d: %f seconds.\n\n", rank, t->close);


}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(char filename[], int SIZE, int integers[], int rank, int lowerBound, int upperBound, int numProc){	
	double start, end;
	
	Timing timerOfProcesses;
	char str[20];
	sprintf(str, "%d.dat", rank);
	strcat(filename, str);
	
	start = MPI_Wtime();// Start timing
	FILE* outfile;
	outfile = fopen(filename,"w");
	end = MPI_Wtime();// End timing
	timerOfProcesses.open = end - start;

	start = MPI_Wtime();// Start Timing
	int i;
	for( i = lowerBound; i < upperBound; i++){
		integers[i] = i;
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;
	
	int* pointer = integers + lowerBound;		
	int sizeAssignedToEachRank;
	int extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}

	start = MPI_Wtime();// Start Timing
	fwrite(pointer, sizeof(int), sizeAssignedToEachRank, outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;
	
	free(integers);
				
	start = MPI_Wtime();//Start Timing
	fclose(outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;
	//Change sizeAssinged to Each
	
	printf("Lower Bound: %d, Upper Bound: %d, from rank: %d",lowerBound, upperBound, rank);	
	printCreateFile(&timerOfProcesses, sizeAssignedToEachRank, rank);
		


}
//THIS FUNCTION OPENS AN EXISTING FILE AND CHECKS THE DATA IN IT TO MAKE SURE THAT IT IS CORRECT
void verifyFile(char filename[], int integers[], int rank, int lowerBound, int upperBound){
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

	start = MPI_Wtime();//Start Timing
	fread(integers, sizeof(int), upperBound, infile);

	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;
			
	start = MPI_Wtime();// Start Timing
	int i;
	for( i = lowerBound; i < upperBound; i++){
		if(integers[i] != i){
			end = MPI_Wtime();// End Timing if files not same
			timerOfProcesses.array = end - start;
		
			free(integers);
	
			start = MPI_Wtime();// Start timing
			fclose(infile);
			end = MPI_Wtime();// End Timing
			timerOfProcesses.close = end - start;
			
			printf("\nThe files are not the same!!\n");	
			printVerifyFile(&timerOfProcesses, rank);	

			result = notsame;			

	 		break;
				
		}
	}
	if(result == same){
	end = MPI_Wtime();// End Timing if files same
	timerOfProcesses.array = end - start;
		
	free(integers);
			
	start = MPI_Wtime();// Start Timing
	fclose(infile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;

	printf("\nThe files are equivalent!!\n");
	printVerifyFile(&timerOfProcesses, rank);
	}	


}


	
