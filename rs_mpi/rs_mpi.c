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

typedef struct timespec timestamp_type;

int *final;
int final_size;

void error_logger(char* func_name, char* message) {
	fprintf(stderr, "\nERROR: func=%s()\tmsg=%s\n", message);
	MPI_Finalize();
}

int printFunction(int nprocs, int my_rank, int chunk_size,
		const array_d* local_arr) {
	for (int i = 0; i < nprocs; i++) {

		if (my_rank == i) {
			printf("\n Process #%d ", my_rank);

			for (int i = 0; i < chunk_size; i++) {
				printf("\n%d", local_arr->array[i]);
				fflush(stdout);
			}

		}
		MPI_Barrier(MPI_COMM_WORLD);

	}
	return my_rank;
}

void print_array(const int P, const int rank, int *a, int *n) {
	if (rank == 0) {
		int index = 0;

		final = (int*) malloc(sizeof(int) * final_size);
		memcpy(final, a, n[rank] * sizeof(int));

		index += n[rank];

		/*// print array for rank 0 first
		 for (int i = 0; i < n[rank]; i++) {

		 printf("%d\n", a[i]);
		 }*/
		// then receive and print from others
		for (int p = 1; p < P; p++) {


			MPI_Status stat;
			int a_size = n[p];
			int buff[a_size];

			MPI_Recv(buff, a_size, MPI_INT, p, PRINT_TAG_NUM, MPI_COMM_WORLD,
					&stat);

			memcpy(final + index, buff, a_size * sizeof(int));

			index += a_size;
			/*	for (int i = 0; i < a_size; i++) {
			 printf("%d\n", buff[i]);
			 }*/
		}

		print_numbers("test.txt", final, final_size);

		is_sorted(final, final_size);

	} else {
		// if not rank 0, send your data to other processes
		MPI_Send(a, n[rank], MPI_INT, 0, PRINT_TAG_NUM, MPI_COMM_WORLD);
	}
}

int main(int argc, char* argv[]) {

	int my_rank; /* rank of process */
	int nprocs; /* number of processes */
	int source; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */

	int chunk_size = 0;
	int nelements;

	array_d numbers;
	const int root = 0;

	/* Parsing Command Line Arguments*/
	if (argc < 2) {
		if (my_rank == 0)
			error_logger("argv",
					"Usage: mpirun -n num_procs rs_mpi input_filename output_filename");
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];
	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

	/* return status for receive */
	MPI_Status status;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (my_rank == root) {
		/* Read integers from file */
		init_array(&numbers, INITIAL_SIZE);

		read_numbers(input_file, &numbers);

		nelements = numbers.length;
		final_size = numbers.length;

		printf("\nReading Process 0 nnumbers=%d nprocs=%d\n", numbers.length,
				nprocs); // ??
	}

	/* Broadcast number of elements to all processes. No Shared memory remember? */
	MPI_Bcast(&nelements, 1, MPI_INT, root, MPI_COMM_WORLD);

	//MPI_Bcast(&chunk_size, 1, MPI_INT, root, MPI_COMM_WORLD);

	/* Do all initializations */

	chunk_size = nelements / nprocs;

	//TODO: is nelements guaranteed to be divisible by nprocs ? 100/8 ? Ask?
	// if not,make the last process handle the remainder

	if (my_rank == nprocs - 1) {
		int remainder = nelements % nprocs;
		if (remainder > 0) {
			chunk_size += remainder;
		}
	}

	//TODO: Will number of buckets be divisible by nprocs?
	//int bucket_load = BUCKETS % nprocs; // Is this check remianeder required?

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

	// is this required ?const int start_index = chunk_size * my_rank;

	//Call sort here
	//radix_sort(&local_arr);

	local_arr.array = radix_sort3(&local_arr.array[0], buckets, nprocs, my_rank,
			&chunk_size);

	MPI_Barrier(MPI_COMM_WORLD);

	//my_rank = printFunction(nprocs, my_rank, chunk_size, &local_arr);

	// store number of items per each process after the sort
	int* p_n = malloc(nprocs * sizeof(int));

	// first store our own number
	p_n[my_rank] = chunk_size;

	// communicate number of items among other processes
	MPI_Request req;
	MPI_Status stat;

	for (int i = 0; i < nprocs; i++) {
		if (i != my_rank) {
			MPI_Isend(&chunk_size, 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &req);
		}
	}

	for (int i = 0; i < nprocs; i++) {
		if (i != my_rank) {
			MPI_Recv(&p_n[i], 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &stat);
		}
	}

	/*MPI_Gather(local_arr.array, chunk_size, MPI_INT, numbers.array, chunk_size,
	 MPI_INT, root, MPI_COMM_WORLD);*/

	//MPI_Gatherv(local_arr.array,n[my_rank],MPI_INT,numbers,,MPI_INT)
	// print results
	print_array(nprocs, my_rank, local_arr.array, p_n);

	// Write sorted output into a file using a single process my_rank=0
	if (my_rank == root) {
		//Write Array here
		printf("\nPrinting in Process 0 nnumbers=%d nprocs=%d chunk_size=%d\n",
				numbers.length, nprocs, chunk_size);
		//	print_numbers(output_file, numbers.array, numbers.length);
		free_array(&numbers);

	}

	/* shut down MPI */
	MPI_Finalize();

	for (int j = 0; j < BUCKETS; j++) {
		free(buckets[j].array);
	}
	free(buckets);
	free(final);
	free(p_n);

	return 0;
}

