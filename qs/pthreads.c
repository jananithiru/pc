# include "my_headers.h"

int main_4(int argc, char** argv) {

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
	pthread_t tid[2];

	err = pthread_create(&(tid[0]),NULL,
			&read_helper,(void *)&read_thr);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	else
		printf("\nRead Thread created successfully\n");

	int rc;

	rc = pthread_join(tid[0], NULL);

	if (rc){
			fprintf(stderr, "Error joining thread\n");
			return 2;
	}

	/*
	 * Sequential or Parallel Quicksort
	 */


	struct timespec t1, t2;
	clock_gettime(CLOCK_REALTIME, &t1);

	//quicksort(numbers.array, 0, numbers.used - 1);
	//parallel_qs(numbers.array, numbers.used, THREAD_LEVEL); // THREAD LEVEL

	clock_gettime(CLOCK_REALTIME, &t2);
	double seconds = (double) ((t2.tv_sec + t2.tv_nsec * 1e-9)
			- (double) (t1.tv_sec + t1.tv_nsec * 1e-9));

	print_time(seconds);

	is_sorted(numbers.array, numbers.used);

	print_thread_data print_thr;
	print_thr.filename = outputfile;
	print_thr.numbers = numbers.array;
	print_thr.nnumbers = numbers.used;

	err = pthread_create(&(tid[1]), NULL,
			&print_helper,(void *)&print_thr);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	else
		printf("\n Write Thread created successfully\n");

	int rc2;
	rc2 = pthread_join(tid[1], NULL);

	if (rc2){
			fprintf(stderr, "Error joining thread\n");
			return 2;
	}

	free_array(&numbers);
	//pthread_exit();
	return 0;
}


