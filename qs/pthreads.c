# include "my_headers.h"

int main3(int argc, char** argv) {

	if (argc != 4) {
		printf("USAGE: "
				"./qs_threads.c "
				"input_filename "
				"num_threads "
				"output_filename");
	}

	char inputfile[LENGTH_FILENAME], outputfile[LENGTH_FILENAME];

	strcpy(inputfile, argv[1]);
	int nthreads = atoi(argv[2]);//strtoimax(argv[2]);
	strcpy(outputfile, argv[3]);

	array_d numbers;
	init_array(&numbers, INITIAL_SIZE);

	read_thread_data read_thr;
	read_thr.filename = inputfile;
	read_thr.numbers = &numbers;

	int j = 0;
	int err;
	pthread_t tid[NUM_OF_THREADS];

	err = pthread_create(&(tid[j]),NULL,
			&read_helper,(void *)&read_thr);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	else
		printf("\nRead Thread created successfully\n");

	j++;


	struct timespec t1, t2;
	clock_gettime(CLOCK_REALTIME, &t1);

	/*
	 * Sequential or Parallel Quicksort
	 */


//	quicksort(numbers.array, 0, numbers.used - 1);

	parallel_qs(numbers.array, numbers.used, THREAD_LEVEL); // THREAD LEVEL

	clock_gettime(CLOCK_REALTIME, &t2);
	double seconds = (double) ((t2.tv_sec + t2.tv_nsec * 1e-9)
			- (double) (t1.tv_sec + t1.tv_nsec * 1e-9));

	print_time(seconds);

	is_sorted(numbers.array, numbers.used);


	/*if (nthreads == 1) {
		struct timespec t1, t2;
		clock_gettime(CLOCK_REALTIME, &t1);

		quicksort(numbers.array, 0, numbers.used - 1);
		clock_gettime(CLOCK_REALTIME, &t2);
		double seconds = (double) ((t2.tv_sec + t2.tv_nsec * 1e-9)
				- (double) (t1.tv_sec + t1.tv_nsec * 1e-9));
		print_time(seconds);

		is_sorted(numbers.array, numbers.used);
		//is_sort_correct(numbers.array,numbers_backup);
	}else {
*/
	/*struct timespec t1, t2;
		clock_gettime(CLOCK_REALTIME, &t1);
		//Work goes here
		parallel_qs(numbers.array, numbers.used, THREAD_LEVEL); // THREAD LEVEL
		clock_gettime(CLOCK_REALTIME, &t2);
		double seconds = (double) ((t2.tv_sec + t2.tv_nsec * 1e-9)
				- (double) (t1.tv_sec + t1.tv_nsec * 1e-9));
		print_time(seconds);

		is_sorted(numbers.array, numbers.used);
		//is_sort_correct(numbers.array,numbers_backup);
//	}*/

	print_thread_data print_thr;
	print_thr.filename = outputfile;
	print_thr.numbers = numbers.array;
	print_thr.nnumbers = numbers.used;

	err = pthread_create(&(tid[j]), NULL,
			&print_helper,(void *)&print_thr);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	else
		printf("\n Write Thread created successfully\n");

	sleep(15);

	for (int j = 0; j < 2; j++) {
		if (pthread_join(tid[j], NULL)) {
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}
	}


	free_array(&numbers);
	//pthread_exit();
	return 0;
}


