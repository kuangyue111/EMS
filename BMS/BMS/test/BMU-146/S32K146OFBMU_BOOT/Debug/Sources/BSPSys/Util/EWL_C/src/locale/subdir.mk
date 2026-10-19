################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/locale/locale.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/locale/locale.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/locale/locale.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/locale/locale.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/locale/locale.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/locale/locale.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/locale/locale.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/locale/locale.o: ../Sources/BSPSys/Util/EWL_C/src/locale/locale.c
	@echo 'Building file: $<'
	@echo 'Executing target #175 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/locale/locale.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/locale/locale.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


