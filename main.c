#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <getopt.h>
#include "createFile.h"
#include "verifyFile.h"
#include "benchmarkFunctions.h"

int main(int argc, char ** argv){

	int rank, numProc, numIORanks; 
	long long int lowerBound, upperBound;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	Choice whatToDoToFile = dontDoAnything;
	InfoAboutFile fileInfo;

	struct option long_options[] = {
		{"size", required_argument, 0, 's' },
        	{"create", required_argument, 0, 'c' },
		{"verify", required_argument, 0, 'v' },
		{"numIORanks", required_argument, 0, 'n'},
		{NULL,0,0,0}
	};

	int long_index = 0;
	int opt = 0;

	while(( opt = getopt_long(argc, argv,"s:c:v:n:",long_options, &long_index)) != -1){
		switch(opt){
			case 's' :
				
				strcpy(fileInfo.filesize,optarg);
                                fileInfo.SIZE = setSize(optarg);
				break;

			case 'n' : 
			
				numIORanks = atoi(optarg);
				break;

			case 'c' :	

				whatToDoToFile = create;
				strcpy(fileInfo.filename,optarg); 
				break;

			case 'v' : 

				whatToDoToFile = verify;	
				strcpy(fileInfo.filename,optarg); 
				break;

		}
	}

	setBoundsForRanks(rank, numProc, fileInfo.SIZE, &lowerBound, &upperBound);
	long long int sizeForRank = (upperBound - lowerBound) * 8;
	printf("Size in main.c: %lld\n", sizeForRank);
	long long int* integers = (long long int*) malloc(sizeForRank * sizeof(long long int));
	if(whatToDoToFile == create){
		createFile(fileInfo, integers, rank, lowerBound, numProc, numIORanks);

	}else if(whatToDoToFile == verify){
		verifyFile(fileInfo, integers, rank, lowerBound, numProc, numIORanks);

	}else{
		printf("See github.com/DanielWherry/striping-benchmark for usage information.\n");

	}

	MPI_Finalize();	
	return 0;

}
