################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Addition/UserDef/UserTactic.c" \

C_SRCS += \
../Sources/Addition/UserDef/UserTactic.c \

C_DEPS_QUOTED += \
"./Sources/Addition/UserDef/UserTactic.d" \

OBJS_QUOTED += \
"./Sources/Addition/UserDef/UserTactic.o" \

C_DEPS += \
./Sources/Addition/UserDef/UserTactic.d \

OBJS_OS_FORMAT += \
./Sources/Addition/UserDef/UserTactic.o \

OBJS += \
./Sources/Addition/UserDef/UserTactic.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Addition/UserDef/UserTactic.o: ../Sources/Addition/UserDef/UserTactic.c
	@echo 'Building file: $<'
	@echo 'Executing target #94 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/UserDef/UserTactic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/UserDef/UserTactic.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


