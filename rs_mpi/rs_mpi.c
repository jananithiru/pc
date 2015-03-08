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

int* final;
int final_size;
char input_file[LENGTH_FILENAME];
char output_file[LENGTH_FILENAME];

int main(int argc, char* argv[]) {

	int my_rank; /* rank of process */
	int nprocs; /* number of processes */

	int nelements;

	const int root = 0;

	/* return status for request and receive */
	MPI_Request request;
	MPI_Status status;

	struct timeval start, stop;

	/* Parsing Command Line Arguments*/
	if (argc < 2) {
		if (my_rank == 0)
			error_logger("argv",
					"Usage: mpirun -n num_procs rs_mpi input_filename output_filename");
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	/* Read integers from file */

	array_d numbers;
	if (my_rank == root) {
		init_array(&numbers, INITIAL_SIZE);
		read_numbers(input_file, &numbers);
		nelements = numbers.length;
		final_size = numbers.length;
	}

	/* Broadcast number of elements to all processes. No Shared memory remember? */
	MPI_Bcast(&nelements, 1, MPI_INT, root, MPI_COMM_WORLD);

	/* Do all initializations */

	int chunk_size = nelements / nprocs;

	/*
	 //TODO: is nelements guaranteed to be divisible by nprocs ? 100/8 ? Ask?
	 // if not,make the last process handle the remainder
	 */

	if (my_rank == nprocs - 1) {
		int remainder = nelements % nprocs;
		if (remainder > 0) {
			chunk_size += remainder;
		}
	}

	/*TODO: Will number of buckets be divisible by nprocs?
	 //int bucket_load = BUCKETS % nprocs; // Is this check remianeder required?
	 */
	int bucket_capacity = chunk_size / BUCKETS;
	if (bucket_capacity < BUCKETS) {
		bucket_capacity = BUCKETS; // Equal bucket sizes
	}

	array_d local_arr;

	init_array(&local_arr, chunk_size);

	MPI_Scatter(numbers.array, chunk_size, MPI_INT, local_arr.array, chunk_size,
			MPI_INT, root, MPI_COMM_WORLD);

	local_arr.length = chunk_size;

	/* Create a list of arrays to store the keys */
	array_d* buckets;

	/* Size of the array of arrays */
	buckets = malloc(BUCKETS * sizeof(array_d));

	for (int j = 0; j < BUCKETS; j++) {
		buckets[j].array = malloc(bucket_capacity * sizeof(int));
		buckets[j].capacity = BUCKETS;
	}

	/* Call Radix Sort here */

	if (my_rank == 0) {
		clock_gettime(CLOCK_REALTIME, &start);
	}

	local_arr.array = radix_sort3(&local_arr.array[0], buckets, nprocs, my_rank,
			&chunk_size);

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		clock_gettime(CLOCK_REALTIME, &stop);
		print_time(get_time_diff(&stop, &start));
	}

	/* store number of items per each process after the sort*/
	int* rcounts = malloc(nprocs * sizeof(int));

	// first store our own number
	rcounts[my_rank] = chunk_size;

	// communicate number of items among other processes

	for (int i = 0; i < nprocs; i++) {
		if (i != my_rank) {
			MPI_Isend(&chunk_size, 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &request);
		}
	}

	for (int i = 0; i < nprocs; i++) {
		if (i != my_rank) {
			MPI_Recv(&rcounts[i], 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &status);
		}
	}

	/* Write sorted output into a file using a single process my_rank=0 */

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {

		int index = 0;

		//final = (int*) malloc(sizeof(int) * final_size);

		memcpy(numbers.array, local_arr.array, rcounts[0] * sizeof(int));

		index = index + rcounts[0];

		// then receive and print from others
		for (int p = 1; p < nprocs; p++) {

			MPI_Status stat;
			int a_size = rcounts[p];
			int buff[a_size];

			MPI_Recv(buff, a_size, MPI_INT, p, PRINT_TAG_NUM, MPI_COMM_WORLD,
					&stat);
			memcpy(numbers.array + index, buff, a_size * sizeof(int));
			index = index + a_size + 1;

		}

		print_numbers(output_file, numbers.array, final_size);

		is_sorted(numbers.array, final_size);

		free_array(&numbers);

	} else {
		// if not rank 0, send your data to other processes
		MPI_Send(local_arr.array, rcounts[my_rank], MPI_INT, 0, PRINT_TAG_NUM,
				MPI_COMM_WORLD);
	}

	/* shut down MPI */
	MPI_Finalize();

	/* Free all allocated arrays*/
	for (int j = 0; j < BUCKETS; j++) {
		free(buckets[j].array);
	}
	free(buckets);
	//free(final);
	free(rcounts);
	free_array(&local_arr);

	return 0;
}

