/*
 * my_constants.h

 *
 *  Created on: Feb 15, 2015
 *      Author: jthiru
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "my_constants.h"

/*
 * dynammic array functions present in array.c
 */
typedef struct {
	int* array;
	size_t length;
	size_t capacity;
}d_array;

typedef struct timespec t_time;

double get_time_diff(const struct timespec* stop, const struct timespec* start);
void read_numbers(char const * const filename, d_array * const numbers);
void print_numbers(char const * const filename, int const * const numbers,
		size_t const nnumbers);
void print_time(double const seconds);
int is_sorted(int* const numbers, size_t size);

