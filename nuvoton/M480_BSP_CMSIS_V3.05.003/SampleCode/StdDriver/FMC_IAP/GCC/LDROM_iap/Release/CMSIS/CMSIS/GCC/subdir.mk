################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/GCC/_syscalls.c 

OBJS += \
./CMSIS/CMSIS/GCC/_syscalls.o 

C_DEPS += \
./CMSIS/CMSIS/GCC/_syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/CMSIS/GCC/_syscalls.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/GCC/_syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/LDROM_iap/../../../../../Library/Device/Nuvoton/M480/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/LDROM_iap/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/LDROM_iap/../../../../../Library/CMSIS/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


