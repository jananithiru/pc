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

/*
parallel quicksort top level:
instantiate parallelQuicksortHelper thread, and that's
basically it.
*/
void parallel_qs(float* numbers, size_t nnumbers, int level)
{
	void *status;

	//Want joinable threads (usually default).
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	qs_thread_data thr_data;
	thr_data.numbers = numbers;
	thr_data.left = 0;
	thr_data.right = nnumbers-1 ; // TODO: typecast
	thr_data.level = level ;

	pthread_t master_thread;
	int err = pthread_create(&master_thread, &attr, parallel_qs_helper,
						(void *) &thr_data);
	if (err)
	{
    	printf("ERROR; return code from pthread_create() is %d\n", err);
    	exit(-1);
    }

	//Now join the thread (wait, as joining blocks) and quit.
	pthread_attr_destroy(&attr);
	rc = pthread_join(theThread, &status);
	if (rc)
	{
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}
	//printf("Main: completed join with top thread having a status of %ld\n",
	//		(long)status);

}

/*
parallelQuicksortHelper
-if the level is still > 0, then partition and make
parallelQuicksortHelper threads to solve the left and
right-hand sides, then quit. Otherwise, call sequential.
*/
void *parallel_qs_helper(void *threadarg)
{
	int mid, t, rc;
	void *status;

	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	//fyi:
	//printf("Thread responsible for [%d, %d], level %d.\n",
	//		my_data->low, my_data->high, my_data->level);

	if (my_data->level <= 0 || my_data->low == my_data->high)
	{
		//We have plenty of threads, finish with sequential.
		quicksortHelper(my_data->lyst, my_data->low, my_data->high);
		pthread_exit(NULL);
	}

	//Want joinable threads (usually default).
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//Now we partition our part of the lyst.
	mid = partition(my_data->lyst, my_data->low, my_data->high);

	//At this point, we will create threads for the
	//left and right sides.  Must create their data args.
	struct thread_data thread_data_array[2];

	for (t = 0; t < 2; t ++)
	{
		thread_data_array[t].lyst = my_data->lyst;
		thread_data_array[t].level = my_data->level - 1;
	}
	thread_data_array[0].low = my_data->low;
	thread_data_array[0].high = mid-1;
	thread_data_array[1].low = mid+1;
	thread_data_array[1].high = my_data->high;

	//Now, instantiate the threads.
	//In quicksort of course, due to the transitive property,
	//no elements in the left and right sides of mid will have
	//to be compared again.
	pthread_t threads[2];
	for (t = 0; t < 2; t ++)
	{
		rc = pthread_create(&threads[t], &attr, parallelQuicksortHelper,
							(void *) &thread_data_array[t]);
		if (rc)
		{
    		printf("ERROR; return code from pthread_create() is %d\n", rc);
    		exit(-1);
    	}
	}

	pthread_attr_destroy(&attr);
	//Now, join the left and right sides to finish.
	for (t = 0; t < 2; t ++)
	{
		rc = pthread_join(threads[t], &status);
		if (rc)
		{
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
	}

	pthread_exit(NULL);
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


