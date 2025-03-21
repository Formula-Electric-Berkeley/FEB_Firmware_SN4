################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/FEB_CAN.c \
../Core/Src/FEB_CAN_APPS.c \
../Core/Src/FEB_CAN_BMS.c \
../Core/Src/FEB_CAN_ICS.c \
../Core/Src/FEB_IO_ICS.c \
../Core/Src/FEB_Main.c \
../Core/Src/FEB_Radio.c \
../Core/Src/FEB_UART_Transmit.c \
../Core/Src/FEB_UI.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/FEB_CAN.o \
./Core/Src/FEB_CAN_APPS.o \
./Core/Src/FEB_CAN_BMS.o \
./Core/Src/FEB_CAN_ICS.o \
./Core/Src/FEB_IO_ICS.o \
./Core/Src/FEB_Main.o \
./Core/Src/FEB_Radio.o \
./Core/Src/FEB_UART_Transmit.o \
./Core/Src/FEB_UI.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/FEB_CAN.d \
./Core/Src/FEB_CAN_APPS.d \
./Core/Src/FEB_CAN_BMS.d \
./Core/Src/FEB_CAN_ICS.d \
./Core/Src/FEB_IO_ICS.d \
./Core/Src/FEB_Main.d \
./Core/Src/FEB_Radio.d \
./Core/Src/FEB_UART_Transmit.d \
./Core/Src/FEB_UI.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F469xx -c -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/squareline_ui" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Common -I../Drivers/BSP/STM32469I-Discovery -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/lvgl" -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/hal_stm_lvgl" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/FEB_CAN.cyclo ./Core/Src/FEB_CAN.d ./Core/Src/FEB_CAN.o ./Core/Src/FEB_CAN.su ./Core/Src/FEB_CAN_APPS.cyclo ./Core/Src/FEB_CAN_APPS.d ./Core/Src/FEB_CAN_APPS.o ./Core/Src/FEB_CAN_APPS.su ./Core/Src/FEB_CAN_BMS.cyclo ./Core/Src/FEB_CAN_BMS.d ./Core/Src/FEB_CAN_BMS.o ./Core/Src/FEB_CAN_BMS.su ./Core/Src/FEB_CAN_ICS.cyclo ./Core/Src/FEB_CAN_ICS.d ./Core/Src/FEB_CAN_ICS.o ./Core/Src/FEB_CAN_ICS.su ./Core/Src/FEB_IO_ICS.cyclo ./Core/Src/FEB_IO_ICS.d ./Core/Src/FEB_IO_ICS.o ./Core/Src/FEB_IO_ICS.su ./Core/Src/FEB_Main.cyclo ./Core/Src/FEB_Main.d ./Core/Src/FEB_Main.o ./Core/Src/FEB_Main.su ./Core/Src/FEB_Radio.cyclo ./Core/Src/FEB_Radio.d ./Core/Src/FEB_Radio.o ./Core/Src/FEB_Radio.su ./Core/Src/FEB_UART_Transmit.cyclo ./Core/Src/FEB_UART_Transmit.d ./Core/Src/FEB_UART_Transmit.o ./Core/Src/FEB_UART_Transmit.su ./Core/Src/FEB_UI.cyclo ./Core/Src/FEB_UI.d ./Core/Src/FEB_UI.o ./Core/Src/FEB_UI.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

