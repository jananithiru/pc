#include "my_headers.h"

#define min(x,y) ((x) < (y) ? (x) :(y))

int choose_pivot(int left, int right) {
	int pivot_index = (int) (left + (right-left + 1)*(1.0*rand()/RAND_MAX));
	return pivot_index;
}

void swap(float numbers[], int i, int j) {
	float temp = numbers[i];
	numbers[i] = numbers[j];
	numbers[j] = temp;
}

int is_pthread_error(int return_code, const char function_name[]) {
	if (return_code) {
		printf("PTHREAD ERROR; Function: %s Return code %d\n", function_name,
				return_code);
	}
	return return_code;
}

int is_sort_correct(float* const numbers, float* const orig_numbers, size_t size) {
	//qsort(orig_numbers,);
	//diff_files();
	//sort orginal list and sorted list
	return 1;
}

int is_sorted(float* const numbers, size_t size) {
	for (int i = 1; i < size; i++) {
		if (numbers[i] < numbers[i - 1]) {
			printf("SORT ERROR; At numbers[%d] %f should not be before %f \n", i, numbers[i],
					numbers[i - 1]);
			return 0;
		}
	}
	return 1;
}

// Implementation specific to Wiki/Quicksort. Not a stable sort

int partition_qs(float numbers[], int left, int right) {
	int pivot_index = choose_pivot(left, right);
	float pivot_value = numbers[pivot_index];

	swap(numbers, pivot_index, right);

	int store_index = left;

	for (int i = left; i < right; i++) {
		if (numbers[i] < pivot_value) {
			swap(numbers, i, store_index);
			store_index++;
		}
	}
	swap(numbers, right, store_index);
	return store_index;
}

void parallel_qs(float numbers[], int size, int tlevel) {

	int return_code;
	void *join_thr_arg;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(&attr,500);

	qs_thread_data td;
	td.numbers = numbers;
	td.left = 0;
	td.right = size - 1;
	td.level = tlevel;

	pthread_t theThread;
	return_code = pthread_create(&theThread, &attr, parallel_qs_helper, (void *) &td);
	if(is_pthread_error(return_code,"pthread_create"))
				exit(-1);

	pthread_attr_destroy(&attr);
	return_code = pthread_join(theThread, &join_thr_arg);
	if(is_pthread_error(return_code,"pthread_join"))
				exit(-1);
}

void init_qs_thread_data_array(qs_thread_data* thread_data_array, int mid,
		qs_thread_data* thr_data) {
	for (int i = 0; i < 2; i++) {
		thread_data_array[i].numbers = thr_data->numbers;
		thread_data_array[i].level = thr_data->level - 1;
	}
	thread_data_array[0].left = thr_data->left;
	thread_data_array[0].right = mid - 1;
	thread_data_array[1].left = mid + 1;
	thread_data_array[1].right = thr_data->right;
}

void *parallel_qs_helper(void *thread_data) {

	void *join_thr_arg;

	qs_thread_data *thr_data;
	thr_data = (qs_thread_data *) thread_data;

	if (thr_data->level <= 0 || thr_data->left == thr_data->right) {
		qs_helper(thr_data->numbers, thr_data->left, thr_data->right);
		pthread_exit(NULL);
	} else if (thr_data->level >= TREE_DEPTH) {// only 2^treeDepth threads, after which we run in sequence
		qs_helper(thr_data->numbers, thr_data->left, thr_data->right);
		pthread_exit(NULL);
	} else {

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int mid = partition_qs(thr_data->numbers, thr_data->left, thr_data->right);

	qs_thread_data thread_data_array[2];
	init_qs_thread_data_array(&thread_data_array, mid, thr_data);

	pthread_t threads[2];
	for (int i = 0; i < 2; i++) {
		int return_code = pthread_create(&threads[i], &attr, parallel_qs_helper,
				(void *) &thread_data_array[i]);
		if(is_pthread_error(return_code,"pthread_create"))
			exit(-1);
	}

	pthread_attr_destroy(&attr);

	for (int i = 0; i < 2; i++) {
		int return_code = pthread_join(threads[i], &join_thr_arg);
		if(is_pthread_error(return_code,"pthread_join"))
			exit(-1);
	}

	pthread_exit(NULL);
	}
}

void quicksort(float numbers[], int size) {
	qs_helper(numbers, 0, size - 1);
}

void qs_helper(float numbers[], int left, int right) {
	if (left >= right)
		return;
	int pivot__index = partition_qs(numbers, left, right);
	qs_helper(numbers, left, pivot__index - 1);
	qs_helper(numbers, pivot__index + 1, right);
}

int qs_main() {

	int i = 0;
	float numbers[] = { 11, 123, 45, 167, 89, 2, 8, 1 };
	float nnumbers = 8;

	for (i = 0; i < nnumbers; i++)
		printf(" %f", numbers[i]);

	quicksort(numbers, nnumbers - 1);

	printf("\n\nSorted array is:  ");
	for (i = 0; i < nnumbers; ++i)
		printf(" %f ", numbers[i]);
	return 0;

}

