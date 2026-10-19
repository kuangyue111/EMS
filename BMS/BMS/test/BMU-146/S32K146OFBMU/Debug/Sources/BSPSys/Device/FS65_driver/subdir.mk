################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/FS65_driver/sbc_fs65.c" \
"../Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.c" \

C_SRCS += \
../Sources/BSPSys/Device/FS65_driver/sbc_fs65.c \
../Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/FS65_driver/sbc_fs65.d" \
"./Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/FS65_driver/sbc_fs65.o" \
"./Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.o" \

C_DEPS += \
./Sources/BSPSys/Device/FS65_driver/sbc_fs65.d \
./Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/FS65_driver/sbc_fs65.o \
./Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.o \

OBJS += \
./Sources/BSPSys/Device/FS65_driver/sbc_fs65.o \
./Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/FS65_driver/sbc_fs65.o: ../Sources/BSPSys/Device/FS65_driver/sbc_fs65.c
	@echo 'Building file: $<'
	@echo 'Executing target #106 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/FS65_driver/sbc_fs65.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/FS65_driver/sbc_fs65.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.o: ../Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.c
	@echo 'Building file: $<'
	@echo 'Executing target #107 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/FS65_driver/sbc_fs65_communication.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


