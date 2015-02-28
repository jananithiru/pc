/*
 ============================================================================
 Name        : mpi.c
 Author      : jthiru
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C
 ============================================================================
 */
#include "my_headers.h"
#include "mpi.h"



int main(int argc, char* argv[]) {
	int my_rank; /* rank of process */
	int p; /* number of processes */
	int source; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	char message[100]; /* storage for message */
	MPI_Status status; /* return status for receive */


	char input_file[100];
	strcpy(input_file,argv[1]);

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (my_rank == 0) {
		/* Read integers from file */
		array_d numbers;
		init_array(&numbers, INITIAL_SIZE);
		read_numbers(input_file,&numbers);
		size_t nnumbers = numbers.used;
	}

	/*
	 * Call scatter here, check if they start only after read array completes
	 */

	/*
	 * Call gather here
	 */

	if (my_rank == 0) {

		/*
		 * Write Array here
		 */
	}

	/* shut down MPI */
	MPI_Finalize();
	return 0;
}
