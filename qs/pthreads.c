# include "my_headers.h"



int main(int argc, char** argv) {


	char inputfile[LENGTH_FILENAME], outputfile[LENGTH_FILENAME];
	int nthreads;
	array_d numbers;

	init_array(&numbers, INITIAL_SIZE);

	if (argc != 4) {
		printf("USAGE: "
				"./qs_threads.c "
				"input_filename "
				"num_threads "
				"output_filename");
	}

	strcpy(inputfile, argv[1]);
	nthreads = argv[2];
	strcpy(outputfile, argv[3]);

	//	read_numbers(inputfile, &numbers);
	//	print_numbers(outputfile, numbers.array, numbers.used);

	read_thread_data read_thr;
	read_thr.filename = inputfile;
	read_thr.numbers = &numbers;

	int i = 0;
	int err;
	pthread_t tid[2];

	err = pthread_create(&(tid[i]),NULL,
			&read_helper,(void *)&read_thr);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
			printf("\n1 Thread created successfully\n");

	sleep(15);

	print_thread_data print_thr;
	print_thr.filename = outputfile;
	print_thr.numbers = numbers.array;
	print_thr.nnumbers = numbers.used;

	err = pthread_create(&(tid[i]), NULL,
			&print_helper,(void *)&print_thr);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
			printf("\n 2 Thread created successfully\n");

	sleep(5);

	free_array(&numbers);

	return 0;
}

