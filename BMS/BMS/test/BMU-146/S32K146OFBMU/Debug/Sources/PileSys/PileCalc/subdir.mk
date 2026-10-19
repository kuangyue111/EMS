################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileCalc/PileCalcUtil.c" \
"../Sources/PileSys/PileCalc/PileDataCalc.c" \
"../Sources/PileSys/PileCalc/PileLimCalc.c" \

C_SRCS += \
../Sources/PileSys/PileCalc/PileCalcUtil.c \
../Sources/PileSys/PileCalc/PileDataCalc.c \
../Sources/PileSys/PileCalc/PileLimCalc.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileCalc/PileCalcUtil.d" \
"./Sources/PileSys/PileCalc/PileDataCalc.d" \
"./Sources/PileSys/PileCalc/PileLimCalc.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileCalc/PileCalcUtil.o" \
"./Sources/PileSys/PileCalc/PileDataCalc.o" \
"./Sources/PileSys/PileCalc/PileLimCalc.o" \

C_DEPS += \
./Sources/PileSys/PileCalc/PileCalcUtil.d \
./Sources/PileSys/PileCalc/PileDataCalc.d \
./Sources/PileSys/PileCalc/PileLimCalc.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileCalc/PileCalcUtil.o \
./Sources/PileSys/PileCalc/PileDataCalc.o \
./Sources/PileSys/PileCalc/PileLimCalc.o \

OBJS += \
./Sources/PileSys/PileCalc/PileCalcUtil.o \
./Sources/PileSys/PileCalc/PileDataCalc.o \
./Sources/PileSys/PileCalc/PileLimCalc.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileCalc/PileCalcUtil.o: ../Sources/PileSys/PileCalc/PileCalcUtil.c
	@echo 'Building file: $<'
	@echo 'Executing target #230 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileCalc/PileCalcUtil.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileCalc/PileCalcUtil.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileCalc/PileDataCalc.o: ../Sources/PileSys/PileCalc/PileDataCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #231 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileCalc/PileDataCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileCalc/PileDataCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileCalc/PileLimCalc.o: ../Sources/PileSys/PileCalc/PileLimCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #232 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileCalc/PileLimCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileCalc/PileLimCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


