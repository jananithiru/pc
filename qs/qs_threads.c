# include "my_headers.h"

double get_time_diff(const struct timespec* stop,
		const struct timespec* start) {
	/*double diff = ((end->tv_sec *BILLION + end->tv_nsec )
			- (start->tv_sec * BILLION + start->tv_nsec))/BILLION;*/
	return ( stop->tv_sec - start->tv_sec )
	          + (double)( stop->tv_nsec - start->tv_nsec )/(double) BILLION;
}

float* copy_numbers(size_t nnumbers, const array_d* input_floats) {
	float* numbers = (float*) malloc(nnumbers * sizeof(float));
	memcpy(numbers, input_floats->array, nnumbers * sizeof(float));
	return numbers;
}

int main(int argc, char** argv) {

	struct timeval start, stop;
	char inputfile[LENGTH_FILENAME], outputfile[LENGTH_FILENAME];

	if (argc != 4) {
		printf("USAGE: "
				"./qs_threads.c "
				"input_filename "
				"num_threads "
				"output_filename");
		exit(0);
	}

	strcpy(inputfile, argv[1]);
	int nthreads = atoi(argv[2]); //strtoimax(argv[2]);
	strcpy(outputfile, argv[3]);

	array_d input_floats;
	init_array(&input_floats, INITIAL_SIZE);

	//read_numbers(inputfile, &input_floats); // MUST FREE

	/*
	 * Read numbers as a thread
	 */
	threaded_read_numbers(inputfile, &input_floats);

	size_t nnumbers = input_floats.used;

	float* numbers = copy_numbers(nnumbers, &input_floats); // MUST FREE

	clock_gettime(CLOCK_REALTIME, &start);

	/*
	 * Run parallel quicksort.
	 * Set nthreads =0 for sequential
	 * 1 for 2 threads , 2 for 4 threads , 3 for 8 threads and so on.
	 */

	parallel_qs(numbers, nnumbers, nthreads);

	clock_gettime(CLOCK_REALTIME, &stop);

	print_time(get_time_diff(&stop, &start));

	is_sorted(numbers, nnumbers);

	is_sort_correct(numbers,input_floats.array);

	/*
	 * Write numbers as a thread
	 */

	// print_numbers(outputfile, numbers, nnumbers);
	threaded_print_numbers(outputfile, numbers, nnumbers);

	free(numbers);

	free_array(&input_floats);

}


