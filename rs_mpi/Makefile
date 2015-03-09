EXECUTABLES = rs_mpi rs_openmp
DEBUG_FLAGS = -O3

all: $(EXECUTABLES)

rs_openmp: rs_openmp.c
	gcc -fopenmp -std=gnu99 $(DEBUG_FLAGS) -o$@ $^ -lrt

rs_mpi: rs_mpi.c
	mpicc -std=gnu99 $(DEBUG_FLAGS) -o$@ $^ -lrt

%.o : %.c %.h
	mpicc $(DEBUG_FLAGS) -c -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o mpe-*

