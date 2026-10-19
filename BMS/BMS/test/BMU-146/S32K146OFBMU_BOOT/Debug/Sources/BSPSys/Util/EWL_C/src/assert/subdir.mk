################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/assert/assert.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/assert/assert.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/assert/assert.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/assert/assert.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/assert/assert.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/assert/assert.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/assert/assert.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/assert/assert.o: ../Sources/BSPSys/Util/EWL_C/src/assert/assert.c
	@echo 'Building file: $<'
	@echo 'Executing target #134 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/assert/assert.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/assert/assert.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


