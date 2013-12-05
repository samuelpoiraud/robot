################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tiny/TActManager.c \
../Tiny/TBall_inflater.c \
../Tiny/TCandle_color_sensor.c \
../Tiny/THammer.c \
../Tiny/TPlier.c 

OBJS += \
./Tiny/TActManager.o \
./Tiny/TBall_inflater.o \
./Tiny/TCandle_color_sensor.o \
./Tiny/THammer.o \
./Tiny/TPlier.o 

C_DEPS += \
./Tiny/TActManager.d \
./Tiny/TBall_inflater.d \
./Tiny/TCandle_color_sensor.d \
./Tiny/THammer.d \
./Tiny/TPlier.d 


# Each subdirectory must supply rules for building sources it contributes
Tiny/%.o: ../Tiny/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Summon Windows GCC C Compiler'
	arm-none-eabi-gcc -I../config -I../QS -I"../stm32f4xx" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


