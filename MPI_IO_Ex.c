#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


int main(int argc, char ** argv){

	const int SIZE = 5000;
	int rank, numProc; 

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);


	int* integers = (int*) malloc(SIZE * 4);
	
	int i;
	for(i = 0; i < SIZE; i++){
		integers[i] =  i; 
	}



	MPI_File file;
	MPI_Status status;
	MPI_Offset disp = rank * sizeof(MPI_INT) * SIZE;
	
	MPI_File_open(MPI_COMM_WORLD, "numbers.dat", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	
	MPI_File_set_view(file, disp, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);	
	
	MPI_File_write_ordered(file, integers, disp, MPI_INT, &status);

	MPI_File_close(&file);
	

	
	MPI_Finalize();
	free(integers);
	return 0;
}
