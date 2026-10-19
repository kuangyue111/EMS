################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/M24512_driver/EepM24512.c" \

C_SRCS += \
../Sources/BSPSys/Device/M24512_driver/EepM24512.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/M24512_driver/EepM24512.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/M24512_driver/EepM24512.o" \

C_DEPS += \
./Sources/BSPSys/Device/M24512_driver/EepM24512.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/M24512_driver/EepM24512.o \

OBJS += \
./Sources/BSPSys/Device/M24512_driver/EepM24512.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/M24512_driver/EepM24512.o: ../Sources/BSPSys/Device/M24512_driver/EepM24512.c
	@echo 'Building file: $<'
	@echo 'Executing target #111 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/M24512_driver/EepM24512.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/M24512_driver/EepM24512.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


