################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/MThread.cpp \
../src/MultipleTCPSocketsListener.cpp \
../src/TCPMessengerClient.cpp \
../src/TCPSocket.cpp \
../src/UDPMessenger.cpp \
../src/UDPSocket.cpp \
../src/main.cpp 

OBJS += \
./src/MThread.o \
./src/MultipleTCPSocketsListener.o \
./src/TCPMessengerClient.o \
./src/TCPSocket.o \
./src/UDPMessenger.o \
./src/UDPSocket.o \
./src/main.o 

CPP_DEPS += \
./src/MThread.d \
./src/MultipleTCPSocketsListener.d \
./src/TCPMessengerClient.d \
./src/TCPSocket.d \
./src/UDPMessenger.d \
./src/UDPSocket.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


