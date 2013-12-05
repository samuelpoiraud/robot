################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Can_msg_processing.c \
../act_queue_utils.c \
../clock.c \
../main.c \
../maths_home.c \
../queue.c 

OBJS += \
./Can_msg_processing.o \
./act_queue_utils.o \
./clock.o \
./main.o \
./maths_home.o \
./queue.o 

C_DEPS += \
./Can_msg_processing.d \
./act_queue_utils.d \
./clock.d \
./main.d \
./maths_home.d \
./queue.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Summon Windows GCC C Compiler'
	arm-none-eabi-gcc -I../config -I../QS -I"../stm32f4xx" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


