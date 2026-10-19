################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileStati/PileEnerStat.c" \

C_SRCS += \
../Sources/PileSys/PileStati/PileEnerStat.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileStati/PileEnerStat.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileStati/PileEnerStat.o" \

C_DEPS += \
./Sources/PileSys/PileStati/PileEnerStat.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileStati/PileEnerStat.o \

OBJS += \
./Sources/PileSys/PileStati/PileEnerStat.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileStati/PileEnerStat.o: ../Sources/PileSys/PileStati/PileEnerStat.c
	@echo 'Building file: $<'
	@echo 'Executing target #245 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileStati/PileEnerStat.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileStati/PileEnerStat.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


