################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/time/time.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/time/time.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/time/time.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/time/time.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/time/time.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/time/time.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/time/time.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/time/time.o: ../Sources/BSPSys/Util/EWL_C/src/time/time.c
	@echo 'Building file: $<'
	@echo 'Executing target #556 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/time/time.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/time/time.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


