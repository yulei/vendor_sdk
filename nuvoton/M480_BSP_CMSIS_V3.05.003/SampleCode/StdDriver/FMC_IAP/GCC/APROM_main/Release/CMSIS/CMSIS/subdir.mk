################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/system_M480.c 

OBJS += \
./CMSIS/CMSIS/system_M480.o 

C_DEPS += \
./CMSIS/CMSIS/system_M480.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/CMSIS/system_M480.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/Device/Nuvoton/M480/Source/system_M480.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


