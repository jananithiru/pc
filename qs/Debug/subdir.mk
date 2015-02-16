################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../array.c \
../file_manager.c \
../pthreads.c \
../qs_threads.c \
../quicksort.c 

OBJS += \
./array.o \
./file_manager.o \
./pthreads.o \
./qs_threads.o \
./quicksort.o 

C_DEPS += \
./array.d \
./file_manager.d \
./pthreads.d \
./qs_threads.d \
./quicksort.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu99 -pthread -lrt -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


