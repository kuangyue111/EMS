################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/LTC2949_driver/LTC2949.c" \
"../Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.c" \

C_SRCS += \
../Sources/BSPSys/Device/LTC2949_driver/LTC2949.c \
../Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/LTC2949_driver/LTC2949.d" \
"./Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/LTC2949_driver/LTC2949.o" \
"./Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.o" \

C_DEPS += \
./Sources/BSPSys/Device/LTC2949_driver/LTC2949.d \
./Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/LTC2949_driver/LTC2949.o \
./Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.o \

OBJS += \
./Sources/BSPSys/Device/LTC2949_driver/LTC2949.o \
./Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/LTC2949_driver/LTC2949.o: ../Sources/BSPSys/Device/LTC2949_driver/LTC2949.c
	@echo 'Building file: $<'
	@echo 'Executing target #108 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/LTC2949_driver/LTC2949.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/LTC2949_driver/LTC2949.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.o: ../Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.c
	@echo 'Building file: $<'
	@echo 'Executing target #109 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/LTC2949_driver/ltcmuc_tools.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


