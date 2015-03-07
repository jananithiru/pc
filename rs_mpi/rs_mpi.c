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

void radix_sort_mpi(array_d* numbers, int nprocs) {

	size_t chunk_size = numbers->used / nprocs;
	;

	printf("Radix sort %d %d", numbers->used, chunk_size);

	int* local_array = (int*) malloc(chunk_size * sizeof(int));

	/*	array_d chunk;	 init_array(chunk, chunk_size);*/

	MPI_Scatter(numbers->array, chunk_size, MPI_INT, local_array, chunk_size,
			MPI_INT, 0, MPI_COMM_WORLD);

	/* Call sort here */

	radix_sort(&local_array);

	/*	MPI_Reduce(local_sort_times, total_sort_times, 9, MPI_DOUBLE,
	 MPI_SUM, 0, MPI_COMM_WORLD);
	 */

	MPI_Gather(local_array, chunk_size, MPI_INT, numbers->array, chunk_size,
			MPI_INT, 0, MPI_COMM_WORLD);

}

int main(int argc, char* argv[]) {

	int my_rank; /* rank of process */
	int nprocs; /* number of processes */
	int source; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */

	int chunk_size = 0;

	size_t nelements;
	array_d numbers;

	MPI_Status status; /* return status for receive */

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];
	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

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

		chunk_size = numbers.used / nprocs;  //TODO: is it guaranteed to be divisible by numProcs ? 100000000

		printf("\nReading Process 0 %d %d", numbers.used, chunk_size); // ??

	}

	MPI_Bcast(&chunk_size,1,MPI_INT,0,MPI_COMM_WORLD);

/*
	printf("Process %d %d", numbers.used, nprocs);
	printf("Radix sort %d %d", numbers.used, chunk_size);

	array_d local;
	init_array(&local, chunk_size);

	MPI_Scatter(numbers.array, chunk_size, MPI_INT, local.array, chunk_size,
			MPI_INT, 0, MPI_COMM_WORLD);

	 Call sort here

	radix_sort(&local.array);

		MPI_Reduce(local_sort_times, total_sort_times, 9, MPI_DOUBLE,
	 MPI_SUM, 0, MPI_COMM_WORLD);


	MPI_Gather(local.array, chunk_size, MPI_INT, numbers.array, chunk_size,
			MPI_INT, 0, MPI_COMM_WORLD);

	//MPI_Barrier(MPI_COMM_WORLD);
*/
	if (my_rank == 0) {
		/*  Write Array here */
		is_sorted(numbers.array, numbers.used);
		printf("\nPrinting Process 0 %d %d", numbers.used, nprocs);
		print_numbers(output_file, numbers.array, numbers.used);
		free_array(&numbers);
	}

	/* shut down MPI */
	MPI_Finalize();
	return 0;
}

