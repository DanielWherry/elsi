#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <getopt.h>

void createFile(char*, int, int*);
void verifyFile(char*, int, int* );
void printCreateFile(double , double , double , double , double , double , double , double , int);
void printVerifyFile(double , double , double , double , double , double , double , double );

int rank, numProc;

int main(int argc, char ** argv){
	
	
	
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
		int* integers = (int*) malloc(SIZE * sizeof(int));

		if(createOrVerify == create){
			
			createFile(filename, SIZE, integers);

			
		}else if(createOrVerify == verify){

			verifyFile(filename, SIZE, integers);

		}else{
			printf("You have made a mistake!! Did you forget an option?\n");
		}
	MPI_Finalize();	
	return 0;

}


//THIS FUNCTION PRINTS CREATION TIMING INFORMATION
void printCreateFile(double endOpen, double startOpen, double endRoW, double startRoW, double endArr, double startArr, double endClose, double startClose, int size){

	printf("\nCreated\n");
	printf("Hello from process %d of %d\n", rank, numProc);
	printf("Time taken to open file for writing: %f seconds.\n",endOpen-startOpen);
	printf("Time taken to create array with %d entries: %f seconds.\n", size, endArr-startArr);
	printf("Time taken to write entries of the array to the file: %f seconds.\n", endRoW-startRoW);
	printf("Time taken to close file: %f seconds.\n\n",endClose-startClose);


}
//THIS FUNCTION PRINTS VERIFICATION TIMING INFORMATION
void printVerifyFile(double endOpen, double startOpen, double endRoW, double startRoW, double endArr, double startArr, double endClose, double startClose){
		
	printf("Hello from process %d of %d\n", rank, numProc);
	printf("Time taken to open file for reading: %f seconds.\n",endOpen-startOpen);
	printf("Time taken to create array whose entries are the values in the file: %f seconds.\n", endRoW-startRoW);
	printf("Time taken to verify entries in the file: %f seconds.\n", endArr-startArr);
	printf("Time taken to close file: %f seconds.\n\n",endClose-startClose);


}
//THIS FUNCTION CREATES A FILE WITH INFORMATION DETERMINED BY THE USER AT THE COMMAND LINE 
void createFile(char filename[], int SIZE, int integers[]){	
	double startOpen, endOpen, startArr, endArr, startRoW, endRoW, startClose, endClose;// RoW = Read or Write

	startOpen = MPI_Wtime();// Start timing
	FILE* outfile;
	outfile = fopen(filename,"w");
	endOpen = MPI_Wtime();// End timing

	startArr = MPI_Wtime();// Start Timing
	int i;
	for( i = 0; i < SIZE; i++){
		integers[i] = i;
	}
	endArr = MPI_Wtime();// End Timing
			
	startRoW = MPI_Wtime();// Start Timing
	fwrite(integers, sizeof(int), SIZE, outfile);
	endRoW = MPI_Wtime();// End Timing

	
	free(integers);
				
	startClose = MPI_Wtime();//Start Timing
	fclose(outfile);
	endClose = MPI_Wtime();// End Timing
	
	
	printCreateFile(endOpen, startOpen, endRoW, startRoW, endArr, startArr, endClose, startClose, SIZE);	


}
//THIS FUNCTION OPENS AN EXISTING FILE AND CHECKS THE DATA IN IT TO MAKE SURE THAT IT IS CORRECT
void verifyFile(char filename[], int SIZE, int integers[]){
	double startOpen, endOpen, startArr, endArr, startRoW, endRoW, startClose, endClose;// RoW = Read or Write
	
	typedef enum{
		same,
		notsame
	} resultOfVerifyTest;

	resultOfVerifyTest result = same;

	startOpen = MPI_Wtime();//Start Timing
	FILE *infile;	
	infile = fopen(filename,"r");
	endOpen = MPI_Wtime();// End Timing

	startRoW = MPI_Wtime();//Start Timing
	fread(integers, sizeof(int), SIZE, infile);
	endRoW = MPI_Wtime();// End Timing
				
	startArr = MPI_Wtime();// Start Timing
	int i;
	for( i = 0; i < SIZE; i++){
		if(integers[i] != i){
			endArr = MPI_Wtime();// End Timing if files not same
				
			free(integers);
	
			startClose = MPI_Wtime();// Start timing
			fclose(infile);
			endClose = MPI_Wtime();// End Timing
						
			printf("\nThe files are not the same!!\n");	
			printVerifyFile(endOpen, startOpen, endRoW, startRoW, endArr, startArr, endClose, startClose);	

			result = notsame;			

	 		break;
				
		}
	}
	if(result == same){
	endArr = MPI_Wtime();// End Timing if files same
			
	free(integers);
			
	startClose = MPI_Wtime();// Start Timing
	fclose(infile);
	endClose = MPI_Wtime();// End Timing
	
	printf("\nThe files are equivalent!!\n");
	printVerifyFile(endOpen, startOpen, endRoW, startRoW, endArr, startArr, endClose, startClose);
	}	


}


	
