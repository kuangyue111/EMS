################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/signal/raise.c" \
"../Sources/BSPSys/Util/EWL_C/src/signal/signal.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/signal/raise.c \
../Sources/BSPSys/Util/EWL_C/src/signal/signal.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/signal/raise.o \
./Sources/BSPSys/Util/EWL_C/src/signal/signal.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/signal/raise.d" \
"./Sources/BSPSys/Util/EWL_C/src/signal/signal.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/signal/raise.o \
./Sources/BSPSys/Util/EWL_C/src/signal/signal.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/signal/raise.o" \
"./Sources/BSPSys/Util/EWL_C/src/signal/signal.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/signal/raise.d \
./Sources/BSPSys/Util/EWL_C/src/signal/signal.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/signal/raise.o: ../Sources/BSPSys/Util/EWL_C/src/signal/raise.c
	@echo 'Building file: $<'
	@echo 'Executing target #408 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/signal/raise.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/signal/raise.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/signal/signal.o: ../Sources/BSPSys/Util/EWL_C/src/signal/signal.c
	@echo 'Building file: $<'
	@echo 'Executing target #409 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/signal/signal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/signal/signal.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


