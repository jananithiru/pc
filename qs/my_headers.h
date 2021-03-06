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
	float *array;
	size_t used;
	size_t size;
} array_d;

void init_array(array_d* arr, size_t initial_size);
void insert_array(array_d* arr, float num);
void free_array(array_d* arr);



/*
 * file manager functions
 */

typedef struct {
	char * filename;
	array_d * numbers;
}read_thread_data;

typedef struct {
	char *  filename;
	float *  numbers;
	size_t nnumbers;
}print_thread_data;

void* read_helper(void *ptr);
void* print_helper(void *ptr);

void print_numbers(
    char const * const filename,
    float const * const numbers,
    size_t const nnumbers);

void read_numbers(
		char const * const filename,
		array_d * const numbers);

/*
 * quicksort functions
 */

//for regular implementation
void quicksort(float numbers[], int size);
void qs_helper(float numbers[], int left, int right);

//for parallel implementation
void parallel_qs(float numbers[], int size, int tlevel);
void *parallel_qs_helper(void *thread_data);

int is_sorted(float* const numbers, size_t size);

typedef struct {
	float *numbers;
	int left;
	int right;
	int level;
}qs_thread_data;


