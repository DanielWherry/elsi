#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


int main(int argc, char ** argv){
	
	MPI_Init(&argc, &argv);
	double startOpen, endOpen, startArr, endArr, startRoW, endRoW, startClose, endClose;// RoW = Read or Write
	int SIZE;

/*	if(strcmp(argv[3],"--numelements") == 0){
		const int SIZE = atoi(argv[4]);	
	}else{
		SIZE = 1000;
	}*/
	
	int* integers;
	integers = (int*)malloc(SIZE * sizeof(int));
	
	if(strcmp(argv[1],"--create") == 0){	
	
		startOpen = MPI_Wtime();// Start timing
		FILE* outfile;
		outfile = fopen(argv[2],"w");
		endOpen = MPI_Wtime();// End timing

		startArr = MPI_Wtime();// Start Timing
		for(int i = 0; i < SIZE; i++){
			integers[i] = i;
		}
		endArr = MPI_Wtime();// End Timing
		
		startRoW = MPI_Wtime();// Start Timing
		fwrite(integers, sizeof(int), SIZE ,outfile);
		endRoW = MPI_Wtime();// End Timing

		printf("\nCreated\n");
	
		free(integers);
		
		startClose = MPI_Wtime();//Start Timing
		fclose(outfile);
		endClose = MPI_Wtime();// End Timing


		printf("Time taken to open file for writing: %f seconds.\n",endOpen-startOpen);
		printf("Time taken to create array with %d entries: %f seconds.\n", SIZE, endArr-startArr);
		printf("Time taken to write entries of the array to the file: %f seconds.\n", endRoW-startRoW);
		printf("Time taken to close file: %f seconds.\n\n",endClose-startClose);
		

		return 0;
		
	}else if(strcmp(argv[1],"--verify") == 0){
		
		startOpen = MPI_Wtime();//Start Timing
		FILE *infile;	
		infile = fopen(argv[2],"r");
		endOpen = MPI_Wtime();// End Timing

		startRoW = MPI_Wtime();//Start Timing
		fread(integers, sizeof(int), SIZE, infile);
		endRoW = MPI_Wtime();// End Timing
		
		startArr = MPI_Wtime();// Start Timing
		for(int i = 0; i < SIZE; i++){
			if(integers[i] != i){
				endArr = MPI_Wtime();// End Timing if files not same
			
				printf("\nThe files are not the same!!\n");
				free(integers);

				startClose = MPI_Wtime();// Start timing
				fclose(infile);
				endClose = MPI_Wtime();// End Timing
				
				printf("Time taken to open file for reading: %f seconds.\n",endOpen-startOpen);
				printf("Time taken to create array whose entries are the values in the file: %f seconds.\n", endRoW-startRoW);
				printf("Time taken to verify entries in the file: %f seconds.\n", endArr-startArr);
				printf("Time taken to close file: %f seconds.\n\n",endClose-startClose);
		
				return 0;
				
			}
		}
		endArr = MPI_Wtime();// End Timing if files same
		
		printf("\nThe files are equivalent!\n");
		free(integers);
		
		startClose = MPI_Wtime();// Start Timing
		fclose(infile);
		endClose = MPI_Wtime();// End Timing

		printf("Time taken to open file for reading: %f seconds.\n",endOpen-startOpen);
		printf("Time taken to create array whose entries are the values in the file: %f seconds.\n", endRoW-startRoW);
		printf("Time taken to verify entries in the file: %f seconds.\n", endArr-startArr);
		printf("Time taken to close file: %f seconds.\n\n",endClose-startClose);



		return 0;

	}	
	
	return 0;
}
	
