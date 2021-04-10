################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CompiledModel.cpp \
../src/MdpmfMain.cpp \
../src/PersonParams.cpp \
../src/TableIOUtils.cpp 

OBJS += \
./src/CompiledModel.o \
./src/MdpmfMain.o \
./src/PersonParams.o \
./src/TableIOUtils.o 

CPP_DEPS += \
./src/CompiledModel.d \
./src/MdpmfMain.d \
./src/PersonParams.d \
./src/TableIOUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -I/usr/local/include -I"../../lib/src" -O3 -Wall -c -fmessage-length=0 --std=gnu++11  -Wl,--stack,4194304 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


