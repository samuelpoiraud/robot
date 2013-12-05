################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QS/impl/QS_uart_stm32f4.c 

OBJS += \
./QS/impl/QS_uart_stm32f4.o 

C_DEPS += \
./QS/impl/QS_uart_stm32f4.d 


# Each subdirectory must supply rules for building sources it contributes
QS/impl/%.o: ../QS/impl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Summon Windows GCC C Compiler'
	arm-none-eabi-gcc -I../config -I../QS -I"../stm32f4xx" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


