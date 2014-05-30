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

void setBoundsForRanks(int, int, uint64_t, uint64_t*, uint64_t*);
void createFile(char*, uint64_t, uint64_t*, int , uint64_t, uint64_t, int);	
void verifyFile(char*, uint64_t*, int, uint64_t, uint64_t, uint64_t, int);
void printCreateFile(Timing* , uint64_t, int);
void printVerifyFile(Timing*, int);


int main(int argc, char ** argv){
	
	int rank, numProc; 
	uint64_t lowerBound, upperBound;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);


	uint64_t SIZE = 0;
	int opt = 0;

	char filename[50]; 

	typedef enum {
		create,
		verify,
		nothing
	} Choice;

	Choice createOrVerify = nothing;

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
		setBoundsForRanks(rank,numProc, SIZE, &lowerBound, &upperBound);
		uint64_t sizeForRank = upperBound - lowerBound + 1;
		uint64_t* integers = (uint64_t*) malloc(sizeForRank * sizeof(uint64_t));

		if(createOrVerify == create){
			
			createFile(filename, SIZE, integers, rank, lowerBound, upperBound, numProc);

		}else if(createOrVerify == verify){

			verifyFile(filename, integers, rank, lowerBound, upperBound, SIZE, numProc);
	
		}else{
			printf("You have made a mistake!! Did you forget an option?\n");
		}
		
	free(integers);
	MPI_Finalize();	
	return 0;

}


void setBoundsForRanks(int rank, int numProc, uint64_t arraySize, uint64_t* lowerBound, uint64_t* upperBound){

	uint64_t baseAmount = arraySize / numProc;
	uint64_t extraWork = arraySize % numProc;
	uint64_t finalAmount;

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
void printCreateFile(Timing* t, uint64_t size, int rank){

	printf("{\"rank\": %d, \"Open Time\":%f, \"Generation Time\": %f, \"Write Time\": %f, \"Close Time\": %f}\n",rank, t->open, t->array, t->readOrWrite, t->close);

}
//THIS FUNCTION PRINTS VERIFICATION TIMING INFORMATION
void printVerifyFile(Timing* t, int rank){
		
	printf("{\"rank\": %d, \"Open Time\":%f, \"Verify Time\": %f, \"Read Time\": %f, \"Close Time\": %f}\n",rank, t->open, t->array, t->readOrWrite, t->close);


}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(char filename[], uint64_t SIZE, uint64_t integers[], int rank, uint64_t lowerBound, uint64_t upperBound, int numProc){	
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
	
	uint64_t sizeAssignedToEachRank;
	uint64_t extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}

	start = MPI_Wtime();// Start Timing
	uint64_t i;
	for( i = 0; i < sizeAssignedToEachRank; i++){
		integers[i] = lowerBound + i;
	}
	end = MPI_Wtime();// End Timing
	timerOfProcesses.array = end - start;
		
	start = MPI_Wtime();// Start Timing
	fwrite(integers, sizeof(uint64_t), sizeAssignedToEachRank, outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;
				
	start = MPI_Wtime();//Start Timing
	fclose(outfile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;
	
	printCreateFile(&timerOfProcesses, sizeAssignedToEachRank, rank);
		


}
//THIS FUNCTION OPENS AN EXISTING FILE AND CHECKS THE DATA IN IT TO MAKE SURE THAT IT IS CORRECT
void verifyFile(char filename[], uint64_t integers[], int rank, uint64_t lowerBound, uint64_t upperBound, uint64_t SIZE, int numProc){
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

	uint64_t sizeAssignedToEachRank;
	uint64_t extraWork = SIZE % numProc;
	if(rank < extraWork){
		sizeAssignedToEachRank = (SIZE / numProc) + 1;
	}else{
		sizeAssignedToEachRank = SIZE / numProc;
	}


	start = MPI_Wtime();//Start Timing
	fread(integers, sizeof(uint64_t), sizeAssignedToEachRank, infile);

	end = MPI_Wtime();// End Timing
	timerOfProcesses.readOrWrite = end - start;
			
	start = MPI_Wtime();// Start Timing
	uint64_t i;
	for( i = 0; i < sizeAssignedToEachRank; i++){
		if(integers[i] != (lowerBound + i)){
			end = MPI_Wtime();// End Timing if files not same
			timerOfProcesses.array = end - start;
	
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
			
	start = MPI_Wtime();// Start Timing
	fclose(infile);
	end = MPI_Wtime();// End Timing
	timerOfProcesses.close = end - start;

	printVerifyFile(&timerOfProcesses, rank);
	}	


}


	
