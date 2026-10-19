################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/EventUser/EventUser.c" \

C_SRCS += \
../Sources/Logics/EventUser/EventUser.c \

OBJS_OS_FORMAT += \
./Sources/Logics/EventUser/EventUser.o \

C_DEPS_QUOTED += \
"./Sources/Logics/EventUser/EventUser.d" \

OBJS += \
./Sources/Logics/EventUser/EventUser.o \

OBJS_QUOTED += \
"./Sources/Logics/EventUser/EventUser.o" \

C_DEPS += \
./Sources/Logics/EventUser/EventUser.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/EventUser/EventUser.o: ../Sources/Logics/EventUser/EventUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #202 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/EventUser/EventUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/EventUser/EventUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


