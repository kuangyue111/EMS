################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Addition/LoadApp/LoadApp.c" \

C_SRCS += \
../Sources/Addition/LoadApp/LoadApp.c \

C_DEPS_QUOTED += \
"./Sources/Addition/LoadApp/LoadApp.d" \

OBJS_QUOTED += \
"./Sources/Addition/LoadApp/LoadApp.o" \

C_DEPS += \
./Sources/Addition/LoadApp/LoadApp.d \

OBJS_OS_FORMAT += \
./Sources/Addition/LoadApp/LoadApp.o \

OBJS += \
./Sources/Addition/LoadApp/LoadApp.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Addition/LoadApp/LoadApp.o: ../Sources/Addition/LoadApp/LoadApp.c
	@echo 'Building file: $<'
	@echo 'Executing target #92 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/LoadApp/LoadApp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/LoadApp/LoadApp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


