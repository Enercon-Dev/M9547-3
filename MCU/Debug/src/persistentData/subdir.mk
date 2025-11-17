################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/persistentData/persistentData.cpp \
../src/persistentData/persistentObject.cpp \
../src/persistentData/persistentPlaceHolder.cpp \
../src/persistentData/persistentSystemData.cpp \
../src/persistentData/persistentSystemStats.cpp 

OBJS += \
./src/persistentData/persistentData.o \
./src/persistentData/persistentObject.o \
./src/persistentData/persistentPlaceHolder.o \
./src/persistentData/persistentSystemData.o \
./src/persistentData/persistentSystemStats.o 

CPP_DEPS += \
./src/persistentData/persistentData.d \
./src/persistentData/persistentObject.d \
./src/persistentData/persistentPlaceHolder.d \
./src/persistentData/persistentSystemData.d \
./src/persistentData/persistentSystemStats.d 


# Each subdirectory must supply rules for building sources it contributes
src/persistentData/%.o src/persistentData/%.su src/persistentData/%.cyclo: ../src/persistentData/%.cpp src/persistentData/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-persistentData

clean-src-2f-persistentData:
	-$(RM) ./src/persistentData/persistentData.cyclo ./src/persistentData/persistentData.d ./src/persistentData/persistentData.o ./src/persistentData/persistentData.su ./src/persistentData/persistentObject.cyclo ./src/persistentData/persistentObject.d ./src/persistentData/persistentObject.o ./src/persistentData/persistentObject.su ./src/persistentData/persistentPlaceHolder.cyclo ./src/persistentData/persistentPlaceHolder.d ./src/persistentData/persistentPlaceHolder.o ./src/persistentData/persistentPlaceHolder.su ./src/persistentData/persistentSystemData.cyclo ./src/persistentData/persistentSystemData.d ./src/persistentData/persistentSystemData.o ./src/persistentData/persistentSystemData.su ./src/persistentData/persistentSystemStats.cyclo ./src/persistentData/persistentSystemStats.d ./src/persistentData/persistentSystemStats.o ./src/persistentData/persistentSystemStats.su

.PHONY: clean-src-2f-persistentData

