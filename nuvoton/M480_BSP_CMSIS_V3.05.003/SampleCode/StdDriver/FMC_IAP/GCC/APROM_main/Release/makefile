################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Library/Library/subdir.mk
-include CMSIS/CMSIS/GCC/subdir.mk
-include CMSIS/CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
APROM_main.hex \

SECONDARY_SIZE += \
APROM_main.siz \


# All Target
all: APROM_main.elf secondary-outputs

# Tool invocations
APROM_main.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM GNU C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T "C:\BACKUP\M4\M480\Git\BSP\bsp\Library\Device\Nuvoton\M480\Source\GCC\gcc_arm_128k.ld" -Xlinker --gc-sections -Wl,-Map,"APROM_main.map" -o "APROM_main.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

APROM_main.hex: APROM_main.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "APROM_main.elf"  "APROM_main.hex"
	@echo 'Finished building: $@'
	@echo ' '

APROM_main.siz: APROM_main.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "APROM_main.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) APROM_main.elf
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -O binary APROM_main.elf ../../APROM_main.bin
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
