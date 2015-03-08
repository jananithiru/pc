/*
 * util.c
 *
 *  Created on: Feb 28, 2015
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

void read_numbers(char const * const filename, array_d * const numbers) {

	FILE * fin;
	int temp;

	int scanresult;

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "error opening '%s'\n", filename);
		abort();
	}

	while ((scanresult = fscanf(fin, "%d", &temp)) > 0) {
		insert_array(numbers, temp);
	}

	fclose(fin);
}

/**
 * @brief Write an array of integers to a file.
 *
 * @param filename The name of the file to write to.
 * @param numbers The array of numbers.
 * @param nnumbers How many numbers to write.
 */
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

/**
 * @brief Output the seconds elapsed while sorting. This excludes input and
 *        output time. This should be wallclock time, not CPU time.
 *
 * @param seconds Seconds spent sorting.
 */
void print_time(double const seconds) {
	printf("Sort Time: %0.04fs\n", seconds);
}

void error_logger(char* func_name, char* message) {
	fprintf(stderr, "\nERROR: func=%s()\tmsg=%s\n", message);
	MPI_Finalize();
}
double get_time_diff(const struct timespec* stop,
		const struct timespec* start) {
	/*double diff = ((end->tv_sec *BILLION + end->tv_nsec )
			- (start->tv_sec * BILLION + start->tv_nsec))/BILLION;*/
	return ( stop->tv_sec - start->tv_sec )
	          + (double)( stop->tv_nsec - start->tv_nsec )/(double) BILLION;
}

