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

//int* final;
int final_size;
char input_file[LENGTH_FILENAME];
char output_file[LENGTH_FILENAME];
int* rcounts;
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

	/*
	 //TODO: is nelements guaranteed to be divisible by nprocs ? 100/8 ? Ask?
	 // if not,make the last process handle the remainder
	 */

	int chunk_size = nelements / nprocs;

	/* Create a list of buckets in each process to store the keys */

	array_d* buckets;

	/* Size of the array of arrays */
	buckets = malloc(BUCKETS * sizeof(array_d));

	int bucket_capacity = chunk_size / BUCKETS;
	if (bucket_capacity < BUCKETS) {
		bucket_capacity = BUCKETS; // Equal bucket sizes
	}

	for (int j = 0; j < BUCKETS; j++) {
		//	init_array(&buckets[j], bucket_capacity);
		buckets[j].array = malloc(bucket_capacity * sizeof(int));
		buckets[j].capacity = BUCKETS;
	}

	array_d local_arr;

	init_array(&local_arr, chunk_size);

	MPI_Scatter(numbers.array, chunk_size, MPI_INT, local_arr.array, chunk_size,
			MPI_INT, root, MPI_COMM_WORLD);

	local_arr.length = chunk_size;

	// let all processes get here
	MPI_Barrier(MPI_COMM_WORLD);

	/* Call Radix Sort here */

	if (my_rank == 0) {
		clock_gettime(CLOCK_REALTIME, &start);
	}

	local_arr.array = mpi_radix_sort(&local_arr.array[0], buckets, &chunk_size,
			nprocs, my_rank);

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		clock_gettime(CLOCK_REALTIME, &stop);
		print_time(get_time_diff(&stop, &start));
	}

	/* store number of items per each process after the sort*/
	int ct[nprocs];

	rcounts = malloc(nprocs * sizeof(int));

	for (int i = 0; i < nprocs; i++) {
		ct[i] = 0;
	}
	// first store our own number
	ct[my_rank] = chunk_size;

	/*// communicate number of items among other processes*/
	MPI_Allreduce(ct, rcounts, nprocs, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	/* Write sorted output into a file using a single process my_rank=0 */

	MPI_Barrier(MPI_COMM_WORLD);

	int displs[nprocs];
	int sum = 0;
	displs[root] = sum;

	for (int i = 1; i < nprocs; i++) {
		sum = sum + rcounts[i - 1];
		displs[i] = sum;
	}

	MPI_Gatherv(local_arr.array, rcounts[my_rank], MPI_INT, numbers.array, ct,
			displs, MPI_INT, root, MPI_COMM_WORLD);

	if (my_rank == 0) {
		print_numbers(output_file, numbers.array, final_size);

		is_sorted(numbers.array, final_size);

		free_array(&numbers);
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

int* mpi_radix_sort(int *local_array, array_d* buckets, int * local_chunk_size,
		const int nprocs, const int my_rank) {

	int count[BUCKETS][nprocs];
	int local_counts[BUCKETS];
	int local_num_buckets = BUCKETS / nprocs; // number of local buckets per process
	int prefix_sum[local_num_buckets][nprocs];

	// MPI request and status
	MPI_Request request;
	MPI_Status status;

	for (int pass = 0; pass < NUM_PASSES; pass++) {          // each pass

		// init counts arrays
		for (int j = 0; j < BUCKETS; j++) {
			count[j][my_rank] = 0;
			local_counts[j] = 0;
			buckets[j].length = 0;
		}

		// count items per bucket
		int sort_key;
		for (int i = 0; i < *local_chunk_size; i++) {
			sort_key = 0;
			sort_key = get_bits(local_array[i], pass * r, r);
			count[sort_key][my_rank]++;
			local_counts[sort_key]++;
			if (!insert_array(&buckets[sort_key], local_array[i])) {
				return NULL;
			}
		}

		// do one-to-all transpose
		for (int p = 0; p < nprocs; p++) {
			if (p != my_rank) {
				// send counts of this process to others
				MPI_Isend(local_counts,
				BUCKETS, MPI_INT, p,
				COUNTS_TAG_NUM, MPI_COMM_WORLD, &request);
			}
		}

		// receive counts from others
		for (int p = 0; p < nprocs; p++) {
			if (p != my_rank) {
				MPI_Recv(local_counts,
				BUCKETS, MPI_INT, p,
				COUNTS_TAG_NUM, MPI_COMM_WORLD, &status);

				// populate counts per bucket for other processes
				for (int i = 0; i < BUCKETS; i++) {
					count[i][p] = local_counts[i];
				}
			}
		}

		// calculate new size based on values received from all processes
		int new_size = 0;
		for (int j = 0; j < local_num_buckets; j++) {
			int idx = j + my_rank * local_num_buckets;
			for (int p = 0; p < nprocs; p++) {
				prefix_sum[j][p] = new_size;
				new_size += count[idx][p];
			}
		}

		// reallocate array if newly calculated size is larger
		if (new_size > *local_chunk_size) {
			int* temp = realloc(local_array, new_size * sizeof(int));
			if (!local_array) {
				if (my_rank == 0) {
					printf("ERROR: Could not realloc for size %d!\n", new_size);
				}
				return NULL;
			}
			// reassign pointer back to original
			local_array = temp;
		}

		// send keys of this process to others
		for (int j = 0; j < BUCKETS; j++) {
			int p = j / local_num_buckets; // determine which process this buckets belongs to
			int p_j = j % local_num_buckets; // transpose to that process local bucket index
			if (p != my_rank && buckets[j].length > 0) {
				MPI_Isend(buckets[j].array, buckets[j].length, MPI_INT, p, p_j,
						MPI_COMM_WORLD, &request);
			}
		}

		// receive keys from other processes
		for (int j = 0; j < local_num_buckets; j++) {
			// transpose from local to global index
			int idx = j + my_rank * local_num_buckets;
			for (int p = 0; p < nprocs; p++) {

				// get bucket count
				int b_count = count[idx][p];
				if (b_count > 0) {

					// point to an index in array where to insert received keys
					int *dest = &local_array[prefix_sum[j][p]];
					if (my_rank != p) {
						MPI_Recv(dest, b_count, MPI_INT, p, j, MPI_COMM_WORLD,
								&status);

					} else {
						// is same process, copy from buckets to our array
						memcpy(dest, &buckets[idx].array[0],
								b_count * sizeof(int));
					}
				}
			}
		}

		// update new size
		*local_chunk_size = new_size;

	}

	return local_array;
}
