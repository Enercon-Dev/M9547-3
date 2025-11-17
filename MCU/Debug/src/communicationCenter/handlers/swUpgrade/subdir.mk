################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/communicationCenter/handlers/swUpgrade/FTP.cpp \
../src/communicationCenter/handlers/swUpgrade/upgradeManager.cpp 

C_SRCS += \
../src/communicationCenter/handlers/swUpgrade/calcCRC.c 

C_DEPS += \
./src/communicationCenter/handlers/swUpgrade/calcCRC.d 

OBJS += \
./src/communicationCenter/handlers/swUpgrade/FTP.o \
./src/communicationCenter/handlers/swUpgrade/calcCRC.o \
./src/communicationCenter/handlers/swUpgrade/upgradeManager.o 

CPP_DEPS += \
./src/communicationCenter/handlers/swUpgrade/FTP.d \
./src/communicationCenter/handlers/swUpgrade/upgradeManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/communicationCenter/handlers/swUpgrade/%.o src/communicationCenter/handlers/swUpgrade/%.su src/communicationCenter/handlers/swUpgrade/%.cyclo: ../src/communicationCenter/handlers/swUpgrade/%.cpp src/communicationCenter/handlers/swUpgrade/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/communicationCenter/handlers/swUpgrade/%.o src/communicationCenter/handlers/swUpgrade/%.su src/communicationCenter/handlers/swUpgrade/%.cyclo: ../src/communicationCenter/handlers/swUpgrade/%.c src/communicationCenter/handlers/swUpgrade/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -I../CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-communicationCenter-2f-handlers-2f-swUpgrade

clean-src-2f-communicationCenter-2f-handlers-2f-swUpgrade:
	-$(RM) ./src/communicationCenter/handlers/swUpgrade/FTP.cyclo ./src/communicationCenter/handlers/swUpgrade/FTP.d ./src/communicationCenter/handlers/swUpgrade/FTP.o ./src/communicationCenter/handlers/swUpgrade/FTP.su ./src/communicationCenter/handlers/swUpgrade/calcCRC.cyclo ./src/communicationCenter/handlers/swUpgrade/calcCRC.d ./src/communicationCenter/handlers/swUpgrade/calcCRC.o ./src/communicationCenter/handlers/swUpgrade/calcCRC.su ./src/communicationCenter/handlers/swUpgrade/upgradeManager.cyclo ./src/communicationCenter/handlers/swUpgrade/upgradeManager.d ./src/communicationCenter/handlers/swUpgrade/upgradeManager.o ./src/communicationCenter/handlers/swUpgrade/upgradeManager.su

.PHONY: clean-src-2f-communicationCenter-2f-handlers-2f-swUpgrade

