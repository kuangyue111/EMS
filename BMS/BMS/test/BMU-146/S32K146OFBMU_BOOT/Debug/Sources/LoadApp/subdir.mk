################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/LoadApp/LoadApp.c" \

C_SRCS += \
../Sources/LoadApp/LoadApp.c \

C_DEPS_QUOTED += \
"./Sources/LoadApp/LoadApp.d" \

OBJS_QUOTED += \
"./Sources/LoadApp/LoadApp.o" \

C_DEPS += \
./Sources/LoadApp/LoadApp.d \

OBJS_OS_FORMAT += \
./Sources/LoadApp/LoadApp.o \

OBJS += \
./Sources/LoadApp/LoadApp.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/LoadApp/LoadApp.o: ../Sources/LoadApp/LoadApp.c
	@echo 'Building file: $<'
	@echo 'Executing target #65 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/LoadApp/LoadApp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/LoadApp/LoadApp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


