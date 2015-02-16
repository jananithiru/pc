#include "my_headers.h"
#define SIZE 8

#define min(x,y) ((x) < (y) ? (x) :(y))
int iter=0;



int choosePivot(float* numbers, int leftIndex, int rightIndex) {
	return leftIndex;
}


int partition( float* a, int l, int r) {
   int i, j;
   float pivotValue, t;

   pivotValue = a[l];

   i = l; j = r+1;

   while(1)
   {
   	do ++i; while( a[i] <= pivotValue && i <= r );
   	do --j; while( a[j] > pivotValue );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}

void quicksort(float* numbers, int leftIndex, int rightIndex) {

	if ( leftIndex >= rightIndex )
		return ;

	int pivotIndex = partition(numbers, leftIndex,rightIndex);

	if (leftIndex < pivotIndex){
		quicksort(numbers,leftIndex,pivotIndex);
	}
	if (pivotIndex + 1 < rightIndex)
		quicksort(numbers,pivotIndex+1,rightIndex);

}

int is_sorted(float* const numbers, size_t size) {
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

void parallel_qs(float* numbers, size_t nnumbers, int level)
{
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

	pthread_attr_destroy(&attr);
	void *status;
	err = pthread_join(master_thread, &status); //void *status;

	if (err)
	{
		printf("ERROR; return code from pthread_join() is %d\n", err);
		exit(-1);
	}
}

void *parallel_qs_helper(void *thread_data)
{
	int mid, t, rc;
	void *status;

	qs_thread_data *thr_data;
	thr_data = (qs_thread_data *) thread_data;

	if (thr_data->level <= 0 || thr_data->left == thr_data->right)
	{
		quicksort(thr_data->numbers, thr_data->left, thr_data->right);
		pthread_exit(NULL);
	}

	//Want joinable threads (usually default).
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	mid = partition(thr_data->numbers, thr_data->left, thr_data->right);

	qs_thread_data thread_data_array[2];

	for (t = 0; t < 2; t ++)
	{
		thread_data_array[t].numbers = thr_data->numbers;
		thread_data_array[t].level = thr_data->level - 1;
	}

	thread_data_array[0].left = thr_data->left;
	thread_data_array[0].right = mid-1;
	thread_data_array[1].left = mid+1;
	thread_data_array[1].right = thr_data->right;

	pthread_t threads[2];

	for (t = 0; t < 2; t ++)
	{
		rc = pthread_create(&threads[t], &attr, parallel_qs_helper,
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



int qs_main() {

	int i = 0; 
	float numbers[] = {11,123,45,167,89,2,8,1};
	float nnumbers = SIZE;

	for (i=0; i < nnumbers; i++)
			printf (" %f",numbers[i]);


	quicksort(numbers,0,nnumbers-1);

	printf("\n\nSorted array is:  ");
			for(i = 0; i < nnumbers; ++i)
				printf(" %f ", numbers[i]);
	return 0;

}

