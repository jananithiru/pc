/*
 * radix_sort.c
 *
 *  Created on: Mar 2, 2015
 *      Author: jthiru
 */

#include "my_headers.h"

// Compute j bits which appear k bits from the right in x
// Ex. to obtain rightmost bit of x call bits(x, 0, 1)
unsigned get_bits(unsigned x, int k, int j) {
  return (x >> k) & ~(~0 << j);
}

// Radix sort elements while communicating between other MPI processes
// a - array of elements to be sorted
// buckets - array of buckets, each bucket pointing to array of elements
// P - total number of MPI processes
// rank - rank of this MPI process
// n - number of elements to be sorted


int* radix_sort3(int *a, array_d* buckets, const int P, const int rank, int * n) {
  int count[BUCKETS][P];   // array of counts per bucket for all processes
  int l_count[BUCKETS];    // array of local process counts per bucket
  int l_BUCKETS = BUCKETS / P;   // number of local buckets per process
  int p_sum[l_BUCKETS][P]; // array of prefix sums

  // MPI request and status
  MPI_Request req;
  MPI_Status stat;

  for (int pass = 0; pass < NUM_PASSES; pass++) {          // each pass

    // init counts arrays
    for (int j = 0; j < BUCKETS; j++) {
      count[j][rank] = 0;
      l_count[j] = 0;
      buckets[j].length = 0;
    }

    // count items per bucket
    for (int i = 0; i < *n; i++) {
      unsigned int idx = get_bits(a[i], pass*r, r);
      count[idx][rank]++;
      l_count[idx]++;
      if (!insert_array(&buckets[idx], a[i])) {
        return NULL;
      }
    }

    // do one-to-all transpose
    for (int p = 0; p < P; p++) {
      if (p != rank) {
        // send counts of this process to others
        MPI_Isend(
            l_count,
            BUCKETS,
            MPI_INT,
            p,
            COUNTS_TAG_NUM,
            MPI_COMM_WORLD,
            &req);
      }
    }

    // receive counts from others
    for (int p = 0; p < P; p++) {
      if (p != rank) {
        MPI_Recv(
            l_count,
            BUCKETS,
            MPI_INT,
            p,
            COUNTS_TAG_NUM,
            MPI_COMM_WORLD,
            &stat);

        // populate counts per bucket for other processes
        for (int i = 0; i < BUCKETS; i++) {
          count[i][p] = l_count[i];
        }
      }
    }

    // calculate new size based on values received from all processes
    int new_size = 0;
    for (int j = 0; j < l_BUCKETS; j++) {
      int idx = j + rank * l_BUCKETS;
      for (int p = 0; p < P; p++) {
        p_sum[j][p] = new_size;
        new_size += count[idx][p];
      }
    }

    // reallocate array if newly calculated size is larger
    if (new_size > *n) {
      int* temp = realloc(a, new_size*sizeof(int));
      if (!a) {
        if (rank == 0) {
          printf("ERROR: Could not realloc for size %d!\n", new_size);
        }
        return NULL;
      }
      // reassign pointer back to original
      a = temp;
    }

    // send keys of this process to others
    for (int j = 0; j < BUCKETS; j++) {
      int p = j / l_BUCKETS;   // determine which process this buckets belongs to
      int p_j = j % l_BUCKETS; // transpose to that process local bucket index
      if (p != rank && buckets[j].length > 0) {
        MPI_Isend(
            buckets[j].array,
            buckets[j].length,
            MPI_INT,
            p,
            p_j,
            MPI_COMM_WORLD,
            &req);
      }
    }

    // receive keys from other processes
    for (int j = 0; j < l_BUCKETS; j++) {
      // transpose from local to global index
      int idx = j + rank * l_BUCKETS;
      for (int p = 0; p < P; p++) {

        // get bucket count
        int b_count = count[idx][p];
        if (b_count > 0) {

          // point to an index in array where to insert received keys
          int *dest = &a[p_sum[j][p]];
          if (rank != p) {
            MPI_Recv(
                dest,
                b_count,
                MPI_INT,
                p,
                j,
                MPI_COMM_WORLD,
                &stat);

          } else {
            // is same process, copy from buckets to our array
            memcpy(dest, &buckets[idx].array[0], b_count*sizeof(int));
          }
        }
      }
    }

    // update new size
    *n = new_size;
  }

  return a;
}


int is_sorted(int* const numbers, size_t size) {
	for (int i = 1; i < size; i++) {
		if (numbers[i] < numbers[i - 1]) {
			printf("\nSORT ERROR; At index[%d] number %d should be before %d \n",
					i, numbers[i], numbers[i-1]);
			return 0;
		}
	}
	return 1;
}

int radix_sort(array_d* arr) {

	//array_d* arr = *array;

	int* temp;
	int* list = arr->array;
	int size = arr->length;

	const int radix = 8;
	const int slots = 1 << radix;

	int* matches;
	int* slotpos;
	int shift;
	int mask;
	int val;
	int i;

	/* Allocate structures */
	matches = malloc(slots * sizeof(int));
	if (matches == NULL) {
		return -1;
	}

	slotpos = malloc(slots * sizeof(int));
	if (slotpos == NULL) {
		free(matches);
		return -1;
	}

	temp = malloc(size * sizeof(int));
	if (temp == NULL) {
		free(matches);
		free(slotpos);
		return -1;
	}

	/* Perform the sort */
	for (shift = 0; shift < 32; shift += radix) {
		mask = (slots << shift) - 1;

		/* Populate temporary list */
		for (i = 0; i < size; ++i)
			temp[i] = list[i];

		/* Count matches for each value */
		for (i = 0; i < slots; ++i)
			matches[i] = 0;
		for (i = 0; i < size; ++i) {
			val = list[i] & mask;
			val = val >> shift;
			matches[val] += 1;
		}

		/* Determine positions for each slot */
		slotpos[0] = 0;
		for (i = 1; i < slots; ++i)
			slotpos[i] = slotpos[i - 1] + matches[i - 1];

		/* Copy elements to new array */
		for (i = 0; i < size; ++i) {
			val = temp[i] & mask;
			val = val >> shift;
			list[slotpos[val]] = temp[i];
			++slotpos[val];
		}
	}

	/* Free structures */
	free(temp);
	free(matches);
	free(slotpos);
	return 0;
}



int test1() {
	array_d numbers;
	init_array(&numbers, INITIAL_SIZE);

	insert_array(&numbers, 4);
	insert_array(&numbers, 3);
	insert_array(&numbers, 2);
	insert_array(&numbers, 1);

	radix_sort(&numbers);

	printf("hello");
	for (int i = 0; i < numbers.length; i++) {
		printf("\n%d", numbers.array[i]);
		fflush(stdout);
	}
	free_array(&numbers);
	return 0;
}

int test0() {
	array_d numbers;

	init_array(&numbers, INITIAL_SIZE);

	read_numbers("100.txt", &numbers);

	/*for (int i = 0; i < 1000; i++) {
	 insert_array(&numbers, (int) i);
	 }
	 */
	/*for (int i = 0; i < 1000; i++) {
	 printf("\n%d", numbers.array[i]);
	 }
	 */
	radix_sort(&numbers);

	is_sorted(numbers.array, numbers.length);

	printf("\n\nSorted array is: %d ", is_sorted(numbers.array, numbers.length));

	for (int i = 0; i < numbers.length; i++) {
		printf("\n%d", numbers.array[i]);
	}

	free_array(&numbers);

	return 0;
}
int kmain() {

	test1();
	return 0;

}

