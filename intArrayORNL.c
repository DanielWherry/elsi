#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char ** argv){
	
	MPI_Init(&argc, &argv);
	double start, end, startVer, endVer;
	const int SIZE = 100;
	int* integers;
	integers = (int *)malloc(SIZE*sizeof(int));
	
	if(strcmp(argv[1],"--create") == 0){	
		start = MPI_Wtime();
		FILE *outfile;

		outfile = fopen(argv[2],"w");

		for(int i = 0; i < SIZE; i++){
			integers[i] = i;
		}
		fwrite(integers, sizeof(int), SIZE ,outfile);
		end = MPI_Wtime();
		printf("Created\n");
	
		free(integers);
		fclose(outfile);
		return 0;
		
	}else if(strcmp(argv[1],"--verify") == 0){
		start = MPI_Wtime();
		FILE *infile;
	
		infile = fopen(argv[2],"r");
		fread(integers, sizeof(int), SIZE, infile);
		end = MPI_Wtime();
		for(int i = 0; i < SIZE; i++){
			if(integers[i] != i){			
				printf("The files are not the same!!\n");
				free(integers);
				fclose(infile);
				return 0;
				
			}
		}
	
		
		printf("The files are equivalent!\n");
		free(integers);
		fclose(infile);
		return 0;

	}	
	
	return 0;
}
	
