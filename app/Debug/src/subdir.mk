################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ActionEffectPlayRecs.cpp \
../src/ActionEffectReward.cpp \
../src/ActionEffectTable.cpp \
../src/ActionEffectTrans.cpp \
../src/CompiledModel.cpp \
../src/MdpmfMain.cpp \
../src/PersonParams.cpp \
../src/TableIOUtils.cpp 

OBJS += \
./src/ActionEffectPlayRecs.o \
./src/ActionEffectReward.o \
./src/ActionEffectTable.o \
./src/ActionEffectTrans.o \
./src/CompiledModel.o \
./src/MdpmfMain.o \
./src/PersonParams.o \
./src/TableIOUtils.o 

CPP_DEPS += \
./src/ActionEffectPlayRecs.d \
./src/ActionEffectReward.d \
./src/ActionEffectTable.d \
./src/ActionEffectTrans.d \
./src/CompiledModel.d \
./src/MdpmfMain.d \
./src/PersonParams.d \
./src/TableIOUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -I"C:\MinGW\include" -I/usr/local/include -I"../../MDP Library/src" -O0 -g3 -g -Wall -c -fmessage-length=0 --std=gnu++11   -fopenmp -Wa,-mbig-obj -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


