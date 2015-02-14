/*
 * qs_pthreads.c

 *
 *  Created on: Feb 9, 2015
 *      Author: jthiru
 */
#define _GNU_SOURCE
#include "myHeaders.h"


/**
* @brief Write an array of floats to a file.
*
* @param filename The name of the file to write to.
* @param numbers The array of numbers.
* @param nnumbers How many numbers to write.
*/
static void print_numbers(
    char const * const filename,
    float const * const numbers,
    size_t const nnumbers)
{
  size_t i;
  FILE * fout;

  /* open file */
  if((fout = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "error opening '%s'\n", filename);
    abort();
  }

  /* write numbers to fout */

  for(i = 0; i < nnumbers; ++i) {
    fprintf(fout, "%0.03f\n", numbers[i]);
  }

  fclose(fout);
}

/**
* @brief Read a file of floats into an array
*
* @param filename The name of the file to write to.
* @param numbers The array to store the floats.
* @param nnumbers The size of the array
*/

static void read_numbers(
		char const * const filename,
		    float * const numbers,
		    size_t* nnumbers)
{
   size_t i = 0 ;
   FILE * fin ;
   int scanresult;

   if((fin = fopen(filename, "r")) == NULL) {
     fprintf(stderr, "error opening '%s'\n", filename);
     abort();
   }

   while((scanresult=fscanf(fin,"%f",numbers+i)) > 0) {
   	   i++;
   }

   *nnumbers = i;

   fclose(fin);
}

void readFloatsIntoArray () {

	FILE* fin ;
	float number[3] ;
	int i=0;
	int x=0;
	int scanresult;

	if((fin = fopen("3small_test.txt", "r")) == NULL) {
	     fprintf(stderr, "error opening '%s'\n");
	     abort();
	   }

	for (i=0; i<x; i++) {
		printf("x%d %f\n",i,number[i]);
	}


/*	printf("a%d-",fscanf(fin,"%f", number));
	printf("aa2%d-",fscanf(fin,"%f", number+1));
	printf("aaa%d-",fscanf(fin,"%f", number+2));
	printf("b-%d--",fscanf(fin,"%d", x));
	printf("b-%d--",fscanf(fin,"%d", x));

	for (i=0; i<3; i++) {
		printf("x%d %f\n",i,number[i]);
	}*/



	fclose(fin);
}

int main () {
	main2();
	return 0;
}

int main2() {

	// Step 1:
	//Reads the input file of floats seperated by new line
	//Prints output to file in non decreasing order.
	//Use output function given
	// e.g. small_test.txt 2 small_test.out
	// Read in a chunk of numbers and then make the threads execute them

	int i; //loop variables
	int NUM = 100;
	float numbers[100] = {0.0} ;

	size_t nnumbers ;
	const char input_file[] = "small_test.txt";
	const char output_file[] = "small_test.out";

	read_numbers(input_file, numbers, &nnumbers);
	print_numbers(output_file, numbers, nnumbers);

}

// const char * const myPtr = &char_A; The third declares a pointer to a float where both the pointer value and the value being pointed at will not change
