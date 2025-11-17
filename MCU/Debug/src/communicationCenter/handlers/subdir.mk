################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/communicationCenter/handlers/Control.cpp \
../src/communicationCenter/handlers/DHCP.cpp 

OBJS += \
./src/communicationCenter/handlers/Control.o \
./src/communicationCenter/handlers/DHCP.o 

CPP_DEPS += \
./src/communicationCenter/handlers/Control.d \
./src/communicationCenter/handlers/DHCP.d 


# Each subdirectory must supply rules for building sources it contributes
src/communicationCenter/handlers/%.o src/communicationCenter/handlers/%.su src/communicationCenter/handlers/%.cyclo: ../src/communicationCenter/handlers/%.cpp src/communicationCenter/handlers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-communicationCenter-2f-handlers

clean-src-2f-communicationCenter-2f-handlers:
	-$(RM) ./src/communicationCenter/handlers/Control.cyclo ./src/communicationCenter/handlers/Control.d ./src/communicationCenter/handlers/Control.o ./src/communicationCenter/handlers/Control.su ./src/communicationCenter/handlers/DHCP.cyclo ./src/communicationCenter/handlers/DHCP.d ./src/communicationCenter/handlers/DHCP.o ./src/communicationCenter/handlers/DHCP.su

.PHONY: clean-src-2f-communicationCenter-2f-handlers

