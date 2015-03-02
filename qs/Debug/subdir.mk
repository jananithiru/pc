################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Copy\ of\ k.c \
../array.c \
../file_manager.c \
../qs_threads.c \
../quick_sort.c 

OBJS += \
./Copy\ of\ k.o \
./array.o \
./file_manager.o \
./qs_threads.o \
./quick_sort.o 

C_DEPS += \
./Copy\ of\ k.d \
./array.d \
./file_manager.d \
./qs_threads.d \
./quick_sort.d 


# Each subdirectory must supply rules for building sources it contributes
Copy\ of\ k.o: ../Copy\ of\ k.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu99 -pthread -lrt -MMD -MP -MF"Copy of k.d" -MT"Copy\ of\ k.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu99 -pthread -lrt -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


