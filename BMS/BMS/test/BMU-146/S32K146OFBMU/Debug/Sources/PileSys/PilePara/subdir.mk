################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PilePara/PilePara.c" \
"../Sources/PileSys/PilePara/PileParaRd.c" \
"../Sources/PileSys/PilePara/PileParaWr.c" \

C_SRCS += \
../Sources/PileSys/PilePara/PilePara.c \
../Sources/PileSys/PilePara/PileParaRd.c \
../Sources/PileSys/PilePara/PileParaWr.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PilePara/PilePara.d" \
"./Sources/PileSys/PilePara/PileParaRd.d" \
"./Sources/PileSys/PilePara/PileParaWr.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PilePara/PilePara.o" \
"./Sources/PileSys/PilePara/PileParaRd.o" \
"./Sources/PileSys/PilePara/PileParaWr.o" \

C_DEPS += \
./Sources/PileSys/PilePara/PilePara.d \
./Sources/PileSys/PilePara/PileParaRd.d \
./Sources/PileSys/PilePara/PileParaWr.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PilePara/PilePara.o \
./Sources/PileSys/PilePara/PileParaRd.o \
./Sources/PileSys/PilePara/PileParaWr.o \

OBJS += \
./Sources/PileSys/PilePara/PilePara.o \
./Sources/PileSys/PilePara/PileParaRd.o \
./Sources/PileSys/PilePara/PileParaWr.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PilePara/PilePara.o: ../Sources/PileSys/PilePara/PilePara.c
	@echo 'Building file: $<'
	@echo 'Executing target #239 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PilePara/PilePara.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PilePara/PilePara.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PilePara/PileParaRd.o: ../Sources/PileSys/PilePara/PileParaRd.c
	@echo 'Building file: $<'
	@echo 'Executing target #240 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PilePara/PileParaRd.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PilePara/PileParaRd.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PilePara/PileParaWr.o: ../Sources/PileSys/PilePara/PileParaWr.c
	@echo 'Building file: $<'
	@echo 'Executing target #241 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PilePara/PileParaWr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PilePara/PileParaWr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


