################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/general/genericClasses.cpp \
../src/general/timeAcc.cpp 

C_SRCS += \
../src/general/placeHolder.c 

C_DEPS += \
./src/general/placeHolder.d 

OBJS += \
./src/general/genericClasses.o \
./src/general/placeHolder.o \
./src/general/timeAcc.o 

CPP_DEPS += \
./src/general/genericClasses.d \
./src/general/timeAcc.d 


# Each subdirectory must supply rules for building sources it contributes
src/general/%.o src/general/%.su src/general/%.cyclo: ../src/general/%.cpp src/general/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/general/%.o src/general/%.su src/general/%.cyclo: ../src/general/%.c src/general/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -I../CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-general

clean-src-2f-general:
	-$(RM) ./src/general/genericClasses.cyclo ./src/general/genericClasses.d ./src/general/genericClasses.o ./src/general/genericClasses.su ./src/general/placeHolder.cyclo ./src/general/placeHolder.d ./src/general/placeHolder.o ./src/general/placeHolder.su ./src/general/timeAcc.cyclo ./src/general/timeAcc.d ./src/general/timeAcc.o ./src/general/timeAcc.su

.PHONY: clean-src-2f-general

