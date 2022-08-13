################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/GCC/_syscalls.c 

S_UPPER_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/GCC/startup_M480.S 

OBJS += \
./CMSIS/CMSIS/GCC/_syscalls.o \
./CMSIS/CMSIS/GCC/startup_M480.o 

S_UPPER_DEPS += \
./CMSIS/CMSIS/GCC/startup_M480.d 

C_DEPS += \
./CMSIS/CMSIS/GCC/_syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/CMSIS/GCC/_syscalls.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/GCC/_syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/CMSIS/GCC/startup_M480.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/GCC/startup_M480.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


