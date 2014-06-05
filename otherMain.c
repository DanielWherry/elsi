#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <getopt.h>


int main(int argc, char ** argv){


	int rank, numProc;
	double start, end, time;

	unsigned long long int arraySize = 100000000;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	

	unsigned long long int* integers = (unsigned long long int*) malloc(arraySize * 8);

	start = MPI_Wtime();// Start Timing
        unsigned long long int i;
        for( i = 0; i < arraySize; i++){
                integers[i] = i;
        }
        end = MPI_Wtime();// End Timing
        time = end - start;

	printf( "{\"rank\": %d, \"Generate Time\":%f}\n", rank, time);
	free(integers);	
	MPI_Finalize();
	return 0;

}
