#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <getopt.h>
#include <omp.h>

int main(int argc, char ** argv){


	int rank, numProc, i;
	double start, end, time;

	int arraySize = 100000000;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	

	int * integers = (  int *) malloc(arraySize * 8);

	#pragma omp parallel for
	start = MPI_Wtime();// Start Timing
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
