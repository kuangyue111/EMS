################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/DataCalc/CalcInterface.c" \
"../Sources/Funtion/DataCalc/CalcUtil.c" \
"../Sources/Funtion/DataCalc/CharactCalc.c" \
"../Sources/Funtion/DataCalc/CurrLimCalc.c" \
"../Sources/Funtion/DataCalc/CurrLimFall.c" \
"../Sources/Funtion/DataCalc/CurrLimPID.c" \
"../Sources/Funtion/DataCalc/VoltLimCalc.c" \

C_SRCS += \
../Sources/Funtion/DataCalc/CalcInterface.c \
../Sources/Funtion/DataCalc/CalcUtil.c \
../Sources/Funtion/DataCalc/CharactCalc.c \
../Sources/Funtion/DataCalc/CurrLimCalc.c \
../Sources/Funtion/DataCalc/CurrLimFall.c \
../Sources/Funtion/DataCalc/CurrLimPID.c \
../Sources/Funtion/DataCalc/VoltLimCalc.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/DataCalc/CalcInterface.d" \
"./Sources/Funtion/DataCalc/CalcUtil.d" \
"./Sources/Funtion/DataCalc/CharactCalc.d" \
"./Sources/Funtion/DataCalc/CurrLimCalc.d" \
"./Sources/Funtion/DataCalc/CurrLimFall.d" \
"./Sources/Funtion/DataCalc/CurrLimPID.d" \
"./Sources/Funtion/DataCalc/VoltLimCalc.d" \

OBJS_QUOTED += \
"./Sources/Funtion/DataCalc/CalcInterface.o" \
"./Sources/Funtion/DataCalc/CalcUtil.o" \
"./Sources/Funtion/DataCalc/CharactCalc.o" \
"./Sources/Funtion/DataCalc/CurrLimCalc.o" \
"./Sources/Funtion/DataCalc/CurrLimFall.o" \
"./Sources/Funtion/DataCalc/CurrLimPID.o" \
"./Sources/Funtion/DataCalc/VoltLimCalc.o" \

C_DEPS += \
./Sources/Funtion/DataCalc/CalcInterface.d \
./Sources/Funtion/DataCalc/CalcUtil.d \
./Sources/Funtion/DataCalc/CharactCalc.d \
./Sources/Funtion/DataCalc/CurrLimCalc.d \
./Sources/Funtion/DataCalc/CurrLimFall.d \
./Sources/Funtion/DataCalc/CurrLimPID.d \
./Sources/Funtion/DataCalc/VoltLimCalc.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/DataCalc/CalcInterface.o \
./Sources/Funtion/DataCalc/CalcUtil.o \
./Sources/Funtion/DataCalc/CharactCalc.o \
./Sources/Funtion/DataCalc/CurrLimCalc.o \
./Sources/Funtion/DataCalc/CurrLimFall.o \
./Sources/Funtion/DataCalc/CurrLimPID.o \
./Sources/Funtion/DataCalc/VoltLimCalc.o \

OBJS += \
./Sources/Funtion/DataCalc/CalcInterface.o \
./Sources/Funtion/DataCalc/CalcUtil.o \
./Sources/Funtion/DataCalc/CharactCalc.o \
./Sources/Funtion/DataCalc/CurrLimCalc.o \
./Sources/Funtion/DataCalc/CurrLimFall.o \
./Sources/Funtion/DataCalc/CurrLimPID.o \
./Sources/Funtion/DataCalc/VoltLimCalc.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/DataCalc/CalcInterface.o: ../Sources/Funtion/DataCalc/CalcInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #141 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/CalcInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/CalcInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/DataCalc/CalcUtil.o: ../Sources/Funtion/DataCalc/CalcUtil.c
	@echo 'Building file: $<'
	@echo 'Executing target #142 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/CalcUtil.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/CalcUtil.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/DataCalc/CharactCalc.o: ../Sources/Funtion/DataCalc/CharactCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #143 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/CharactCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/CharactCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/DataCalc/CurrLimCalc.o: ../Sources/Funtion/DataCalc/CurrLimCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #144 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/CurrLimCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/CurrLimCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/DataCalc/CurrLimFall.o: ../Sources/Funtion/DataCalc/CurrLimFall.c
	@echo 'Building file: $<'
	@echo 'Executing target #145 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/CurrLimFall.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/CurrLimFall.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/DataCalc/CurrLimPID.o: ../Sources/Funtion/DataCalc/CurrLimPID.c
	@echo 'Building file: $<'
	@echo 'Executing target #146 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/CurrLimPID.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/CurrLimPID.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/DataCalc/VoltLimCalc.o: ../Sources/Funtion/DataCalc/VoltLimCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #147 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/DataCalc/VoltLimCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/DataCalc/VoltLimCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


