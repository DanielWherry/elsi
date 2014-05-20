#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv){ // or char* argv[] , pointer to a pointer

	int id, ierr, p;
	
	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &p);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &id);

	printf("Process %d says 'Hello, World'\n",id);
	
	ierr = MPI_Finalize();
	return 0;
} 
