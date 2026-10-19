################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/BSPCfg/BSPConfig.c" \

C_SRCS += \
../Sources/BSPSys/BSPCfg/BSPConfig.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/BSPCfg/BSPConfig.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/BSPCfg/BSPConfig.d" \

OBJS += \
./Sources/BSPSys/BSPCfg/BSPConfig.o \

OBJS_QUOTED += \
"./Sources/BSPSys/BSPCfg/BSPConfig.o" \

C_DEPS += \
./Sources/BSPSys/BSPCfg/BSPConfig.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/BSPCfg/BSPConfig.o: ../Sources/BSPSys/BSPCfg/BSPConfig.c
	@echo 'Building file: $<'
	@echo 'Executing target #98 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/BSPCfg/BSPConfig.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/BSPCfg/BSPConfig.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


