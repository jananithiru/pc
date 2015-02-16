# include "my_headers.h"
//for parallel implementation
void parallelquiksort(float lyst[], int size, int tlevel);
void *parallelquiksortHelper(void *threadarg);
struct thread_data{
	double *lyst;
	int low;
	int high;
	int level;
};

void quiksort(float lyst[], int size)
{
	quiksortHelper(lyst, 0, size-1);
}

void quiksortHelper(float lyst[], int lo, int hi)
{
	if (lo >= hi) return;
	int b = partition2(lyst, lo, hi);
	quiksortHelper(lyst, lo, b-1);
	quiksortHelper(lyst, b+1, hi);
}

void swap(float lyst[], int i, int j)
{
	float temp = lyst[i];
	lyst[i] = lyst[j];
	lyst[j] = temp;
}

int partition2(float lyst[], int lo, int hi)
{
	int b = lo;
	int r = (int) (lo + (hi-lo + 1)*(1.0*rand()/RAND_MAX));
	float pivot = lyst[r];
	swap(lyst, r, hi);
	for (int i = lo; i < hi; i ++)
	{
		if (lyst[i] < pivot)
		{
			swap(lyst, i, b);
			b ++;
		}
	}
	swap(lyst, hi, b);
	return b;
}


/*
parallel quiksort top level:
instantiate parallelquiksortHelper thread, and that's
basically it.
*/
void parallelquiksort(float lyst[], int size, int tlevel)
{
	int rc;
	void *status;

	//Want joinable threads (usually default).
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//pthread function can take only one argument, so struct.
	struct thread_data td;
	td.lyst = lyst;
	td.low = 0;
	td.high = size - 1;
	td.level = tlevel;

	//The top-level thread.
	pthread_t theThread;
	rc = pthread_create(&theThread, &attr, parallelquiksortHelper,
						(void *) &td);
	if (rc)
	{
    	printf("ERROR; return code from pthread_create() is %d\n", rc);
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
parallelquiksortHelper
-if the level is still > 0, then partition2 and make
parallelquiksortHelper threads to solve the left and
right-hand sides, then quit. Otherwise, call sequential.
*/
void *parallelquiksortHelper(void *threadarg)
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
		quiksortHelper(my_data->lyst, my_data->low, my_data->high);
		pthread_exit(NULL);
	}

	//Want joinable threads (usually default).
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//Now we partition2 our part of the lyst.
	mid = partition2(my_data->lyst, my_data->low, my_data->high);

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
	//In quiksort of course, due to the transitive property,
	//no elements in the left and right sides of mid will have
	//to be compared again.
	pthread_t threads[2];
	for (t = 0; t < 2; t ++)
	{
		rc = pthread_create(&threads[t], &attr, parallelquiksortHelper,
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
	int nthreads = atoi(argv[2]); //strtoimax(argv[2]);
	strcpy(outputfile, argv[3]);

	array_d input_floats;
	init_array(&input_floats, INITIAL_SIZE);

	read_numbers(inputfile, &input_floats);

	size_t nnumbers = input_floats.used;
	float* numbers = (float *) malloc(nnumbers*sizeof(float));

	memcpy(numbers,input_floats.array,nnumbers*sizeof(float));

	struct timeval start, end;

	gettimeofday(&start, NULL);
	parallelquiksort(numbers, nnumbers, THREAD_LEVEL); // THREAD LEVEL
	gettimeofday(&end, NULL);

	double diff;
	diff = ((end.tv_sec * 1000000 + end.tv_usec)
			  - (start.tv_sec * 1000000 + start.tv_usec))/1000000.0;

	print_time(diff);

	is_sorted(numbers, nnumbers);

	print_numbers(outputfile, numbers, nnumbers);

	free(numbers);
	free_array(&input_floats);
	pthread_exit(NULL);
}


