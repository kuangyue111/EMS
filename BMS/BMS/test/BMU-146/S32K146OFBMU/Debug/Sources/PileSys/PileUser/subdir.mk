################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileUser/PileGroupRcv.c" \
"../Sources/PileSys/PileUser/PileUserAPI.c" \

C_SRCS += \
../Sources/PileSys/PileUser/PileGroupRcv.c \
../Sources/PileSys/PileUser/PileUserAPI.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileUser/PileGroupRcv.d" \
"./Sources/PileSys/PileUser/PileUserAPI.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileUser/PileGroupRcv.o" \
"./Sources/PileSys/PileUser/PileUserAPI.o" \

C_DEPS += \
./Sources/PileSys/PileUser/PileGroupRcv.d \
./Sources/PileSys/PileUser/PileUserAPI.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileUser/PileGroupRcv.o \
./Sources/PileSys/PileUser/PileUserAPI.o \

OBJS += \
./Sources/PileSys/PileUser/PileGroupRcv.o \
./Sources/PileSys/PileUser/PileUserAPI.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileUser/PileGroupRcv.o: ../Sources/PileSys/PileUser/PileGroupRcv.c
	@echo 'Building file: $<'
	@echo 'Executing target #256 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileUser/PileGroupRcv.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileUser/PileGroupRcv.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileUser/PileUserAPI.o: ../Sources/PileSys/PileUser/PileUserAPI.c
	@echo 'Building file: $<'
	@echo 'Executing target #257 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileUser/PileUserAPI.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileUser/PileUserAPI.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


