################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/SciUser/HeatModbusMap.c" \
"../Sources/Logics/SciUser/ModbusMap.c" \
"../Sources/Logics/SciUser/SciCtrlHandle.c" \
"../Sources/Logics/SciUser/SciInfoPoint.c" \
"../Sources/Logics/SciUser/SciParaHandle.c" \
"../Sources/Logics/SciUser/SciRcdRead.c" \

C_SRCS += \
../Sources/Logics/SciUser/HeatModbusMap.c \
../Sources/Logics/SciUser/ModbusMap.c \
../Sources/Logics/SciUser/SciCtrlHandle.c \
../Sources/Logics/SciUser/SciInfoPoint.c \
../Sources/Logics/SciUser/SciParaHandle.c \
../Sources/Logics/SciUser/SciRcdRead.c \

C_DEPS_QUOTED += \
"./Sources/Logics/SciUser/HeatModbusMap.d" \
"./Sources/Logics/SciUser/ModbusMap.d" \
"./Sources/Logics/SciUser/SciCtrlHandle.d" \
"./Sources/Logics/SciUser/SciInfoPoint.d" \
"./Sources/Logics/SciUser/SciParaHandle.d" \
"./Sources/Logics/SciUser/SciRcdRead.d" \

OBJS_QUOTED += \
"./Sources/Logics/SciUser/HeatModbusMap.o" \
"./Sources/Logics/SciUser/ModbusMap.o" \
"./Sources/Logics/SciUser/SciCtrlHandle.o" \
"./Sources/Logics/SciUser/SciInfoPoint.o" \
"./Sources/Logics/SciUser/SciParaHandle.o" \
"./Sources/Logics/SciUser/SciRcdRead.o" \

C_DEPS += \
./Sources/Logics/SciUser/HeatModbusMap.d \
./Sources/Logics/SciUser/ModbusMap.d \
./Sources/Logics/SciUser/SciCtrlHandle.d \
./Sources/Logics/SciUser/SciInfoPoint.d \
./Sources/Logics/SciUser/SciParaHandle.d \
./Sources/Logics/SciUser/SciRcdRead.d \

OBJS_OS_FORMAT += \
./Sources/Logics/SciUser/HeatModbusMap.o \
./Sources/Logics/SciUser/ModbusMap.o \
./Sources/Logics/SciUser/SciCtrlHandle.o \
./Sources/Logics/SciUser/SciInfoPoint.o \
./Sources/Logics/SciUser/SciParaHandle.o \
./Sources/Logics/SciUser/SciRcdRead.o \

OBJS += \
./Sources/Logics/SciUser/HeatModbusMap.o \
./Sources/Logics/SciUser/ModbusMap.o \
./Sources/Logics/SciUser/SciCtrlHandle.o \
./Sources/Logics/SciUser/SciInfoPoint.o \
./Sources/Logics/SciUser/SciParaHandle.o \
./Sources/Logics/SciUser/SciRcdRead.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/SciUser/HeatModbusMap.o: ../Sources/Logics/SciUser/HeatModbusMap.c
	@echo 'Building file: $<'
	@echo 'Executing target #215 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/HeatModbusMap.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/HeatModbusMap.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/ModbusMap.o: ../Sources/Logics/SciUser/ModbusMap.c
	@echo 'Building file: $<'
	@echo 'Executing target #216 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/ModbusMap.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/ModbusMap.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciCtrlHandle.o: ../Sources/Logics/SciUser/SciCtrlHandle.c
	@echo 'Building file: $<'
	@echo 'Executing target #217 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciCtrlHandle.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciCtrlHandle.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciInfoPoint.o: ../Sources/Logics/SciUser/SciInfoPoint.c
	@echo 'Building file: $<'
	@echo 'Executing target #218 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciInfoPoint.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciInfoPoint.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciParaHandle.o: ../Sources/Logics/SciUser/SciParaHandle.c
	@echo 'Building file: $<'
	@echo 'Executing target #219 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciParaHandle.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciParaHandle.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/SciUser/SciRcdRead.o: ../Sources/Logics/SciUser/SciRcdRead.c
	@echo 'Building file: $<'
	@echo 'Executing target #220 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/SciUser/SciRcdRead.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/SciUser/SciRcdRead.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


