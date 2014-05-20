#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv){ // or char* argv[] , pointer to a pointer

	int rank, size;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);// Initializes which core is being used
	MPI_Comm_size(MPI_COMM_WORLD, &size);//Initilizes how many cores there are

	printf("Hello from process %d of %d\n",rank, size);
	
	MPI_Finalize();
	return 0;
} 
