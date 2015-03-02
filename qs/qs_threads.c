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

int extract_thread_level(int nthreads);

int main2(int argc, char** argv) {

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

	int thread_level = extract_thread_level(nthreads);

	array_d input_floats;
	init_array(&input_floats, INITIAL_SIZE);

	/*
	 * Read numbers as a thread
	 */

	//read_numbers(inputfile, &input_floats); // MUST FREE

	threaded_read_numbers(inputfile, &input_floats);

	size_t nnumbers = input_floats.used;

	float* numbers = copy_numbers(nnumbers, &input_floats); // MUST FREE

	clock_gettime(CLOCK_REALTIME, &start);

	/*
	 * Run parallel quicksort.
	 * Set nthreads =0 for sequential
	 * 1 for 2 threads , 2 for 4 threads , 3 for 8 threads and so on.
	 */

	parallel_qs(numbers, nnumbers, thread_level);

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

int extract_thread_level(int nthreads) {
	int thread_level;
	if (nthreads <= 1)
		thread_level = 0;
	else if (nthreads == 2)
		thread_level = 1;
	else if (nthreads == 4)
		thread_level = 2;
	else if (nthreads == 8)
		thread_level = 3;
	else if (nthreads == 16)
		thread_level = 4;
	else if (nthreads == 32)
		thread_level = 5;
	else
		thread_level = 6;
	return thread_level;
}
