/*

 * qs_pthreads.c
// const char * const myPtr = &char_A;
 *  The third declares a pointer to a float where both the
 *  pointer value and the value being pointed at will not change
 *
 *  Created on: Feb 9, 2015
 *      Author: jthiru
 */
#include "my_headers.h"

void* print_helper(void *ptr) {
	print_thread_data *data;
	data = (print_thread_data *) ptr; /* type cast to a pointer to thdata */

	/* do the work */
	print_numbers(data->filename, data->numbers, data->nnumbers);

	pthread_exit(0); /* exit */
}

void* read_helper(void *ptr) {
	read_thread_data *data;
	data = (read_thread_data *) ptr; /* type cast to a pointer to thdata */

	/* do the work */
	read_numbers(data->filename, data->numbers);

	pthread_exit(0); /* exit */
}

void print_time(
    double const seconds)
{
  printf("Sort Time: %0.04fs\n", seconds);
}

void print_numbers(
		char const * const filename,
		float const * const numbers,
		size_t const nnumbers) {
	size_t i;
	FILE * fout;

	if ((fout = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	for (i = 0; i < nnumbers; ++i) {
		fprintf(fout, "%0.03f\n", numbers[i]);
	}

	fclose(fout);
}


void read_numbers(
		char const * const filename,
		array_d * const numbers) {

	FILE * fin;
	float temp;

	int scanresult;

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "error opening '%s'\n",filename);
		abort();
	}

	while ((scanresult = fscanf(fin, "%f", &temp)) > 0) {
		insert_array(numbers,temp);
	}

	fclose(fin);
}

int fm_main() {

	array_d numbers;
	init_array(&numbers, INITIAL_SIZE);

	const char input_file[] = "small_test.txt";
	const char output_file[] = "small_test.out";

	read_numbers(input_file, &numbers);
	print_numbers(output_file, numbers.array, numbers.used);

	free_array(&numbers);

}

