################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Addition/UserProto/CatlMsg.c" \
"../Sources/Addition/UserProto/FrockMsg.c" \

C_SRCS += \
../Sources/Addition/UserProto/CatlMsg.c \
../Sources/Addition/UserProto/FrockMsg.c \

C_DEPS_QUOTED += \
"./Sources/Addition/UserProto/CatlMsg.d" \
"./Sources/Addition/UserProto/FrockMsg.d" \

OBJS_QUOTED += \
"./Sources/Addition/UserProto/CatlMsg.o" \
"./Sources/Addition/UserProto/FrockMsg.o" \

C_DEPS += \
./Sources/Addition/UserProto/CatlMsg.d \
./Sources/Addition/UserProto/FrockMsg.d \

OBJS_OS_FORMAT += \
./Sources/Addition/UserProto/CatlMsg.o \
./Sources/Addition/UserProto/FrockMsg.o \

OBJS += \
./Sources/Addition/UserProto/CatlMsg.o \
./Sources/Addition/UserProto/FrockMsg.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Addition/UserProto/CatlMsg.o: ../Sources/Addition/UserProto/CatlMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #95 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/UserProto/CatlMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/UserProto/CatlMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Addition/UserProto/FrockMsg.o: ../Sources/Addition/UserProto/FrockMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #96 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/UserProto/FrockMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/UserProto/FrockMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


