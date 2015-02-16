# include "my_headers.h"

double get_time_diff(const struct timeval* end,
		const struct timeval* start) {
	double diff = ((end->tv_sec * 1000000 + end->tv_usec)
			- (start->tv_sec * 1000000 + start->tv_usec)) / 1000000.0;
	return diff;
}

float* copy_numbers(size_t nnumbers, const array_d* input_floats) {
	float* numbers = (float*) malloc(nnumbers * sizeof(float));
	memcpy(numbers, input_floats->array, nnumbers * sizeof(float));
	return numbers;
}

int main(int argc, char** argv) {

	struct timeval start, end;
	char inputfile[LENGTH_FILENAME], outputfile[LENGTH_FILENAME];

	if (argc != 4) {
		printf("USAGE: "
				"./qs_threads.c "
				"input_filename "
				"num_threads "
				"output_filename");
	}

	strcpy(inputfile, argv[1]);
	int nthreads = atoi(argv[2]); //strtoimax(argv[2]);
	strcpy(outputfile, argv[3]);

	array_d input_floats;
	init_array(&input_floats, INITIAL_SIZE);

	read_numbers(inputfile, &input_floats); // MUST FREE

	size_t nnumbers = input_floats.used;

	float* numbers = copy_numbers(nnumbers, &input_floats); // MUST FREE

	gettimeofday(&start, NULL);

	parallel_qs(numbers, nnumbers, nthreads);

	gettimeofday(&end, NULL);

	print_time(get_time_diff(&end, &start));

	is_sorted(numbers, nnumbers);

	is_sort_correct(numbers,input_floats.array);

	print_numbers(outputfile, numbers, nnumbers);

	free(numbers);

	free_array(&input_floats);

	pthread_exit(NULL);
}


