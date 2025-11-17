################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Hardware/ADC.cpp \
../src/Hardware/CB.cpp \
../src/Hardware/GPIO.cpp \
../src/Hardware/Global.cpp \
../src/Hardware/HardwareCenter.cpp \
../src/Hardware/Macro.cpp \
../src/Hardware/Thermistor.cpp \
../src/Hardware/led.cpp 

OBJS += \
./src/Hardware/ADC.o \
./src/Hardware/CB.o \
./src/Hardware/GPIO.o \
./src/Hardware/Global.o \
./src/Hardware/HardwareCenter.o \
./src/Hardware/Macro.o \
./src/Hardware/Thermistor.o \
./src/Hardware/led.o 

CPP_DEPS += \
./src/Hardware/ADC.d \
./src/Hardware/CB.d \
./src/Hardware/GPIO.d \
./src/Hardware/Global.d \
./src/Hardware/HardwareCenter.d \
./src/Hardware/Macro.d \
./src/Hardware/Thermistor.d \
./src/Hardware/led.d 


# Each subdirectory must supply rules for building sources it contributes
src/Hardware/%.o src/Hardware/%.su src/Hardware/%.cyclo: ../src/Hardware/%.cpp src/Hardware/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-Hardware

clean-src-2f-Hardware:
	-$(RM) ./src/Hardware/ADC.cyclo ./src/Hardware/ADC.d ./src/Hardware/ADC.o ./src/Hardware/ADC.su ./src/Hardware/CB.cyclo ./src/Hardware/CB.d ./src/Hardware/CB.o ./src/Hardware/CB.su ./src/Hardware/GPIO.cyclo ./src/Hardware/GPIO.d ./src/Hardware/GPIO.o ./src/Hardware/GPIO.su ./src/Hardware/Global.cyclo ./src/Hardware/Global.d ./src/Hardware/Global.o ./src/Hardware/Global.su ./src/Hardware/HardwareCenter.cyclo ./src/Hardware/HardwareCenter.d ./src/Hardware/HardwareCenter.o ./src/Hardware/HardwareCenter.su ./src/Hardware/Macro.cyclo ./src/Hardware/Macro.d ./src/Hardware/Macro.o ./src/Hardware/Macro.su ./src/Hardware/Thermistor.cyclo ./src/Hardware/Thermistor.d ./src/Hardware/Thermistor.o ./src/Hardware/Thermistor.su ./src/Hardware/led.cyclo ./src/Hardware/led.d ./src/Hardware/led.o ./src/Hardware/led.su

.PHONY: clean-src-2f-Hardware

