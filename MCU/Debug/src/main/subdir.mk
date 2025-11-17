################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/main/main.cpp \
../src/main/systemManagement.cpp \
../src/main/systemObjects.cpp 

OBJS += \
./src/main/main.o \
./src/main/systemManagement.o \
./src/main/systemObjects.o 

CPP_DEPS += \
./src/main/main.d \
./src/main/systemManagement.d \
./src/main/systemObjects.d 


# Each subdirectory must supply rules for building sources it contributes
src/main/%.o src/main/%.su src/main/%.cyclo: ../src/main/%.cpp src/main/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-main

clean-src-2f-main:
	-$(RM) ./src/main/main.cyclo ./src/main/main.d ./src/main/main.o ./src/main/main.su ./src/main/systemManagement.cyclo ./src/main/systemManagement.d ./src/main/systemManagement.o ./src/main/systemManagement.su ./src/main/systemObjects.cyclo ./src/main/systemObjects.d ./src/main/systemObjects.o ./src/main/systemObjects.su

.PHONY: clean-src-2f-main

