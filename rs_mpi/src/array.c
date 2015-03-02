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
	arr->array = (unsigned int *) malloc(sizeof(unsigned int) * initial_size);
	arr->size = initial_size;
	arr->used = 0;
}

/*
 * Function to insert every element into dynammic array
 */

void insert_array(array_d* arr, unsigned int num) {
	if (arr->used == arr->size) {
		arr->size = arr->size * 2;
		arr->array = (unsigned int *) realloc(arr->array,
				(sizeof(unsigned int) * arr->size));
	}
	arr->array[arr->used++] = num;
}

/*
 * Function to free dynammic array
 */

void free_array(array_d* arr) {
	free(arr->array);
	arr->array = NULL;
	arr->used = arr->size = 0;
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

	size_t nnumbers = numbers.used;
	printf("Hello MPI World From process 0 writing array");

	for (int i = 0; i < numbers.used; i++) {
		printf("%d", numbers.array[i]);
	}

	print_numbers(output_file, numbers.array, numbers.used);

	free_array(&numbers);

	/*init_array(&numbers, 5);

	 for (int i = 0; i < 1000; i++) {
	 insert_array(&numbers, (unsigned int) i);
	 }
	 printf("\n%d", numbers.array[992]);

	 free_array(&numbers);
	 */return 0;
}
