################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/communicationCenter/ethernet/Layer3.cpp \
../src/communicationCenter/ethernet/arp.cpp \
../src/communicationCenter/ethernet/frame.cpp \
../src/communicationCenter/ethernet/ipFrame.cpp \
../src/communicationCenter/ethernet/ipLayer.cpp \
../src/communicationCenter/ethernet/mac.cpp \
../src/communicationCenter/ethernet/udpConnection.cpp \
../src/communicationCenter/ethernet/udpMessage.cpp 

C_SRCS += \
../src/communicationCenter/ethernet/ethernet_mgr.c \
../src/communicationCenter/ethernet/messageQueue.c 

C_DEPS += \
./src/communicationCenter/ethernet/ethernet_mgr.d \
./src/communicationCenter/ethernet/messageQueue.d 

OBJS += \
./src/communicationCenter/ethernet/Layer3.o \
./src/communicationCenter/ethernet/arp.o \
./src/communicationCenter/ethernet/ethernet_mgr.o \
./src/communicationCenter/ethernet/frame.o \
./src/communicationCenter/ethernet/ipFrame.o \
./src/communicationCenter/ethernet/ipLayer.o \
./src/communicationCenter/ethernet/mac.o \
./src/communicationCenter/ethernet/messageQueue.o \
./src/communicationCenter/ethernet/udpConnection.o \
./src/communicationCenter/ethernet/udpMessage.o 

CPP_DEPS += \
./src/communicationCenter/ethernet/Layer3.d \
./src/communicationCenter/ethernet/arp.d \
./src/communicationCenter/ethernet/frame.d \
./src/communicationCenter/ethernet/ipFrame.d \
./src/communicationCenter/ethernet/ipLayer.d \
./src/communicationCenter/ethernet/mac.d \
./src/communicationCenter/ethernet/udpConnection.d \
./src/communicationCenter/ethernet/udpMessage.d 


# Each subdirectory must supply rules for building sources it contributes
src/communicationCenter/ethernet/%.o src/communicationCenter/ethernet/%.su src/communicationCenter/ethernet/%.cyclo: ../src/communicationCenter/ethernet/%.cpp src/communicationCenter/ethernet/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../CMSIS/Include -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -O1 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/communicationCenter/ethernet/%.o src/communicationCenter/ethernet/%.su src/communicationCenter/ethernet/%.cyclo: ../src/communicationCenter/ethernet/%.c src/communicationCenter/ethernet/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_CUBEIDE -DSTM32F107xC -c -I../src/persistentData -I../src/communicationCenter -I../src -I../src/main -I../src/general -I../src/stm32 -I../CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-communicationCenter-2f-ethernet

clean-src-2f-communicationCenter-2f-ethernet:
	-$(RM) ./src/communicationCenter/ethernet/Layer3.cyclo ./src/communicationCenter/ethernet/Layer3.d ./src/communicationCenter/ethernet/Layer3.o ./src/communicationCenter/ethernet/Layer3.su ./src/communicationCenter/ethernet/arp.cyclo ./src/communicationCenter/ethernet/arp.d ./src/communicationCenter/ethernet/arp.o ./src/communicationCenter/ethernet/arp.su ./src/communicationCenter/ethernet/ethernet_mgr.cyclo ./src/communicationCenter/ethernet/ethernet_mgr.d ./src/communicationCenter/ethernet/ethernet_mgr.o ./src/communicationCenter/ethernet/ethernet_mgr.su ./src/communicationCenter/ethernet/frame.cyclo ./src/communicationCenter/ethernet/frame.d ./src/communicationCenter/ethernet/frame.o ./src/communicationCenter/ethernet/frame.su ./src/communicationCenter/ethernet/ipFrame.cyclo ./src/communicationCenter/ethernet/ipFrame.d ./src/communicationCenter/ethernet/ipFrame.o ./src/communicationCenter/ethernet/ipFrame.su ./src/communicationCenter/ethernet/ipLayer.cyclo ./src/communicationCenter/ethernet/ipLayer.d ./src/communicationCenter/ethernet/ipLayer.o ./src/communicationCenter/ethernet/ipLayer.su ./src/communicationCenter/ethernet/mac.cyclo ./src/communicationCenter/ethernet/mac.d ./src/communicationCenter/ethernet/mac.o ./src/communicationCenter/ethernet/mac.su ./src/communicationCenter/ethernet/messageQueue.cyclo ./src/communicationCenter/ethernet/messageQueue.d ./src/communicationCenter/ethernet/messageQueue.o ./src/communicationCenter/ethernet/messageQueue.su ./src/communicationCenter/ethernet/udpConnection.cyclo ./src/communicationCenter/ethernet/udpConnection.d ./src/communicationCenter/ethernet/udpConnection.o ./src/communicationCenter/ethernet/udpConnection.su ./src/communicationCenter/ethernet/udpMessage.cyclo ./src/communicationCenter/ethernet/udpMessage.d ./src/communicationCenter/ethernet/udpMessage.o ./src/communicationCenter/ethernet/udpMessage.su

.PHONY: clean-src-2f-communicationCenter-2f-ethernet

