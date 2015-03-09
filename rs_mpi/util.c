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

void read_numbers(char const * const filename, List * const numbers) {

	FILE * fin;
	int temp;

	int scanresult;

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	while ((scanresult = fscanf(fin, "%d", &temp)) > 0) {
		add_item(numbers, temp);
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

void print_time(double const seconds) {
	printf("Sort Time: %0.04fs\n", seconds);
}

void error_logger(char* func_name, char* message) {
	fprintf(stderr, "\nERROR: func=%s()\tmsg=%s\n", message);
}
double get_time_diff(const struct timespec* stop, const struct timespec* start) {
	/*double diff = ((end->tv_sec *BILLION + end->tv_nsec )
	 - (start->tv_sec * BILLION + start->tv_nsec))/BILLION;*/
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

int add_item(List* list, int item) {
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
// Compute j bits which appear k bits from the right in x
// Ex. to obtain rightmost bit of x call bits(x, 0, 1)
unsigned bits(unsigned x, int k, int j) {
  return (x >> k) & ~(~0 << j);
}


/* usage : (x, 0, 1)
 * gets rightmost bit of x
 */

int get_bits(unsigned x, int k, int j) {
	return (x >> k) & ~(~0 << j);
}
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
// add item to a dynamic array encapsulated in a structure

/*
 * Initialize dynammic array
 */
void init_d_array(array_d* arr, size_t initial_size) {
	arr->array = (int *) malloc(sizeof(int) * initial_size);
	arr->capacity = initial_size;
	arr->length = 0;
}

/*
 * Function to free dynammic array
 */

void free_array(array_d* arr) {
	free(arr->array);
	arr->array = NULL;
	arr->length = arr->capacity = 0;
}


void usage() {
  fprintf(stderr, "Incorrect usage!\n");
  fprintf(stderr, "Usage: radix_sort [f] [n]\n");
  fprintf(stderr, "  [f] - input file to be sorted\n");
  fprintf(stderr, "  [n] - number of elements in the file\n");
  fprintf(stderr, "  [r] - print sorted results 0/1, 0 by default\n");
}

void print_array(int *a, const int n) {
  for (int i = 0; i < n; i++) {
    printf("%d\n", a[i]);
  }
}

int init_array(char* file, const int begin, const int n, int *a) {

  // open file in read-only mode and check for errors
  FILE *file_ptr;
  file_ptr = fopen(file, "r");
  if (file_ptr == NULL) {
    return EXIT_FAILURE;
  }

  // read n numbers from a file into array a starting at begin position
  int skip;

  // first skip to the begin position
  for (int i = 0; i < begin; i++) {
    int s = fscanf(file_ptr, "%d", &skip);
  }
  // then read numbers into array a
  for (int i = 0; i < n; i++) {
    int s = fscanf(file_ptr, "%d", &a[i]);
  }

  return EXIT_SUCCESS;
}
