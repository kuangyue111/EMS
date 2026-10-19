################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Addition/Calibration/HigVoltCorr.c" \
"../Sources/Addition/Calibration/OutCurrCorr.c" \

C_SRCS += \
../Sources/Addition/Calibration/HigVoltCorr.c \
../Sources/Addition/Calibration/OutCurrCorr.c \

OBJS_OS_FORMAT += \
./Sources/Addition/Calibration/HigVoltCorr.o \
./Sources/Addition/Calibration/OutCurrCorr.o \

C_DEPS_QUOTED += \
"./Sources/Addition/Calibration/HigVoltCorr.d" \
"./Sources/Addition/Calibration/OutCurrCorr.d" \

OBJS += \
./Sources/Addition/Calibration/HigVoltCorr.o \
./Sources/Addition/Calibration/OutCurrCorr.o \

OBJS_QUOTED += \
"./Sources/Addition/Calibration/HigVoltCorr.o" \
"./Sources/Addition/Calibration/OutCurrCorr.o" \

C_DEPS += \
./Sources/Addition/Calibration/HigVoltCorr.d \
./Sources/Addition/Calibration/OutCurrCorr.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Addition/Calibration/HigVoltCorr.o: ../Sources/Addition/Calibration/HigVoltCorr.c
	@echo 'Building file: $<'
	@echo 'Executing target #90 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/Calibration/HigVoltCorr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/Calibration/HigVoltCorr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Addition/Calibration/OutCurrCorr.o: ../Sources/Addition/Calibration/OutCurrCorr.c
	@echo 'Building file: $<'
	@echo 'Executing target #91 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Addition/Calibration/OutCurrCorr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Addition/Calibration/OutCurrCorr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


