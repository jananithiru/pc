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

void radix_sort(int *data, const int nnumbers) {
	unsigned * buffer = malloc(nnumbers * sizeof(unsigned));
	int total_digits = sizeof(unsigned) * 8;

	//Each thread use local_bucket to move data
	size_t i;
	for (int shift = 0; shift < total_digits; shift += r) {
		size_t bucket[bins] = { 0 };
		size_t local_bucket[bins] = { 0 }; // size needed in each bucket/thread
		//1st pass, scan whole and check the count
#pragma omp parallel firstprivate(local_bucket)
		{
#pragma omp for schedule(static) nowait
			for (i = 0; i < nnumbers; i++) {
				local_bucket[DIGITS(data[i], shift)]++;
			}
#pragma omp critical
			for (i = 0; i < bins; i++) {
				bucket[i] += local_bucket[i];
			}
#pragma omp barrier
#pragma omp single
			for (i = 1; i < bins; i++) {
				bucket[i] += bucket[i - 1];
			}
			int nthreads = omp_get_num_threads();
			int tid = omp_get_thread_num();
			for (int cur_t = nthreads - 1; cur_t >= 0; cur_t--) {
				if (cur_t == tid) {
					for (i = 0; i < bins; i++) {
						bucket[i] -= local_bucket[i];
						local_bucket[i] = bucket[i];
					}
				} else { //just do barrier
#pragma omp barrier
				}

			}
#pragma omp for schedule(static)
			for (i = 0; i < nnumbers; i++) { //note here the end condition
				buffer[local_bucket[DIGITS(data[i], shift)]++] = data[i];
			}
		}
		//now move data
		unsigned* tmp = data;
		data = buffer;
		buffer = tmp;
	}
	free(buffer);
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

	printf("%s",argv[2]);
	printf("%d",nthreads);

	omp_set_num_threads(nthreads);

	t_time start, stop;
	d_array numbers;

	numbers.array = (int *) malloc(sizeof(int) * INITIAL_SIZE);
	numbers.capacity = INITIAL_SIZE;
	numbers.length = 0;
	read_numbers(input_file, &numbers);

	final_size = n = numbers.length;

	clock_gettime(CLOCK_REALTIME, &start);

	/* Call radix sort here */
	radix_sort(&numbers.array[0], n);

	clock_gettime(CLOCK_REALTIME, &stop);
	print_time(get_time_diff(&stop, &start));

	printf("\nSorted=%d\n", is_sorted(numbers.array, n));
	print_numbers(output_file, numbers.array, n);

	free(numbers.array);
	return 0;
}
