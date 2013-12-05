################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QS/QS_CapteurCouleurCW.c \
../QS/QS_DCMotor2.c \
../QS/QS_Step_motor.c \
../QS/QS_adc.c \
../QS/QS_ax12.c \
../QS/QS_buffer_fifo.c \
../QS/QS_buttons.c \
../QS/QS_can.c \
../QS/QS_can_over_uart.c \
../QS/QS_can_over_xbee.c \
../QS/QS_configCheck.c \
../QS/QS_external_it.c \
../QS/QS_global_vars.c \
../QS/QS_i2c.c \
../QS/QS_outputlog.c \
../QS/QS_ports.c \
../QS/QS_pwm.c \
../QS/QS_qei.c \
../QS/QS_rf.c \
../QS/QS_servo.c \
../QS/QS_spi.c \
../QS/QS_sys.c \
../QS/QS_timer.c \
../QS/QS_trap_handler.c \
../QS/QS_uart.c \
../QS/QS_watchdog.c \
../QS/QS_who_am_i.c 

S_UPPER_SRCS += \
../QS/startup_stm32f40xx.S 

OBJS += \
./QS/QS_CapteurCouleurCW.o \
./QS/QS_DCMotor2.o \
./QS/QS_Step_motor.o \
./QS/QS_adc.o \
./QS/QS_ax12.o \
./QS/QS_buffer_fifo.o \
./QS/QS_buttons.o \
./QS/QS_can.o \
./QS/QS_can_over_uart.o \
./QS/QS_can_over_xbee.o \
./QS/QS_configCheck.o \
./QS/QS_external_it.o \
./QS/QS_global_vars.o \
./QS/QS_i2c.o \
./QS/QS_outputlog.o \
./QS/QS_ports.o \
./QS/QS_pwm.o \
./QS/QS_qei.o \
./QS/QS_rf.o \
./QS/QS_servo.o \
./QS/QS_spi.o \
./QS/QS_sys.o \
./QS/QS_timer.o \
./QS/QS_trap_handler.o \
./QS/QS_uart.o \
./QS/QS_watchdog.o \
./QS/QS_who_am_i.o \
./QS/startup_stm32f40xx.o 

C_DEPS += \
./QS/QS_CapteurCouleurCW.d \
./QS/QS_DCMotor2.d \
./QS/QS_Step_motor.d \
./QS/QS_adc.d \
./QS/QS_ax12.d \
./QS/QS_buffer_fifo.d \
./QS/QS_buttons.d \
./QS/QS_can.d \
./QS/QS_can_over_uart.d \
./QS/QS_can_over_xbee.d \
./QS/QS_configCheck.d \
./QS/QS_external_it.d \
./QS/QS_global_vars.d \
./QS/QS_i2c.d \
./QS/QS_outputlog.d \
./QS/QS_ports.d \
./QS/QS_pwm.d \
./QS/QS_qei.d \
./QS/QS_rf.d \
./QS/QS_servo.d \
./QS/QS_spi.d \
./QS/QS_sys.d \
./QS/QS_timer.d \
./QS/QS_trap_handler.d \
./QS/QS_uart.d \
./QS/QS_watchdog.d \
./QS/QS_who_am_i.d 

S_UPPER_DEPS += \
./QS/startup_stm32f40xx.d 


# Each subdirectory must supply rules for building sources it contributes
QS/%.o: ../QS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Summon Windows GCC C Compiler'
	arm-none-eabi-gcc -I../config -I../QS -I"../stm32f4xx" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

QS/%.o: ../QS/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Summon Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


