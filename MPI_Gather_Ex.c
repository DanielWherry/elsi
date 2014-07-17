#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

	MPI_Init(&argc, &argv);
	int rank, numProc;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	
	// How much memory that can be allocated by each rank due to the number of ranks per node.
	// I will make these numbers smaller to make sure program doesn't crash
	//
	//
	//
	// 1 rankPerNode long long int size = 26843545600; // 25 GB	
	// 4 rankPerNode long long int size = 7516192768; // 7 GB
	// 8 rankPerNode long long int size = 3758096384; // 3.5 GB
	// 16 rankPerNode long long int size = 1879048192; // 1.75 GB	

	long long int* sendBuf = (long long int*) malloc(size);
	long long int i;
	for(i = 0; i < size / 8; i++){
		sendBuf[i] = i;
	}
	//long long int* recvBuf = (long long int*) malloc( sizeof(long long int) * size * numProc);

	//MPI_Gather(sendBuf, size, MPI_LONG_LONG_INT, recvBuf, size, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
	free(sendBuf);
	MPI_Finalize();


	return 0;
}
