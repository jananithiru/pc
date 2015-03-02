/*
 ============================================================================
 Name        : mpi.c
 Author      : jthiru
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C
 ============================================================================
 */
#include "my_headers.h"
#include "mpi.h"



int main(int argc, char* argv[]) {
	int my_rank; /* rank of process */
	int p; /* number of processes */
	int source; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	char message[100]; /* storage for message */
	MPI_Status status; /* return status for receive */

	char input_file[LENGTH_FILENAME];
	char output_file[LENGTH_FILENAME];
	strcpy(input_file,argv[1]);
	strcpy(output_file,argv[2]);

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (my_rank == 0) {
		/* Read integers from file */
		array_d numbers;
		init_array(&numbers, INITIAL_SIZE);
		read_numbers(input_file, &numbers);
	}

	/*
	 * Call scatter here, check if they start only after read array completes
	 */

	/*
	 * Call gather here
	 */

	if (my_rank == 0) {
		/*
		 * Write Array here
		 */
//		print_numbers(output_file,numbers.array,numbers.used);
//		free_array(&numbers);

	}


	/*if (my_rank !=0){
		 create message
		sprintf(message, "Hello MPI World from process %d!", my_rank);
		dest = 0;
		 use strlen+1 so that '\0' get transmitted
		MPI_Send(message, strlen(message)+1, MPI_CHAR,
		   dest, tag, MPI_COMM_WORLD);
	}
	else{
		printf("Hello MPI World From process 0: Num processes: %d\n",p);
		for (source = 1; source < p; source++) {
			MPI_Recv(message, 100, MPI_CHAR, source, tag,
			      MPI_COMM_WORLD, &status);
			printf("%s\n",message);
		}
	}
*/
	/* shut down MPI */
	MPI_Finalize();
	return 0;
}
