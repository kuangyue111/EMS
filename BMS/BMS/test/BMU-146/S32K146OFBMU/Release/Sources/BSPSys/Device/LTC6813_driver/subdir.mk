################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.c" \

C_SRCS += \
../Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.d" \

OBJS += \
./Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.o" \

C_DEPS += \
./Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.o: ../Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.c
	@echo 'Building file: $<'
	@echo 'Executing target #110 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/LTC6813_driver/LTC6813-1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


