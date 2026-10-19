################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.c" \

C_SRCS += \
../Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.d" \

OBJS += \
./Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.o" \

C_DEPS += \
./Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.o: ../Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.c
	@echo 'Building file: $<'
	@echo 'Executing target #114 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/SST26VF064_driver/SST26VF064.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


