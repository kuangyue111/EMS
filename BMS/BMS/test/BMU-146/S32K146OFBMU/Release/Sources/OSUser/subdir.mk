################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/OSUser/OSUserHOOK.c" \

C_SRCS += \
../Sources/OSUser/OSUserHOOK.c \

OBJS_OS_FORMAT += \
./Sources/OSUser/OSUserHOOK.o \

C_DEPS_QUOTED += \
"./Sources/OSUser/OSUserHOOK.d" \

OBJS += \
./Sources/OSUser/OSUserHOOK.o \

OBJS_QUOTED += \
"./Sources/OSUser/OSUserHOOK.o" \

C_DEPS += \
./Sources/OSUser/OSUserHOOK.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/OSUser/OSUserHOOK.o: ../Sources/OSUser/OSUserHOOK.c
	@echo 'Building file: $<'
	@echo 'Executing target #216 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/OSUser/OSUserHOOK.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/OSUser/OSUserHOOK.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


