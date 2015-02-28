/*
 * array.c

 *
 *  Created on: Feb 15, 2015
 *      Author: jthiru
 */

#include "my_headers.h"

void init_array(array_d* arr, size_t initial_size) {
	arr->array = (unsigned int *) malloc(sizeof(unsigned int) * initial_size);
	arr->size = initial_size;
	arr->used = 0;
}

void insert_array(array_d* arr, unsigned int num) {
	if (arr->used == arr->size) {
		arr->size = arr->size * 2;
		arr->array = (unsigned int *) realloc(arr->array, (sizeof(unsigned int) * arr->size));
	}
	arr->array[arr->used++] = num;

}

void free_array(array_d* arr) {
	free(arr->array);
	arr->array = NULL;
	arr->used = arr->size = 0;
}

int array_main() {

	array_d numbers;

	init_array(&numbers, 5);

	for (int i = 0; i < 1000; i++) {
		insert_array(&numbers, (unsigned int) i);
	}
	printf("\n%d", numbers.array[992]);

	free_array(&numbers);

}

