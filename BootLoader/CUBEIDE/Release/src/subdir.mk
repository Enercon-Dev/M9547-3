################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Projects/Embedded/BootLoader/src/main.c \
C:/Projects/Embedded/BootLoader/src/stm32f10x_flash.c 

C_DEPS += \
./src/main.d \
./src/stm32f10x_flash.d 

OBJS += \
./src/main.o \
./src/stm32f10x_flash.o 


# Each subdirectory must supply rules for building sources it contributes
src/main.o: C:/Projects/Embedded/BootLoader/src/main.c src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F107xC -c -I../Drivers/CMSIS/Include -I../../ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/stm32f10x_flash.o: C:/Projects/Embedded/BootLoader/src/stm32f10x_flash.c src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F107xC -c -I../Drivers/CMSIS/Include -I../../ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src

clean-src:
	-$(RM) ./src/main.cyclo ./src/main.d ./src/main.o ./src/main.su ./src/stm32f10x_flash.cyclo ./src/stm32f10x_flash.d ./src/stm32f10x_flash.o ./src/stm32f10x_flash.su

.PHONY: clean-src

