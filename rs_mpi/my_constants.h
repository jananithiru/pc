/*
 * my_constants.h

 *
 *  Created on: Feb 15, 2015
 *      Author: jthiru
 */


/* defines the number of threads used to parallelize quicksort */
#define LENGTH_FILENAME 64
#define INITIAL_SIZE 1000// 000
#define TREE_DEPTH 16
#define BILLION 1000000000L


/*radix sort definitions */

#define bits 32
#define r 8            // groups as shown in section 9.6.2


#define NUM_PASSES bits / r
#define BUCKETS (1 << r)     // 2^r number of buckets,

// MPI tags constants, offset by max bucket to avoid collisions
#define COUNTS_TAG_NUM  BUCKETS + 1
#define PRINT_TAG_NUM  COUNTS_TAG_NUM + 1
#define NUM_TAG PRINT_TAG_NUM + 1


#define _GNU_SOURCE
