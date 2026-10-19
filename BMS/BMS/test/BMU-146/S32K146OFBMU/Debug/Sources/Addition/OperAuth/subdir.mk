################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Addition/OperAuth/Authority.c" \

C_SRCS += \
../Sources/Addition/OperAuth/Authority.c \

C_DEPS_QUOTED += \
"./Sources/Addition/OperAuth/Authority.d" \

OBJS_QUOTED += \
"./Sources/Addition/OperAuth/Authority.o" \

C_DEPS += \
./Sources/Addition/OperAuth/Authority.d \

OBJS_OS_FORMAT += \
./Sources/Addition/OperAuth/Authority.o \

OBJS += \
./Sources/Addition/OperAuth/Authority.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Addition/OperAuth/Authority.o: ../Sources/Addition/OperAuth/Authority.c
	@echo 'Building file: $<'
	@echo 'Executing target #93 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/OperAuth/Authority.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/OperAuth/Authority.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


