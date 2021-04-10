# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/muParser.cpp \
../src/muParserBase.cpp \
../src/muParserBytecode.cpp \
../src/muParserCallback.cpp \
../src/muParserError.cpp \
../src/muParserTokenReader.cpp \
../src/Action.cpp \
../src/ActionEffectPlayRecs.cpp \
../src/ActionEffectReward.cpp \
../src/ActionEffectTable.cpp \
../src/ActionEffectTrans.cpp \
../src/ActionSequence.cpp \
../src/ActionSet.cpp \
../src/BootstrapEstimator.cpp \
../src/DRBolzman.cpp \
../src/DRIRT.cpp \
../src/DRMax.cpp \
../src/DecisionRule.cpp \
../src/HypothesisSpace.cpp \
../src/IOTable.cpp \
../src/MDP.cpp \
../src/MDPIRTModel.cpp \
../src/ModelSpec.cpp \
../src/ParamList.cpp \
../src/Policy.cpp \
../src/Reward.cpp \
../src/RewardTable.cpp \
../src/SimEngine.cpp \
../src/State.cpp \
../src/StateSpace.cpp \
../src/StateSpaceVarList.cpp \
../src/StateVarList.cpp \
../src/State_test.cpp \
../src/TransTable.cpp \
../src/Transition.cpp \
../src/VarDesc.cpp

OBJS += \
./src/muParser.o \
./src/muParserBase.o \
./src/muParserBytecode.o \
./src/muParserCallback.o \
./src/muParserError.o \
./src/muParserTokenReader.o \
./src/Action.o \
./src/ActionEffectPlayRecs.o \
./src/ActionEffectReward.o \
./src/ActionEffectTable.o \
./src/ActionEffectTrans.o \
./src/ActionSequence.o \
./src/ActionSet.o \
./src/BootstrapEstimator.o \
./src/DRBolzman.o \
./src/DRIRT.o \
./src/DRMax.o \
./src/DecisionRule.o \
./src/HypothesisSpace.o \
./src/IOTable.o \
./src/MDP.o \
./src/MDPIRTModel.o \
./src/ModelSpec.o \
./src/ParamList.o \
./src/Policy.o \
./src/Reward.o \
./src/RewardTable.o \
./src/SimEngine.o \
./src/State.o \
./src/StateSpace.o \
./src/StateSpaceVarList.o \
./src/StateVarList.o \
./src/State_test.o \
./src/TransTable.o \
./src/Transition.o \
./src/VarDesc.o 

CPP_DEPS += \
./src/muParser.d \
./src/muParserBase.d \
./src/muParserBytecode.d \
./src/muParserCallback.d \
./src/muParserError.d \
./src/muParserTokenReader.d \
./src/Action.d \
./src/ActionEffectPlayRecs.d \
./src/ActionEffectReward.d \
./src/ActionEffectTable.d \
./src/ActionEffectTrans.d \
./src/ActionSequence.d \
./src/ActionSet.d \
./src/BootstrapEstimator.d \
./src/DRBolzman.d \
./src/DRIRT.d \
./src/DRMax.d \
./src/DecisionRule.d \
./src/HypothesisSpace.d \
./src/IOTable.d \
./src/MDP.d \
./src/MDPIRTModel.d \
./src/ModelSpec.d \
./src/ParamList.d \
./src/Policy.d \
./src/Reward.d \
./src/RewardTable.d \
./src/SimEngine.d \
./src/State.d \
./src/StateGrid.d \
./src/StateSpace.d \
./src/StateSpaceGrid.d \
./src/StateSpaceVarList.d \
./src/StateVarList.d \
./src/State_test.d \
./src/TransTable.d \
./src/Transition.d \
./src/VarDesc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/usr/local/include -I"/mingw64/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


