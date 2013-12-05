################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Tiny/subdir.mk
-include QS/impl/subdir.mk
-include QS/subdir.mk
-include Krusty/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_LIST += \
Actionneur.lst \

SECONDARY_SIZE += \
Actionneur.siz \


# All Target
all: Actionneur.elf secondary-outputs

# Tool invocations
Actionneur.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Summon Windows GCC C Linker'
	arm-none-eabi-gcc -T""../QS/stm32f4xx_flash.ld"" -nostartfiles -L"../stm32f4xx" -Wl,-Map,Actionneur.map -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "Actionneur.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

Actionneur.lst: Actionneur.elf
	@echo 'Invoking: ARM Summon Windows GNU Create Listing'
	arm-none-eabi-objdump -h -S Actionneur.elf > "Actionneur.lst"
	@echo 'Finished building: $@'
	@echo ' '

Actionneur.siz: Actionneur.elf
	@echo 'Invoking: ARM Summon Windows GNU Print Size'
	arm-none-eabi-size  --format=berkeley Actionneur.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) Actionneur.elf
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -Obinary Actionneur.elf Actionneur.bin
	-@echo ' '

secondary-outputs: $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
