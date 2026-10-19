################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/SciUser/ModbusMap.c" \
"../Sources/Logics/SciUser/SciCtrlHandle.c" \
"../Sources/Logics/SciUser/SciInfoPoint.c" \
"../Sources/Logics/SciUser/SciParaHandle.c" \
"../Sources/Logics/SciUser/SciRcdRead.c" \

C_SRCS += \
../Sources/Logics/SciUser/ModbusMap.c \
../Sources/Logics/SciUser/SciCtrlHandle.c \
../Sources/Logics/SciUser/SciInfoPoint.c \
../Sources/Logics/SciUser/SciParaHandle.c \
../Sources/Logics/SciUser/SciRcdRead.c \

OBJS_OS_FORMAT += \
./Sources/Logics/SciUser/ModbusMap.o \
./Sources/Logics/SciUser/SciCtrlHandle.o \
./Sources/Logics/SciUser/SciInfoPoint.o \
./Sources/Logics/SciUser/SciParaHandle.o \
./Sources/Logics/SciUser/SciRcdRead.o \

C_DEPS_QUOTED += \
"./Sources/Logics/SciUser/ModbusMap.d" \
"./Sources/Logics/SciUser/SciCtrlHandle.d" \
"./Sources/Logics/SciUser/SciInfoPoint.d" \
"./Sources/Logics/SciUser/SciParaHandle.d" \
"./Sources/Logics/SciUser/SciRcdRead.d" \

OBJS += \
./Sources/Logics/SciUser/ModbusMap.o \
./Sources/Logics/SciUser/SciCtrlHandle.o \
./Sources/Logics/SciUser/SciInfoPoint.o \
./Sources/Logics/SciUser/SciParaHandle.o \
./Sources/Logics/SciUser/SciRcdRead.o \

OBJS_QUOTED += \
"./Sources/Logics/SciUser/ModbusMap.o" \
"./Sources/Logics/SciUser/SciCtrlHandle.o" \
"./Sources/Logics/SciUser/SciInfoPoint.o" \
"./Sources/Logics/SciUser/SciParaHandle.o" \
"./Sources/Logics/SciUser/SciRcdRead.o" \

C_DEPS += \
./Sources/Logics/SciUser/ModbusMap.d \
./Sources/Logics/SciUser/SciCtrlHandle.d \
./Sources/Logics/SciUser/SciInfoPoint.d \
./Sources/Logics/SciUser/SciParaHandle.d \
./Sources/Logics/SciUser/SciRcdRead.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/SciUser/ModbusMap.o: ../Sources/Logics/SciUser/ModbusMap.c
	@echo 'Building file: $<'
	@echo 'Executing target #204 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/ModbusMap.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/ModbusMap.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciCtrlHandle.o: ../Sources/Logics/SciUser/SciCtrlHandle.c
	@echo 'Building file: $<'
	@echo 'Executing target #205 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciCtrlHandle.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciCtrlHandle.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciInfoPoint.o: ../Sources/Logics/SciUser/SciInfoPoint.c
	@echo 'Building file: $<'
	@echo 'Executing target #206 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciInfoPoint.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciInfoPoint.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciParaHandle.o: ../Sources/Logics/SciUser/SciParaHandle.c
	@echo 'Building file: $<'
	@echo 'Executing target #207 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciParaHandle.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciParaHandle.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciRcdRead.o: ../Sources/Logics/SciUser/SciRcdRead.c
	@echo 'Building file: $<'
	@echo 'Executing target #208 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciRcdRead.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciRcdRead.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


