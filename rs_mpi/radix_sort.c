/*
 * radix_sort.c
 *
 *  Created on: Mar 2, 2015
 *      Author: jthiru
 */

#include "my_headers.h"

int is_sorted(int* const numbers, size_t size) {
	for (int i = 1; i < size; i++) {
		if (numbers[i] < numbers[i - 1]) {
			printf("\nSORT ERROR; At numbers[%d] %d should not be before %d \n",
					i, numbers[i], numbers[i - 1]);
			return 0;
		}
	}
	return 1;
}

int radix_sort(array_d* arr) {

	int* temp;
	int* list = arr->array;
	int size = arr->used;

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

int rs_main() {

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

	is_sorted(numbers.array, numbers.used);

	printf("\n\nSorted array is: %d ",is_sorted(numbers.array, numbers.used));

	for (int i = 0; i < 100; i++) {
			printf("\n%d", numbers.array[i]);
		}

	free_array(&numbers);

	return 0;

}

