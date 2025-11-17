################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/communicationCenter/buffers/bufferFlush.cpp \
../src/communicationCenter/buffers/buffers.cpp 

OBJS += \
./src/communicationCenter/buffers/bufferFlush.o \
./src/communicationCenter/buffers/buffers.o 

CPP_DEPS += \
./src/communicationCenter/buffers/bufferFlush.d \
./src/communicationCenter/buffers/buffers.d 


# Each subdirectory must supply rules for building sources it contributes
src/communicationCenter/buffers/%.o src/communicationCenter/buffers/%.su src/communicationCenter/buffers/%.cyclo: ../src/communicationCenter/buffers/%.cpp src/communicationCenter/buffers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-communicationCenter-2f-buffers

clean-src-2f-communicationCenter-2f-buffers:
	-$(RM) ./src/communicationCenter/buffers/bufferFlush.cyclo ./src/communicationCenter/buffers/bufferFlush.d ./src/communicationCenter/buffers/bufferFlush.o ./src/communicationCenter/buffers/bufferFlush.su ./src/communicationCenter/buffers/buffers.cyclo ./src/communicationCenter/buffers/buffers.d ./src/communicationCenter/buffers/buffers.o ./src/communicationCenter/buffers/buffers.su

.PHONY: clean-src-2f-communicationCenter-2f-buffers

