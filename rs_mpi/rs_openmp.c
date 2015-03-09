
/*
 ============================================================================
 Name        : rs_openmp.c
 Author      : jthiru
 Version     :
 Copyright   : Your copyright notice
 Description : Hello OpenMP World in C
 ============================================================================
 */
#include "my_constants.h"
#include "omp.h"

#define b 32           // number of bits for integer, usually four words
#define g 8            // group of bits for each scan, one word
#define B (1 << g)     // number of buckets, 2^g


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

// Compute j bits which appear k bits from the right in x
// Ex. to obtain rightmost bit of x call bits(x, 0, 1)
unsigned bits(unsigned x, int k, int j) {
  return (x >> k) & ~(~0 << j);
}

void radix_sort(int *a, const int n) {
  int* t = malloc(n*sizeof(int));     // temp array used for sorting
  int count[B];                       // array of counts per bucket

  for (int pass = 0; pass < b/g; pass++) {       // each pass
    for (int j = 0; j < B; j++) count[j] = 0;    // init counts array
    for (int i = 0; i < n; i++) {
      count[bits(a[i], pass*g, g)]++;            // count keys per bucket
    }
    for (int j = 1; j < B; j++) {
      count[j] = count[j-1] + count[j];          // compute prefix sum
    }
    for (int i = n-1; i >= 0; i--) {
      int idx = --count[bits(a[i], pass*g, g)];
      t[idx] = a[i];                             // transpose to temp array
    }
    for (int i = 0; i < n; i++) a[i] = t[i];     // copy back to master
  }
  free(t);
}

int main(int argc, char** argv)
{
  int print_results = 0;
  int nthreads ;

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];

	strcpy(input_file, argv[1]);
	nthreads = atoi(argv[2]);
	strcpy(output_file, argv[3]);


	timestamp_type start, stop;

  // initialize vars and allocate memory
  const int n = atoi(argv[2]);
  int* a = malloc(sizeof(int) * n);

  // initialize local array
  if (init_array(argv[1], 0, n, &a[0]) != EXIT_SUCCESS) {
    printf("File %s could not be opened!\n", argv[1]);
    return EXIT_FAILURE;
  }

  int tid = omp_get_thread_num();
  // take a timestamp before the sort starts
  if (tid == 0) {
  		clock_gettime(CLOCK_REALTIME, &start);
  		//free(numbers.array);
  	}

  // sort elements
  radix_sort(&a[0], n);

  if (tid == 0) {
  		clock_gettime(CLOCK_REALTIME, &stop);
  		print_time(get_time_diff(&stop, &start));
  	}

  // print sorted resutls
  if (1) {
    print_array(&a[0], n);
    printf("\nSorted=%d\n", is_sorted(a, n));
  }

  // release resources no longer used
  free(a);

  return 0;
}
