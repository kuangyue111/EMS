################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/DEVADC.c" \
"../Sources/BSPSys/Device/DEVCAN.c" \
"../Sources/BSPSys/Device/DEVGPIO.c" \
"../Sources/BSPSys/Device/DEVSCI.c" \

C_SRCS += \
../Sources/BSPSys/Device/DEVADC.c \
../Sources/BSPSys/Device/DEVCAN.c \
../Sources/BSPSys/Device/DEVGPIO.c \
../Sources/BSPSys/Device/DEVSCI.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/DEVADC.d" \
"./Sources/BSPSys/Device/DEVCAN.d" \
"./Sources/BSPSys/Device/DEVGPIO.d" \
"./Sources/BSPSys/Device/DEVSCI.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/DEVADC.o" \
"./Sources/BSPSys/Device/DEVCAN.o" \
"./Sources/BSPSys/Device/DEVGPIO.o" \
"./Sources/BSPSys/Device/DEVSCI.o" \

C_DEPS += \
./Sources/BSPSys/Device/DEVADC.d \
./Sources/BSPSys/Device/DEVCAN.d \
./Sources/BSPSys/Device/DEVGPIO.d \
./Sources/BSPSys/Device/DEVSCI.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/DEVADC.o \
./Sources/BSPSys/Device/DEVCAN.o \
./Sources/BSPSys/Device/DEVGPIO.o \
./Sources/BSPSys/Device/DEVSCI.o \

OBJS += \
./Sources/BSPSys/Device/DEVADC.o \
./Sources/BSPSys/Device/DEVCAN.o \
./Sources/BSPSys/Device/DEVGPIO.o \
./Sources/BSPSys/Device/DEVSCI.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/DEVADC.o: ../Sources/BSPSys/Device/DEVADC.c
	@echo 'Building file: $<'
	@echo 'Executing target #102 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/DEVADC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/DEVADC.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Device/DEVCAN.o: ../Sources/BSPSys/Device/DEVCAN.c
	@echo 'Building file: $<'
	@echo 'Executing target #103 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/DEVCAN.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/DEVCAN.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Device/DEVGPIO.o: ../Sources/BSPSys/Device/DEVGPIO.c
	@echo 'Building file: $<'
	@echo 'Executing target #104 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/DEVGPIO.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/DEVGPIO.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Device/DEVSCI.o: ../Sources/BSPSys/Device/DEVSCI.c
	@echo 'Building file: $<'
	@echo 'Executing target #105 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/DEVSCI.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/DEVSCI.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


