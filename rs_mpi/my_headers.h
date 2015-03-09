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

typedef struct timespec timestamp_type;


// global constants definitions

#define LENGTH_FILENAME 64
#define INITIAL_SIZE 100000// 000
#define TREE_DEPTH 16
#define BILLION 1000000000L

#define b 32           // number of bits for integer
#define g 8            // group of bits for each scan
#define N b / g        // number of passes
#define B (1 << g)     // number of buckets, 2^g

// MPI tags constants, offset by max bucket to avoid collisions
#define COUNTS_TAG_NUM  B + 1
#define PRINT_TAG_NUM  COUNTS_TAG_NUM + 1
#define NUM_TAG PRINT_TAG_NUM + 1

/*radix sort definitions */

#define bits2 32
#define r 8            // groups as shown in section 9.6.2


#define NUM_PASSES bits / r
#define BUCKETS (1 << r)     // 2^r number of buckets,

#define _GNU_SOURCE

/*
 * dynammic array functions present in array.c
 */

typedef struct {
	int *array;
	size_t length;
	size_t capacity;
} array_d;

typedef struct {
	int* array;
	size_t length;
	size_t capacity;
}List;
