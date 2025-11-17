################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/communicationCenter/SlaveMasterComm.cpp \
../src/communicationCenter/communicationCenter.cpp 

OBJS += \
./src/communicationCenter/SlaveMasterComm.o \
./src/communicationCenter/communicationCenter.o 

CPP_DEPS += \
./src/communicationCenter/SlaveMasterComm.d \
./src/communicationCenter/communicationCenter.d 


# Each subdirectory must supply rules for building sources it contributes
src/communicationCenter/%.o src/communicationCenter/%.su src/communicationCenter/%.cyclo: ../src/communicationCenter/%.cpp src/communicationCenter/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-communicationCenter

clean-src-2f-communicationCenter:
	-$(RM) ./src/communicationCenter/SlaveMasterComm.cyclo ./src/communicationCenter/SlaveMasterComm.d ./src/communicationCenter/SlaveMasterComm.o ./src/communicationCenter/SlaveMasterComm.su ./src/communicationCenter/communicationCenter.cyclo ./src/communicationCenter/communicationCenter.d ./src/communicationCenter/communicationCenter.o ./src/communicationCenter/communicationCenter.su

.PHONY: clean-src-2f-communicationCenter

