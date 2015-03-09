
/*
 ============================================================================
 Name        : rs_mpi.c
 Author      : jthiru
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C
 ============================================================================
 */
/**
 * Parallel implementation of radix sort. The list to be sorted is split
 * across multiple MPI processes and each sub-list is sorted during each
 * pass as in straight radix sort. 
 */
#include <mpi.h>
#include "my_constants.h"

int final_size;
int* rcounts;
int* final;

typedef struct list List;
struct list {
	int* array;
	size_t length;
	size_t capacity;
};

int add_item(List* list, int item) {
	if (list->length >= list->capacity) {
		size_t new_capacity = list->capacity * 2;
		int* temp = realloc(list->array, new_capacity * sizeof(int));
		if (!temp) {
			printf("ERROR: Could not realloc for size %d!\n",
					(int) new_capacity);
			return 0;
		}
		list->array = temp;
		list->capacity = new_capacity;
	}

	list->array[list->length++] = item;

	return 1;
}
void print_time(double const seconds) {
	printf("Sort Time: %0.04fs\n", seconds);
}

void error_logger(char* func_name, char* message) {
	fprintf(stderr, "\nERROR: func=%s()\tmsg=%s\n", message);
	MPI_Finalize();
}
double get_time_diff(const struct timespec* stop, const struct timespec* start) {
	/*double diff = ((end->tv_sec *BILLION + end->tv_nsec )
	 - (start->tv_sec * BILLION + start->tv_nsec))/BILLION;*/
	return (stop->tv_sec - start->tv_sec)
			+ (double) (stop->tv_nsec - start->tv_nsec) / (double) BILLION;
}

/* usage : (x, 0, 1)
 * gets rightmost bit of x
 */

int get_bits(unsigned x, int k, int j) {
	return (x >> k) & ~(~0 << j);
}
int insert_array(array_d* arr, int num) {
	if (arr->length == arr->capacity) {
		size_t c = arr->capacity * 2;
		int* t = (int *) realloc(arr->array, (sizeof(int) * c));
		if (!t) {
			printf("ERROR: Could not realloc for size %d!\n", (int) c);
			return 0;
		}
		arr->array = t;
		arr->capacity = c;
	}
	arr->array[arr->length++] = num;
	return 1;
}

/**
 * @brief Read an array of integers from a file.
 *
 * @param filename The name of the file to read from
 * @param numbers The array of numbers will be stored here
 *
 */

void read_numbers(char const * const filename, List * const numbers) {

	FILE * fin;
	int temp;

	int scanresult;

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	while ((scanresult = fscanf(fin, "%d", &temp)) > 0) {
		add_item(numbers, temp);
	}

	fclose(fin);
}

int is_sorted(int* const numbers, size_t size) {
	for (int i = 1; i < size; i++) {
		if (numbers[i] < numbers[i - 1]) {
			printf(
					"\nSORT ERROR; At index[%d] number %d should not be before %d \n",
					i, numbers[i - 1], numbers[i]);
			return 0;
		}
	}
	return 1;
}

void print_numbers(char const * const filename, int const * const numbers,
		size_t const nnumbers) {
	size_t i;
	FILE * fout;

	/* open file */
	if ((fout = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	/* write numbers to fout */
	for (i = 0; i < nnumbers; ++i) {
		fprintf(fout, "%d\n", numbers[i]);
	}

	fclose(fout);
}
// add item to a dynamic array encapsulated in a structure


// Compute j bits which appear k bits from the right in x
// Ex. to obtain rightmost bit of x call bits(x, 0, 1)
unsigned bits(unsigned x, int k, int j) {
	return (x >> k) & ~(~0 << j);
}

/*
 * Initialize dynammic array
 */
void init_array(array_d* arr, size_t initial_size) {
	arr->array = (int *) malloc(sizeof(int) * initial_size);
	arr->capacity = initial_size;
	arr->length = 0;
}

/*
 * Function to free dynammic array
 */

void free_array(array_d* arr) {
	free(arr->array);
	arr->array = NULL;
	arr->length = arr->capacity = 0;
}

// Radix sort elements while communicating between other MPI processes
// a - array of elements to be sorted
// buckets - array of buckets, each bucket pointing to array of elements
// P - total number of MPI processes
// rank - rank of this MPI process
// n - number of elements to be sorted
int* radix_sort(int *a, List* buckets, const int P, const int rank, int * n) {
	int count[B][P];   // array of counts per bucket for all processes
	int l_count[B];    // array of local process counts per bucket
	int l_B = B / P;   // number of local buckets per process
	int p_sum[l_B][P]; // array of prefix sums

	// MPI request and status
	MPI_Request req;
	MPI_Status stat;

	for (int pass = 0; pass < N; pass++) {          // each pass

		// init counts arrays
		for (int j = 0; j < B; j++) {
			count[j][rank] = 0;
			l_count[j] = 0;
			buckets[j].length = 0;
		}

		// count items per bucket
		for (int i = 0; i < *n; i++) {
			unsigned int idx = bits(a[i], pass * g, g);
			count[idx][rank]++;
			l_count[idx]++;
			if (!add_item(&buckets[idx], a[i])) {
				return NULL;
			}
		}

		// do one-to-all transpose
		for (int p = 0; p < P; p++) {
			if (p != rank) {
				// send counts of this process to others
				MPI_Isend(l_count,
				B, MPI_INT, p,
				COUNTS_TAG_NUM, MPI_COMM_WORLD, &req);
			}
		}

		// receive counts from others
		for (int p = 0; p < P; p++) {
			if (p != rank) {
				MPI_Recv(l_count,
				B, MPI_INT, p,
				COUNTS_TAG_NUM, MPI_COMM_WORLD, &stat);

				// populate counts per bucket for other processes
				for (int i = 0; i < B; i++) {
					count[i][p] = l_count[i];
				}
			}
		}

		// calculate new size based on values received from all processes
		int new_size = 0;
		for (int j = 0; j < l_B; j++) {
			int idx = j + rank * l_B;
			for (int p = 0; p < P; p++) {
				p_sum[j][p] = new_size;
				new_size += count[idx][p];
			}
		}

		// reallocate array if newly calculated size is larger
		if (new_size > *n) {
			int* temp = realloc(a, new_size * sizeof(int));
			if (!a) {
				if (rank == 0) {
					printf("ERROR: Could not realloc for size %d!\n", new_size);
				}
				return NULL;
			}
			// reassign pointer back to original
			a = temp;
		}

		// send keys of this process to others
		for (int j = 0; j < B; j++) {
			int p = j / l_B;  // determine which process this buckets belongs to
			int p_j = j % l_B; // transpose to that process local bucket index
			if (p != rank && buckets[j].length > 0) {
				MPI_Isend(buckets[j].array, buckets[j].length, MPI_INT, p, p_j,
						MPI_COMM_WORLD, &req);
			}
		}

		// receive keys from other processes
		for (int j = 0; j < l_B; j++) {
			// transpose from local to global index
			int idx = j + rank * l_B;
			for (int p = 0; p < P; p++) {

				// get bucket count
				int b_count = count[idx][p];
				if (b_count > 0) {

					// point to an index in array where to insert received keys
					int *dest = &a[p_sum[j][p]];
					if (rank != p) {
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
		*n = new_size;
	}

	return a;
}

int main(int argc, char** argv) {

	int rank, size, n_total;
	int print_results = 0;
	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];

	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	timestamp_type start, stop;

	List numbers;

	if (rank == 0) {
		numbers.array = (int *) malloc(sizeof(int) * INITIAL_SIZE);
		numbers.capacity = INITIAL_SIZE;
		numbers.length = 0;
		read_numbers(input_file, &numbers);
		final_size = n_total = numbers.length;
	}

	/* Broadcast number of elements to all processes. No Shared memory remember? */
	MPI_Bcast(&n_total, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int n = n_total / size;

	int b_capacity = n / B;
	if (b_capacity < B) {
		b_capacity = B;
	}
	List* buckets = malloc(B * sizeof(List));
	for (int j = 0; j < B; j++) {
		buckets[j].array = malloc(b_capacity * sizeof(int));
		buckets[j].capacity = B;
	}

	int* a = malloc(sizeof(int) * n);

	MPI_Scatter(numbers.array, n, MPI_INT, a, n, MPI_INT, 0, MPI_COMM_WORLD);

//-------------------------------------------------------------------------------

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) {
		clock_gettime(CLOCK_REALTIME, &start);
		//free(numbers.array);
	}

	a = radix_sort(&a[0], buckets, size, rank, &n);

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) {
		clock_gettime(CLOCK_REALTIME, &stop);
		print_time(get_time_diff(&stop, &start));
	}

//-------------------------------------------------------------------------------
	// store number of items per each process after the sort
	int* p_n = malloc(size * sizeof(int));

	// first store our own number
	p_n[rank] = n;

	// communicate number of items among other processes
	MPI_Request req;
	MPI_Status stat;

	for (int i = 0; i < size; i++) {
		if (i != rank) {
			MPI_Isend(&n, 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &req);
		}
	}

	for (int i = 0; i < size; i++) {
		if (i != rank) {
			MPI_Recv(&p_n[i], 1, MPI_INT, i,
			NUM_TAG, MPI_COMM_WORLD, &stat);
		}
	}

	if (rank == 0) {
		final = malloc(sizeof(int) * final_size);
	}

	int displs[size];
	int sum = 0;
	displs[0] = sum;

	for (int i = 1; i < size; i++) {
		sum = sum + p_n[i - 1];
		displs[i] = sum;
	}

	MPI_Gatherv(final, p_n[rank], MPI_INT, a, p_n, displs, MPI_INT, 0,
			MPI_COMM_WORLD);

	if (rank == 0) {
		print_numbers(output_file, final, final_size);
		printf("\nSorted=%d\n", is_sorted(final, final_size));

	}

	// release MPI resources
	MPI_Finalize();

	// release memory allocated resources
	for (int j = 0; j < B; j++) {
		free(buckets[j].array);
	}
	free(buckets);
	free(a);
	free(p_n);
	free(final);

	return 0;
}
