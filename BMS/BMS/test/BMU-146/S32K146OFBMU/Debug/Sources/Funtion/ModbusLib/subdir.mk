################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/ModbusLib/MbClient.c" \
"../Sources/Funtion/ModbusLib/MbHandle.c" \
"../Sources/Funtion/ModbusLib/MbInterface.c" \
"../Sources/Funtion/ModbusLib/MbQueue.c" \
"../Sources/Funtion/ModbusLib/MbServer.c" \
"../Sources/Funtion/ModbusLib/MbTriSend.c" \
"../Sources/Funtion/ModbusLib/MbUserDef.c" \
"../Sources/Funtion/ModbusLib/ModbusUser.c" \

C_SRCS += \
../Sources/Funtion/ModbusLib/MbClient.c \
../Sources/Funtion/ModbusLib/MbHandle.c \
../Sources/Funtion/ModbusLib/MbInterface.c \
../Sources/Funtion/ModbusLib/MbQueue.c \
../Sources/Funtion/ModbusLib/MbServer.c \
../Sources/Funtion/ModbusLib/MbTriSend.c \
../Sources/Funtion/ModbusLib/MbUserDef.c \
../Sources/Funtion/ModbusLib/ModbusUser.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/ModbusLib/MbClient.d" \
"./Sources/Funtion/ModbusLib/MbHandle.d" \
"./Sources/Funtion/ModbusLib/MbInterface.d" \
"./Sources/Funtion/ModbusLib/MbQueue.d" \
"./Sources/Funtion/ModbusLib/MbServer.d" \
"./Sources/Funtion/ModbusLib/MbTriSend.d" \
"./Sources/Funtion/ModbusLib/MbUserDef.d" \
"./Sources/Funtion/ModbusLib/ModbusUser.d" \

OBJS_QUOTED += \
"./Sources/Funtion/ModbusLib/MbClient.o" \
"./Sources/Funtion/ModbusLib/MbHandle.o" \
"./Sources/Funtion/ModbusLib/MbInterface.o" \
"./Sources/Funtion/ModbusLib/MbQueue.o" \
"./Sources/Funtion/ModbusLib/MbServer.o" \
"./Sources/Funtion/ModbusLib/MbTriSend.o" \
"./Sources/Funtion/ModbusLib/MbUserDef.o" \
"./Sources/Funtion/ModbusLib/ModbusUser.o" \

C_DEPS += \
./Sources/Funtion/ModbusLib/MbClient.d \
./Sources/Funtion/ModbusLib/MbHandle.d \
./Sources/Funtion/ModbusLib/MbInterface.d \
./Sources/Funtion/ModbusLib/MbQueue.d \
./Sources/Funtion/ModbusLib/MbServer.d \
./Sources/Funtion/ModbusLib/MbTriSend.d \
./Sources/Funtion/ModbusLib/MbUserDef.d \
./Sources/Funtion/ModbusLib/ModbusUser.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/ModbusLib/MbClient.o \
./Sources/Funtion/ModbusLib/MbHandle.o \
./Sources/Funtion/ModbusLib/MbInterface.o \
./Sources/Funtion/ModbusLib/MbQueue.o \
./Sources/Funtion/ModbusLib/MbServer.o \
./Sources/Funtion/ModbusLib/MbTriSend.o \
./Sources/Funtion/ModbusLib/MbUserDef.o \
./Sources/Funtion/ModbusLib/ModbusUser.o \

OBJS += \
./Sources/Funtion/ModbusLib/MbClient.o \
./Sources/Funtion/ModbusLib/MbHandle.o \
./Sources/Funtion/ModbusLib/MbInterface.o \
./Sources/Funtion/ModbusLib/MbQueue.o \
./Sources/Funtion/ModbusLib/MbServer.o \
./Sources/Funtion/ModbusLib/MbTriSend.o \
./Sources/Funtion/ModbusLib/MbUserDef.o \
./Sources/Funtion/ModbusLib/ModbusUser.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/ModbusLib/MbClient.o: ../Sources/Funtion/ModbusLib/MbClient.c
	@echo 'Building file: $<'
	@echo 'Executing target #169 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbClient.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbClient.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/MbHandle.o: ../Sources/Funtion/ModbusLib/MbHandle.c
	@echo 'Building file: $<'
	@echo 'Executing target #170 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbHandle.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbHandle.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/MbInterface.o: ../Sources/Funtion/ModbusLib/MbInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #171 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/MbQueue.o: ../Sources/Funtion/ModbusLib/MbQueue.c
	@echo 'Building file: $<'
	@echo 'Executing target #172 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbQueue.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbQueue.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/MbServer.o: ../Sources/Funtion/ModbusLib/MbServer.c
	@echo 'Building file: $<'
	@echo 'Executing target #173 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbServer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbServer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/MbTriSend.o: ../Sources/Funtion/ModbusLib/MbTriSend.c
	@echo 'Building file: $<'
	@echo 'Executing target #174 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbTriSend.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbTriSend.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/MbUserDef.o: ../Sources/Funtion/ModbusLib/MbUserDef.c
	@echo 'Building file: $<'
	@echo 'Executing target #175 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/MbUserDef.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/MbUserDef.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/ModbusLib/ModbusUser.o: ../Sources/Funtion/ModbusLib/ModbusUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #176 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/ModbusLib/ModbusUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/ModbusLib/ModbusUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


