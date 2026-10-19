################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileCan/PileCatlMsg.c" \
"../Sources/PileSys/PileCan/PileDnMsg.c" \
"../Sources/PileSys/PileCan/PileUpMsg.c" \

C_SRCS += \
../Sources/PileSys/PileCan/PileCatlMsg.c \
../Sources/PileSys/PileCan/PileDnMsg.c \
../Sources/PileSys/PileCan/PileUpMsg.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileCan/PileCatlMsg.d" \
"./Sources/PileSys/PileCan/PileDnMsg.d" \
"./Sources/PileSys/PileCan/PileUpMsg.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileCan/PileCatlMsg.o" \
"./Sources/PileSys/PileCan/PileDnMsg.o" \
"./Sources/PileSys/PileCan/PileUpMsg.o" \

C_DEPS += \
./Sources/PileSys/PileCan/PileCatlMsg.d \
./Sources/PileSys/PileCan/PileDnMsg.d \
./Sources/PileSys/PileCan/PileUpMsg.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileCan/PileCatlMsg.o \
./Sources/PileSys/PileCan/PileDnMsg.o \
./Sources/PileSys/PileCan/PileUpMsg.o \

OBJS += \
./Sources/PileSys/PileCan/PileCatlMsg.o \
./Sources/PileSys/PileCan/PileDnMsg.o \
./Sources/PileSys/PileCan/PileUpMsg.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileCan/PileCatlMsg.o: ../Sources/PileSys/PileCan/PileCatlMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #233 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileCan/PileCatlMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileCan/PileCatlMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileCan/PileDnMsg.o: ../Sources/PileSys/PileCan/PileDnMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #234 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileCan/PileDnMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileCan/PileDnMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileCan/PileUpMsg.o: ../Sources/PileSys/PileCan/PileUpMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #235 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileCan/PileUpMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileCan/PileUpMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


