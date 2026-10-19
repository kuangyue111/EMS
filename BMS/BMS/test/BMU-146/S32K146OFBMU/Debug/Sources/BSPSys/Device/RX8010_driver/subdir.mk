################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/RX8010_driver/RX8010.c" \

C_SRCS += \
../Sources/BSPSys/Device/RX8010_driver/RX8010.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/RX8010_driver/RX8010.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/RX8010_driver/RX8010.o" \

C_DEPS += \
./Sources/BSPSys/Device/RX8010_driver/RX8010.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/RX8010_driver/RX8010.o \

OBJS += \
./Sources/BSPSys/Device/RX8010_driver/RX8010.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/RX8010_driver/RX8010.o: ../Sources/BSPSys/Device/RX8010_driver/RX8010.c
	@echo 'Building file: $<'
	@echo 'Executing target #113 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/RX8010_driver/RX8010.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/RX8010_driver/RX8010.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


