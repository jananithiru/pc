################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/array.c \
../src/radix_sort.c \
../src/rs_mpi.c \
../src/util.c 

OBJS += \
./src/array.o \
./src/radix_sort.o \
./src/rs_mpi.o \
./src/util.o 

C_DEPS += \
./src/array.d \
./src/radix_sort.d \
./src/rs_mpi.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	/usr/lib64/openmpi/bin/mpicc -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


