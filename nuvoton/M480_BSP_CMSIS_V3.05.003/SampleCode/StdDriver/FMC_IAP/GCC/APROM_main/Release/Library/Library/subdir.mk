################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/clk.c \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/fmc.c \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/retarget.c \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/sys.c \
C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/uart.c 

OBJS += \
./Library/Library/clk.o \
./Library/Library/fmc.o \
./Library/Library/retarget.o \
./Library/Library/sys.o \
./Library/Library/uart.o 

C_DEPS += \
./Library/Library/clk.d \
./Library/Library/fmc.d \
./Library/Library/retarget.d \
./Library/Library/sys.d \
./Library/Library/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Library/Library/clk.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/clk.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Library/Library/fmc.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/fmc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Library/Library/retarget.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/retarget.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Library/Library/sys.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/sys.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Library/Library/uart.o: C:/BACKUP/M4/M480/Git/BSP/bsp/Library/StdDriver/src/uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/CMSIS/Include" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/StdDriver/inc" -I"C:/BACKUP/M4/M480/Git/BSP/bsp/SampleCode/StdDriver/FMC_IAP/GCC/APROM_main/../../../../../Library/Device/Nuvoton/M480/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


