/*
 * array.c

 *
 *  Created on: Feb 15, 2015
 *      Author: jthiru
 */

#include "my_headers.h"

/*
 * Initialize dynammic array
 */
void init_array(array_d* arr, size_t initial_size) {
	arr->array = (int *) malloc(sizeof(int) * initial_size);
	arr->capacity = initial_size;
	arr->length = 0;
}

/*
 * Function to insert every element into dynammic array
 */

int insert_array(array_d* arr, int num) {
	if (arr->length == arr->capacity) {
		size_t c = arr->capacity * 2;
		int* t = (int *) realloc(arr->array, (sizeof(int) * c));
		if (!t) {
			printf("ERROR: Could not realloc for size %d!\n", (int) c);
			return 0;
		}
		arr->array = t;
		arr->capacity = c;
	}
	arr->array[arr->length++] = num;
	return 1;
}

/*
 * Function to free dynammic array
 */

void free_array(array_d* arr) {
	free(arr->array);
	arr->array = NULL;
	arr->length = arr->capacity = 0;
}

/*
 * Function to test dynammic array
 */
int array_main(int argc, char* argv[]) {

	array_d numbers;

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];
	strcpy(input_file, argv[1]);
	strcpy(output_file, argv[2]);

	init_array(&numbers, INITIAL_SIZE);

	read_numbers(input_file, &numbers);

	size_t nnumbers = numbers.length;
	printf("Hello MPI World From process 0 writing array");

	for (int i = 0; i < numbers.length; i++) {
		printf("%d", numbers.array[i]);
	}

	print_numbers(output_file, numbers.array, numbers.length);

	free_array(&numbers);

	/*init_array(&numbers, 5);

	 for (int i = 0; i < 1000; i++) {
	 insert_array(&numbers, (int) i);
	 }
	 printf("\n%d", numbers.array[992]);

	 free_array(&numbers);
	 */return 0;
}
