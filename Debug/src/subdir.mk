################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/DoubleLinkedList.c \
../src/asmdsl.c \
../src/main.c \
../src/mat.c \
../src/matdsl.c \
../src/parser.c 

OBJS += \
./src/DoubleLinkedList.o \
./src/asmdsl.o \
./src/main.o \
./src/mat.o \
./src/matdsl.o \
./src/parser.o 

C_DEPS += \
./src/DoubleLinkedList.d \
./src/asmdsl.d \
./src/main.d \
./src/mat.d \
./src/matdsl.d \
./src/parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -pedantic -pedantic-errors -Wall -c -fmessage-length=0 -ansi -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


