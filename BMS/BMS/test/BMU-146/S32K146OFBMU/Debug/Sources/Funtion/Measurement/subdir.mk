################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/Measurement/CellAFESamp.c" \
"../Sources/Funtion/Measurement/CurrSample.c" \
"../Sources/Funtion/Measurement/HigAFESamp.c" \
"../Sources/Funtion/Measurement/InsuCheck.c" \
"../Sources/Funtion/Measurement/LowADCSamp.c" \
"../Sources/Funtion/Measurement/PortCheck.c" \

C_SRCS += \
../Sources/Funtion/Measurement/CellAFESamp.c \
../Sources/Funtion/Measurement/CurrSample.c \
../Sources/Funtion/Measurement/HigAFESamp.c \
../Sources/Funtion/Measurement/InsuCheck.c \
../Sources/Funtion/Measurement/LowADCSamp.c \
../Sources/Funtion/Measurement/PortCheck.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/Measurement/CellAFESamp.d" \
"./Sources/Funtion/Measurement/CurrSample.d" \
"./Sources/Funtion/Measurement/HigAFESamp.d" \
"./Sources/Funtion/Measurement/InsuCheck.d" \
"./Sources/Funtion/Measurement/LowADCSamp.d" \
"./Sources/Funtion/Measurement/PortCheck.d" \

OBJS_QUOTED += \
"./Sources/Funtion/Measurement/CellAFESamp.o" \
"./Sources/Funtion/Measurement/CurrSample.o" \
"./Sources/Funtion/Measurement/HigAFESamp.o" \
"./Sources/Funtion/Measurement/InsuCheck.o" \
"./Sources/Funtion/Measurement/LowADCSamp.o" \
"./Sources/Funtion/Measurement/PortCheck.o" \

C_DEPS += \
./Sources/Funtion/Measurement/CellAFESamp.d \
./Sources/Funtion/Measurement/CurrSample.d \
./Sources/Funtion/Measurement/HigAFESamp.d \
./Sources/Funtion/Measurement/InsuCheck.d \
./Sources/Funtion/Measurement/LowADCSamp.d \
./Sources/Funtion/Measurement/PortCheck.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/Measurement/CellAFESamp.o \
./Sources/Funtion/Measurement/CurrSample.o \
./Sources/Funtion/Measurement/HigAFESamp.o \
./Sources/Funtion/Measurement/InsuCheck.o \
./Sources/Funtion/Measurement/LowADCSamp.o \
./Sources/Funtion/Measurement/PortCheck.o \

OBJS += \
./Sources/Funtion/Measurement/CellAFESamp.o \
./Sources/Funtion/Measurement/CurrSample.o \
./Sources/Funtion/Measurement/HigAFESamp.o \
./Sources/Funtion/Measurement/InsuCheck.o \
./Sources/Funtion/Measurement/LowADCSamp.o \
./Sources/Funtion/Measurement/PortCheck.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/Measurement/CellAFESamp.o: ../Sources/Funtion/Measurement/CellAFESamp.c
	@echo 'Building file: $<'
	@echo 'Executing target #163 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Measurement/CellAFESamp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Measurement/CellAFESamp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Measurement/CurrSample.o: ../Sources/Funtion/Measurement/CurrSample.c
	@echo 'Building file: $<'
	@echo 'Executing target #164 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Measurement/CurrSample.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Measurement/CurrSample.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Measurement/HigAFESamp.o: ../Sources/Funtion/Measurement/HigAFESamp.c
	@echo 'Building file: $<'
	@echo 'Executing target #165 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Measurement/HigAFESamp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Measurement/HigAFESamp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Measurement/InsuCheck.o: ../Sources/Funtion/Measurement/InsuCheck.c
	@echo 'Building file: $<'
	@echo 'Executing target #166 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Measurement/InsuCheck.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Measurement/InsuCheck.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Measurement/LowADCSamp.o: ../Sources/Funtion/Measurement/LowADCSamp.c
	@echo 'Building file: $<'
	@echo 'Executing target #167 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Measurement/LowADCSamp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Measurement/LowADCSamp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Measurement/PortCheck.o: ../Sources/Funtion/Measurement/PortCheck.c
	@echo 'Building file: $<'
	@echo 'Executing target #168 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Measurement/PortCheck.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Measurement/PortCheck.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


