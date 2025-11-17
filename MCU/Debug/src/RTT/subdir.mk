################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/RTT/SEGGER_RTT.c \
../src/RTT/SEGGER_RTT_printf.c 

C_DEPS += \
./src/RTT/SEGGER_RTT.d \
./src/RTT/SEGGER_RTT_printf.d 

OBJS += \
./src/RTT/SEGGER_RTT.o \
./src/RTT/SEGGER_RTT_printf.o 


# Each subdirectory must supply rules for building sources it contributes
src/RTT/%.o src/RTT/%.su src/RTT/%.cyclo: ../src/RTT/%.c src/RTT/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -I../CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-RTT

clean-src-2f-RTT:
	-$(RM) ./src/RTT/SEGGER_RTT.cyclo ./src/RTT/SEGGER_RTT.d ./src/RTT/SEGGER_RTT.o ./src/RTT/SEGGER_RTT.su ./src/RTT/SEGGER_RTT_printf.cyclo ./src/RTT/SEGGER_RTT_printf.d ./src/RTT/SEGGER_RTT_printf.o ./src/RTT/SEGGER_RTT_printf.su

.PHONY: clean-src-2f-RTT

