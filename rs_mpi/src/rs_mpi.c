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

void radix_sort_mpi() {
	size_t chunk_size = numbers.used % nprocs;

	array_d chunk;

	init_array(chunk, chunk_size);

	MPI_Scatter(numbers.array, chunk_size, MPI_INT, chunk.array, chunk.size,
			MPI_INT, 0, MPI_COMM_WORLD);

	/*  sort here */

	radix_sort(&chunk);

	/* Call gather here, check for synchronisation	 */

	MPI_Gather(numbers.array, chunk_size, MPI_INT, chunk.array, chunk.size,
			MPI_INT, 0, MPI_COMM_WORLD);

}

int mpi_main(int argc, char* argv[]) {
	int my_rank; /* rank of process */
	int nprocs; /* number of processes */
	int source; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	char message[100]; /* storage for message */
	MPI_Status status; /* return status for receive */

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];
	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

	/* Array to be shared*/
	array_d numbers;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (my_rank == 0) {
		/* Read integers from file */
		init_array(&numbers, INITIAL_SIZE);
		read_numbers(input_file, &numbers);
	}

	/* Call scatter here, check if they start only after read array completes */

	MPI_Barrier(MPI_COMM_WORLD);

	/*
	 * Call sort here
	 */

	radix_sort_mpi();

	/*	MPI_Reduce(local_sort_times, total_sort_times, 9, MPI_DOUBLE,
	 MPI_SUM, 0, MPI_COMM_WORLD);
	 */

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		/*
		 * Write Array here
		 */
		is_sorted(numbers.array, numbers.used);
		print_numbers(output_file, numbers.array, numbers.used);
		free_array(&numbers);
	}

	/* shut down MPI */
	MPI_Finalize();
	return 0;
}

