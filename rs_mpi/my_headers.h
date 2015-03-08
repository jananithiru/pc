#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "mpi.h"

#include "my_constants.h"

/*
 * dynammic array functions present in array.c
 */

typedef struct {
	int *array;
	size_t length;
	size_t capacity;
} array_d;

void init_array(array_d* arr, size_t initial_size);
int insert_array(array_d* arr, int num);
void free_array(array_d* arr);

/*
 * util.c headers
 */

void print_numbers(char const * const filename, int const * const numbers,
		size_t const nnumbers);

void read_numbers(char const * const filename, array_d * const numbers);


/*
 * sort.c headers
 */

int radix_sort(array_d* arr);
