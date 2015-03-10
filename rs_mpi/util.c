/*
 * util.c
 *
 *  Created on: Mar 9, 2015
 *      Author: jthiru
 */
#include "my_headers.h"


/**
 * @brief Read an array of integers from a file.
 *
 * @param filename The name of the file to read from
 * @param numbers The array of numbers will be stored here
 *
 */

void read_numbers(char const * const filename, d_array * const numbers) {

	FILE * fin;
	int temp;

	int scanresult;

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	while ((scanresult = fscanf(fin, "%d", &temp)) > 0) {
		insert_item(numbers, temp);
	}

	fclose(fin);
}

void print_numbers(char const * const filename, int const * const numbers,
		size_t const nnumbers) {
	size_t i;
	FILE * fout;

	/* open file */
	if ((fout = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	/* write numbers to fout */
	for (i = 0; i < nnumbers; ++i) {
		fprintf(fout, "%d\n", numbers[i]);
	}

	fclose(fout);
}

void print_array(int *a, const int n) {
  for (int i = 0; i < n; i++) {
    printf("%d\n", a[i]);
  }
}

void print_time(double const seconds) {
	printf("Sort Time: %0.04fs\n", seconds);
}

void error_logger(char* func_name, char* message) {
	fprintf(stderr, "\nERROR: func=%s()\tmsg=%s\n", message);
}

double get_time_diff(const struct timespec* stop, const struct timespec* start) {
	return (stop->tv_sec - start->tv_sec)
			+ (double) (stop->tv_nsec - start->tv_nsec) / (double) BILLION;
}
int is_sorted(int* const numbers, size_t size) {
	if ( size == 0)
		return 0;

	for (int i = 1; i < size; i++) {
		if (numbers[i] < numbers[i - 1]) {
			printf(
					"\nSORT ERROR; At index[%d] number %d should not be before %d \n",
					i, numbers[i - 1], numbers[i]);
			return 0;
		}
	}
	return 1;
}

int insert_item(d_array* list, int item) {
	if (list->length >= list->capacity) {
		size_t new_capacity = list->capacity * 2;
		int* temp = realloc(list->array, new_capacity * sizeof(int));
		if (!temp) {
			printf("ERROR: Could not realloc for size %d!\n",
					(int) new_capacity);
			return 0;
		}
		list->array = temp;
		list->capacity = new_capacity;
	}

	list->array[list->length++] = item;

	return 1;
}

/* usage : (x, 0, 1)
 * gets "r" group of bits from the rightmost bit as the starting point
 */
unsigned get_lsd_group_bits(unsigned num, int index, int rbits) {
  return (num >> index) & ~(~0 << rbits);
}
