/*
 ============================================================================
 Name        : rs_mpi.c
 Author      : jthiru
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C
 ============================================================================
 */

#include <mpi.h>
#include "my_headers.h"

int final_size;
int* rcounts;
int* final;

int* radix_sort(int *local_array, d_array* buckets, const int nprocs, const int my_rank, int * chunk_size) {
	int bin_count[bins][nprocs];
	int l_count[bins];    // array of local process counts per bucket
	int l_B = bins / nprocs;   // number of local buckets per process
	int p_sum[l_B][nprocs]; // array of prefix sums

	// MPI request and status
	MPI_Request req;
	MPI_Status stat;

	for (int pass = 0; pass < npasses; pass++) {          // each pass

		// Set to 0s , else wrong values
		for (int j = 0; j < bins; j++) {
			bin_count[j][my_rank] = 0;
			l_count[j] = 0;
			buckets[j].length = 0;
		}

		// count items per bucketi
		for (int i = 0; i < *chunk_size; i++) {
			unsigned int idx = get_lsd_group_bits(local_array[i], pass * r, r);
			bin_count[idx][my_rank]++;
			l_count[idx]++;
			if (!insert_item(&buckets[idx], local_array[i])) {
				return NULL;
			}
		}

		// do one-to-all transpose
		for (int p = 0; p < nprocs; p++) {
			if (p != my_rank) {
				// send counts of this process to others
				MPI_Isend(l_count,
				bins, MPI_INT, p,
				C_TAG_NUM, MPI_COMM_WORLD, &req);
			}
		}

		// receive counts from others
		for (int p = 0; p < nprocs; p++) {
			if (p != my_rank) {
				MPI_Recv(l_count,
				bins, MPI_INT, p,
				C_TAG_NUM, MPI_COMM_WORLD, &stat);

				// populate counts per bucket for other processes
				for (int i = 0; i < bins; i++) {
					bin_count[i][p] = l_count[i];
				}
			}
		}

		// calculate new size based on values received from all processes
		int new_size = 0;
		for (int j = 0; j < l_B; j++) {
			int idx = j + my_rank * l_B;
			for (int p = 0; p < nprocs; p++) {
				p_sum[j][p] = new_size;
				new_size += bin_count[idx][p];
			}
		}

		// reallocate array if newly calculated size is larger
		if (new_size > *chunk_size) {
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
		for (int j = 0; j < bins; j++) {
			int p = j / l_B;  // determine which process this buckets belongs to
			int p_j = j % l_B; // transpose to that process local bucket index
			if (p != my_rank && buckets[j].length > 0) {
				MPI_Isend(buckets[j].array, buckets[j].length, MPI_INT, p, p_j,
						MPI_COMM_WORLD, &req);
			}
		}

		// receive keys from other processes
		for (int j = 0; j < l_B; j++) {
			// transpose from local to global index
			int idx = j + my_rank * l_B;
			for (int p = 0; p < nprocs; p++) {

				// get bucket count
				int b_count = bin_count[idx][p];
				if (b_count > 0) {

					// point to an index in array where to insert received keys
					int *dest = &local_array[p_sum[j][p]];
					if (my_rank != p) {
						MPI_Recv(dest, b_count, MPI_INT, p, j, MPI_COMM_WORLD,
								&stat);

					} else {
						// is same process, copy from buckets to our array
						memcpy(dest, &buckets[idx].array[0],
								b_count * sizeof(int));
					}
				}
			}
		}
		// update new size
		*chunk_size = new_size;
	}

	return local_array;
}

int main(int argc, char** argv) {

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];

	t_time start, stop;
	d_array numbers;

	int my_rank, nprocs, nnumbers;
	int print_results = 0;

	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (my_rank == 0) {
		numbers.array = (int *) malloc(sizeof(int) * INITIAL_SIZE);
		numbers.capacity = INITIAL_SIZE;
		numbers.length = 0;
		read_numbers(input_file, &numbers);
		final_size = nnumbers = numbers.length;
	}

	/* Broadcast number of elements to all processes. No Shared memory remember? */
	MPI_Bcast(&nnumbers, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int chunk_size = nnumbers / nprocs;

	int b_capacity = chunk_size / bins;
	if (b_capacity < bins) {
		b_capacity = bins;
	}
	d_array* buckets = malloc(bins * sizeof(d_array));
	for (int j = 0; j < bins; j++) {
		buckets[j].array = malloc(b_capacity * sizeof(int));
		buckets[j].capacity = bins;
	}

	int* local_array = malloc(sizeof(int) * chunk_size);

	MPI_Scatter(numbers.array, chunk_size, MPI_INT, local_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

	/* Start Radix Sort */

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		clock_gettime(CLOCK_REALTIME, &start);
		free(numbers.array);
	}

	local_array = radix_sort(&local_array[0], buckets, nprocs, my_rank, &chunk_size);

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		clock_gettime(CLOCK_REALTIME, &stop);
		print_time(get_time_diff(&stop, &start));
	}

	/* End Radix Sort */

	int* n_proc = malloc(nprocs * sizeof(int));

	n_proc[my_rank] = chunk_size;

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
			MPI_Recv(&n_proc[i], 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &stat);
		}
	}

	/* create final merge array */
	if (my_rank == 0) {
		final = malloc(sizeof(int) * final_size);
	}

	/* calculate displacements for Gatherv */
	int displs[nprocs];
	int sum = 0;
	displs[0] = sum;

	for (int i = 1; i < nprocs; i++) {
		sum = sum + n_proc[i - 1];
		displs[i] = sum;
	}

	/* Get all the data from the individual processes */
	MPI_Gatherv(final, n_proc[my_rank], MPI_INT, local_array, n_proc, displs, MPI_INT, 0,
			MPI_COMM_WORLD);

	/* Data output */

	if (my_rank == 0) {

		print_numbers(output_file, final, final_size);

		/* To check if sort is working correctly*/
		printf("\nSorted=%d\n", is_sorted(final, final_size));

	}

	MPI_Finalize();

	for (int j = 0; j < bins; j++) {
		free(buckets[j].array);
	}
	free(buckets);
	free(local_array);
	free(n_proc);
	free(final);

	return 0;
}
