# include "my_headers.h"

int is_sorted(float* const numbers, int size) {
	for (int i = 1; i < size; i++) {
		if (numbers[i] < numbers[i - 1]) {
			printf("ERROR: at loc %d, %f < %f \n", i, numbers[i],
					numbers[i - 1]);
			return 0;
		}
	}
	return 1;
}

int is_sort_correct(float* const  numbers_my_qs, float* const numbers_backup) {
	return 0;
}



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
	int nthreads = strtoimax(argv[2]);
	strcpy(outputfile, argv[3]);

	array_d numbers;
	init_array(&numbers, INITIAL_SIZE);

	read_thread_data read_thr;
	read_thr.filename = inputfile;
	read_thr.numbers = &numbers;

	int j = 0;
	int err;
	pthread_t tid[nthreads];

	err = pthread_create(&(tid[j]),NULL,
			&read_helper,(void *)&read_thr);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	else
		printf("\nRead Thread created successfully\n");

	j++;

	sleep(5);


	/*
	 * Sequential Quicksort
	 */
	quicksort(numbers.array,0,numbers.used-1);

	is_sorted(numbers.array,numbers.used);

	//is_sort_correct(numbers.array,numbers_backup);

	/*
	 * Parallel Quicksort
	 */

	/*
	 * Start time and end time
	 */

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

	sleep(5);

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


