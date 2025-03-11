################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/FEB_AD68xx_Interface.c \
../Core/Src/FEB_ADBMS6830B.c \
../Core/Src/FEB_ADBMS6830B_Driver.c \
../Core/Src/FEB_CAN.c \
../Core/Src/FEB_Config.c \
../Core/Src/FEB_HW.c \
../Core/Src/FEB_IVT.c \
../Core/Src/FEB_Main.c \
../Core/Src/FEB_SM.c \
../Core/Src/FEB_UART_Transmit.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/FEB_AD68xx_Interface.o \
./Core/Src/FEB_ADBMS6830B.o \
./Core/Src/FEB_ADBMS6830B_Driver.o \
./Core/Src/FEB_CAN.o \
./Core/Src/FEB_Config.o \
./Core/Src/FEB_HW.o \
./Core/Src/FEB_IVT.o \
./Core/Src/FEB_Main.o \
./Core/Src/FEB_SM.o \
./Core/Src/FEB_UART_Transmit.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/FEB_AD68xx_Interface.d \
./Core/Src/FEB_ADBMS6830B.d \
./Core/Src/FEB_ADBMS6830B_Driver.d \
./Core/Src/FEB_CAN.d \
./Core/Src/FEB_Config.d \
./Core/Src/FEB_HW.d \
./Core/Src/FEB_IVT.d \
./Core/Src/FEB_Main.d \
./Core/Src/FEB_SM.d \
./Core/Src/FEB_UART_Transmit.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -IC:/Users/andys/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/andys/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/andys/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/andys/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/FEB_AD68xx_Interface.cyclo ./Core/Src/FEB_AD68xx_Interface.d ./Core/Src/FEB_AD68xx_Interface.o ./Core/Src/FEB_AD68xx_Interface.su ./Core/Src/FEB_ADBMS6830B.cyclo ./Core/Src/FEB_ADBMS6830B.d ./Core/Src/FEB_ADBMS6830B.o ./Core/Src/FEB_ADBMS6830B.su ./Core/Src/FEB_ADBMS6830B_Driver.cyclo ./Core/Src/FEB_ADBMS6830B_Driver.d ./Core/Src/FEB_ADBMS6830B_Driver.o ./Core/Src/FEB_ADBMS6830B_Driver.su ./Core/Src/FEB_CAN.cyclo ./Core/Src/FEB_CAN.d ./Core/Src/FEB_CAN.o ./Core/Src/FEB_CAN.su ./Core/Src/FEB_Config.cyclo ./Core/Src/FEB_Config.d ./Core/Src/FEB_Config.o ./Core/Src/FEB_Config.su ./Core/Src/FEB_HW.cyclo ./Core/Src/FEB_HW.d ./Core/Src/FEB_HW.o ./Core/Src/FEB_HW.su ./Core/Src/FEB_IVT.cyclo ./Core/Src/FEB_IVT.d ./Core/Src/FEB_IVT.o ./Core/Src/FEB_IVT.su ./Core/Src/FEB_Main.cyclo ./Core/Src/FEB_Main.d ./Core/Src/FEB_Main.o ./Core/Src/FEB_Main.su ./Core/Src/FEB_SM.cyclo ./Core/Src/FEB_SM.d ./Core/Src/FEB_SM.o ./Core/Src/FEB_SM.su ./Core/Src/FEB_UART_Transmit.cyclo ./Core/Src/FEB_UART_Transmit.d ./Core/Src/FEB_UART_Transmit.o ./Core/Src/FEB_UART_Transmit.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

