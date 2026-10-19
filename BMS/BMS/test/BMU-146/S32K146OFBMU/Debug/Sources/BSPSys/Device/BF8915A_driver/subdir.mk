################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/BF8915A_driver/BF8915A.c" \

C_SRCS += \
../Sources/BSPSys/Device/BF8915A_driver/BF8915A.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/BF8915A_driver/BF8915A.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/BF8915A_driver/BF8915A.o" \

C_DEPS += \
./Sources/BSPSys/Device/BF8915A_driver/BF8915A.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/BF8915A_driver/BF8915A.o \

OBJS += \
./Sources/BSPSys/Device/BF8915A_driver/BF8915A.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/BF8915A_driver/BF8915A.o: ../Sources/BSPSys/Device/BF8915A_driver/BF8915A.c
	@echo 'Building file: $<'
	@echo 'Executing target #101 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/BF8915A_driver/BF8915A.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/BF8915A_driver/BF8915A.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


