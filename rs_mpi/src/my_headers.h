#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "my_constants.h"

#define _GNU_SOURCE

/*
 * dynammic array functions
 */

typedef struct {
	unsigned int *array;
	size_t used;
	size_t size;
} array_d;

void init_array(array_d* arr, size_t initial_size);
void insert_array(array_d* arr, unsigned int num);
void free_array(array_d* arr);

/*
 * print_numbers provided
 */

void print_numbers(
    char const * const filename,
    int const * const numbers,
    size_t const nnumbers);


