/*
 ============================================================================
 Name        : rs_openmp.c
 Author      : jthiru
 Version     :
 Copyright   : Your copyright notice
 Description : Hello OpenMP World in C
 ============================================================================
 */
#include "my_headers.h"
#include "omp.h"

int final_size;
int* rcounts;
int* final;

void radix_sort(int *a, const int n) {
	int* t = malloc(n * sizeof(int));     // temp array used for sorting
	int count[B];                       // array of counts per bucket

	for (int pass = 0; pass < b / g; pass++) {       // each pass
		for (int j = 0; j < B; j++)
			count[j] = 0;    // init counts array
		for (int i = 0; i < n; i++) {
			count[bits(a[i], pass * g, g)]++;           // count keys per bucket
		}
		for (int j = 1; j < B; j++) {
			count[j] = count[j - 1] + count[j];          // compute prefix sum
		}
		for (int i = n - 1; i >= 0; i--) {
			int idx = --count[bits(a[i], pass * g, g)];
			t[idx] = a[i];                            // transpose to temp array
		}
		for (int i = 0; i < n; i++)
			a[i] = t[i];     // copy back to master
	}
	free(t);
}

int main(int argc, char** argv) {
	int print_results = 0;
	int nthreads;
	int n;

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];

	strcpy(input_file, argv[1]);
	nthreads = atoi(argv[2]);
	strcpy(output_file, argv[3]);

	timestamp_type start, stop;

	List numbers;

	#pragma omp parallel num_threads(nthreads)
	{
		#pragma omp single
		{
			numbers.array = (int *) malloc(sizeof(int) * INITIAL_SIZE);
			numbers.capacity = INITIAL_SIZE;
			numbers.length = 0;
			read_numbers(input_file, &numbers);
			final_size = n = numbers.length;

			printf("%d", final_size);
		}
		int tid = omp_get_thread_num();

		// take a timestamp before the sort starts
		if (tid == 0) {
			clock_gettime(CLOCK_REALTIME, &start);
			//free(numbers.array);
		}

		// sort elements
		radix_sort(&numbers.array[0], n);

		if (tid == 0) {
			clock_gettime(CLOCK_REALTIME, &stop);
			print_time(get_time_diff(&stop, &start));
		}

		#pragma omp single
		{
			print_array(&numbers.array[0], n);
			printf("\nSorted=%d\n", is_sorted(numbers.array, n));
		}

		printf("Hello World from thread number %d\n", tid);
	}
	// release resources no longer used
	free(numbers.array);
	return 0;
}
