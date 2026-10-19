################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Device/MCP3913_driver/MCP3913.c" \

C_SRCS += \
../Sources/BSPSys/Device/MCP3913_driver/MCP3913.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Device/MCP3913_driver/MCP3913.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Device/MCP3913_driver/MCP3913.d" \

OBJS += \
./Sources/BSPSys/Device/MCP3913_driver/MCP3913.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Device/MCP3913_driver/MCP3913.o" \

C_DEPS += \
./Sources/BSPSys/Device/MCP3913_driver/MCP3913.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Device/MCP3913_driver/MCP3913.o: ../Sources/BSPSys/Device/MCP3913_driver/MCP3913.c
	@echo 'Building file: $<'
	@echo 'Executing target #112 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Device/MCP3913_driver/MCP3913.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Device/MCP3913_driver/MCP3913.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


