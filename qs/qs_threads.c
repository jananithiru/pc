# include "my_headers.h"

int main(int argc, char** argv) {

	if (argc != 4) {
		printf("USAGE: "
				"./qs_threads.c "
				"input_filename "
				"num_threads "
				"output_filename");
	}

	char inputfile[LENGTH_FILENAME], outputfile[LENGTH_FILENAME];

	strcpy(inputfile, argv[1]);
	int nthreads = atoi(argv[2]); //strtoimax(argv[2]);
	strcpy(outputfile, argv[3]);

	array_d numbers;
	init_array(&numbers, INITIAL_SIZE);

	read_numbers(inputfile, numbers.array);

	struct timespec t1, t2;
	clock_gettime(CLOCK_REALTIME, &t1);

	//parallel_qs(numbers.array, numbers.used, THREAD_LEVEL); // THREAD LEVEL
	quicksort(numbers.array, 0, numbers.used - 1);

	clock_gettime(CLOCK_REALTIME, &t2);
	double seconds = (double) ((t2.tv_sec + t2.tv_nsec * 1e-9)
			- (double) (t1.tv_sec + t1.tv_nsec * 1e-9));

	print_time(seconds);

	is_sorted(numbers.array, numbers.used);

	print_numbers(outputfile, numbers.array, numbers.used);

	free_array(&numbers);
}
