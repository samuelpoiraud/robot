################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Krusty/KActManager.c \
../Krusty/KBall_launcher.c \
../Krusty/KBall_sorter.c \
../Krusty/KFruit.c \
../Krusty/KLift.c \
../Krusty/KPlate.c \
../Krusty/template.c 

OBJS += \
./Krusty/KActManager.o \
./Krusty/KBall_launcher.o \
./Krusty/KBall_sorter.o \
./Krusty/KFruit.o \
./Krusty/KLift.o \
./Krusty/KPlate.o \
./Krusty/template.o 

C_DEPS += \
./Krusty/KActManager.d \
./Krusty/KBall_launcher.d \
./Krusty/KBall_sorter.d \
./Krusty/KFruit.d \
./Krusty/KLift.d \
./Krusty/KPlate.d \
./Krusty/template.d 


# Each subdirectory must supply rules for building sources it contributes
Krusty/%.o: ../Krusty/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Summon Windows GCC C Compiler'
	arm-none-eabi-gcc -I../config -I../QS -I"../stm32f4xx" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


