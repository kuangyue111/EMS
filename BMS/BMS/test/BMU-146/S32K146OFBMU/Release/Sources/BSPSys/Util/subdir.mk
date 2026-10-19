################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/BSPQueue.c" \

C_SRCS += \
../Sources/BSPSys/Util/BSPQueue.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/BSPQueue.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/BSPQueue.d" \

OBJS += \
./Sources/BSPSys/Util/BSPQueue.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/BSPQueue.o" \

C_DEPS += \
./Sources/BSPSys/Util/BSPQueue.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/BSPQueue.o: ../Sources/BSPSys/Util/BSPQueue.c
	@echo 'Building file: $<'
	@echo 'Executing target #132 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/BSPQueue.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/BSPQueue.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


