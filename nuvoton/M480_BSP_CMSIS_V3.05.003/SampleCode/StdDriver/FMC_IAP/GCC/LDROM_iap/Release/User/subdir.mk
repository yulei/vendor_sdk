################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/LDROM_iap.c 

S_UPPER_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/startup_M480_tiny.S 

OBJS += \
./User/LDROM_iap.o \
./User/startup_M480_tiny.o 

S_UPPER_DEPS += \
./User/startup_M480_tiny.d 

C_DEPS += \
./User/LDROM_iap.d 


# Each subdirectory must supply rules for building sources it contributes
User/LDROM_iap.o: C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/LDROM_iap.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/LDROM_iap/../../../../../Library/Device/Nuvoton/M480/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/LDROM_iap/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/LDROM_iap/../../../../../Library/CMSIS/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

User/startup_M480_tiny.o: C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/startup_M480_tiny.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


