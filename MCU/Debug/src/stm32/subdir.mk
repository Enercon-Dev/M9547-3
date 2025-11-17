################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/stm32/I2C.cpp \
../src/stm32/SPI.cpp \
../src/stm32/macAddr.cpp 

C_SRCS += \
../src/stm32/misc.c \
../src/stm32/stm32Clock.c \
../src/stm32/stm32Init.c \
../src/stm32/stm32Interface.c \
../src/stm32/stm32f10x_adc.c \
../src/stm32/stm32f10x_dma.c \
../src/stm32/stm32f10x_flash.c \
../src/stm32/stm32f10x_gpio.c \
../src/stm32/stm32f10x_i2c.c \
../src/stm32/stm32f10x_rcc.c \
../src/stm32/stm32f10x_spi.c \
../src/stm32/stm32f10x_tim.c \
../src/stm32/syscalls.c \
../src/stm32/system_stm32f10x.c 

C_DEPS += \
./src/stm32/misc.d \
./src/stm32/stm32Clock.d \
./src/stm32/stm32Init.d \
./src/stm32/stm32Interface.d \
./src/stm32/stm32f10x_adc.d \
./src/stm32/stm32f10x_dma.d \
./src/stm32/stm32f10x_flash.d \
./src/stm32/stm32f10x_gpio.d \
./src/stm32/stm32f10x_i2c.d \
./src/stm32/stm32f10x_rcc.d \
./src/stm32/stm32f10x_spi.d \
./src/stm32/stm32f10x_tim.d \
./src/stm32/syscalls.d \
./src/stm32/system_stm32f10x.d 

OBJS += \
./src/stm32/I2C.o \
./src/stm32/SPI.o \
./src/stm32/macAddr.o \
./src/stm32/misc.o \
./src/stm32/stm32Clock.o \
./src/stm32/stm32Init.o \
./src/stm32/stm32Interface.o \
./src/stm32/stm32f10x_adc.o \
./src/stm32/stm32f10x_dma.o \
./src/stm32/stm32f10x_flash.o \
./src/stm32/stm32f10x_gpio.o \
./src/stm32/stm32f10x_i2c.o \
./src/stm32/stm32f10x_rcc.o \
./src/stm32/stm32f10x_spi.o \
./src/stm32/stm32f10x_tim.o \
./src/stm32/syscalls.o \
./src/stm32/system_stm32f10x.o 

CPP_DEPS += \
./src/stm32/I2C.d \
./src/stm32/SPI.d \
./src/stm32/macAddr.d 


# Each subdirectory must supply rules for building sources it contributes
src/stm32/%.o src/stm32/%.su src/stm32/%.cyclo: ../src/stm32/%.cpp src/stm32/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../CMSIS/Include -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/stm32/%.o src/stm32/%.su src/stm32/%.cyclo: ../src/stm32/%.c src/stm32/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -I../CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-stm32

clean-src-2f-stm32:
	-$(RM) ./src/stm32/I2C.cyclo ./src/stm32/I2C.d ./src/stm32/I2C.o ./src/stm32/I2C.su ./src/stm32/SPI.cyclo ./src/stm32/SPI.d ./src/stm32/SPI.o ./src/stm32/SPI.su ./src/stm32/macAddr.cyclo ./src/stm32/macAddr.d ./src/stm32/macAddr.o ./src/stm32/macAddr.su ./src/stm32/misc.cyclo ./src/stm32/misc.d ./src/stm32/misc.o ./src/stm32/misc.su ./src/stm32/stm32Clock.cyclo ./src/stm32/stm32Clock.d ./src/stm32/stm32Clock.o ./src/stm32/stm32Clock.su ./src/stm32/stm32Init.cyclo ./src/stm32/stm32Init.d ./src/stm32/stm32Init.o ./src/stm32/stm32Init.su ./src/stm32/stm32Interface.cyclo ./src/stm32/stm32Interface.d ./src/stm32/stm32Interface.o ./src/stm32/stm32Interface.su ./src/stm32/stm32f10x_adc.cyclo ./src/stm32/stm32f10x_adc.d ./src/stm32/stm32f10x_adc.o ./src/stm32/stm32f10x_adc.su ./src/stm32/stm32f10x_dma.cyclo ./src/stm32/stm32f10x_dma.d ./src/stm32/stm32f10x_dma.o ./src/stm32/stm32f10x_dma.su ./src/stm32/stm32f10x_flash.cyclo ./src/stm32/stm32f10x_flash.d ./src/stm32/stm32f10x_flash.o ./src/stm32/stm32f10x_flash.su ./src/stm32/stm32f10x_gpio.cyclo ./src/stm32/stm32f10x_gpio.d ./src/stm32/stm32f10x_gpio.o ./src/stm32/stm32f10x_gpio.su ./src/stm32/stm32f10x_i2c.cyclo ./src/stm32/stm32f10x_i2c.d ./src/stm32/stm32f10x_i2c.o ./src/stm32/stm32f10x_i2c.su ./src/stm32/stm32f10x_rcc.cyclo ./src/stm32/stm32f10x_rcc.d ./src/stm32/stm32f10x_rcc.o ./src/stm32/stm32f10x_rcc.su ./src/stm32/stm32f10x_spi.cyclo ./src/stm32/stm32f10x_spi.d ./src/stm32/stm32f10x_spi.o ./src/stm32/stm32f10x_spi.su ./src/stm32/stm32f10x_tim.cyclo ./src/stm32/stm32f10x_tim.d ./src/stm32/stm32f10x_tim.o ./src/stm32/stm32f10x_tim.su ./src/stm32/syscalls.cyclo ./src/stm32/syscalls.d ./src/stm32/syscalls.o ./src/stm32/syscalls.su ./src/stm32/system_stm32f10x.cyclo ./src/stm32/system_stm32f10x.d ./src/stm32/system_stm32f10x.o ./src/stm32/system_stm32f10x.su

.PHONY: clean-src-2f-stm32

