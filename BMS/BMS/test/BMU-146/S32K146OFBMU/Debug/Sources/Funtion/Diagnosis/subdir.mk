################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/Diagnosis/DiagCheck.c" \
"../Sources/Funtion/Diagnosis/DiagFilter.c" \
"../Sources/Funtion/Diagnosis/DiagInterface.c" \
"../Sources/Funtion/Diagnosis/DiagMaker.c" \
"../Sources/Funtion/Diagnosis/DiagSos.c" \
"../Sources/Funtion/Diagnosis/DiagUser.c" \

C_SRCS += \
../Sources/Funtion/Diagnosis/DiagCheck.c \
../Sources/Funtion/Diagnosis/DiagFilter.c \
../Sources/Funtion/Diagnosis/DiagInterface.c \
../Sources/Funtion/Diagnosis/DiagMaker.c \
../Sources/Funtion/Diagnosis/DiagSos.c \
../Sources/Funtion/Diagnosis/DiagUser.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/Diagnosis/DiagCheck.d" \
"./Sources/Funtion/Diagnosis/DiagFilter.d" \
"./Sources/Funtion/Diagnosis/DiagInterface.d" \
"./Sources/Funtion/Diagnosis/DiagMaker.d" \
"./Sources/Funtion/Diagnosis/DiagSos.d" \
"./Sources/Funtion/Diagnosis/DiagUser.d" \

OBJS_QUOTED += \
"./Sources/Funtion/Diagnosis/DiagCheck.o" \
"./Sources/Funtion/Diagnosis/DiagFilter.o" \
"./Sources/Funtion/Diagnosis/DiagInterface.o" \
"./Sources/Funtion/Diagnosis/DiagMaker.o" \
"./Sources/Funtion/Diagnosis/DiagSos.o" \
"./Sources/Funtion/Diagnosis/DiagUser.o" \

C_DEPS += \
./Sources/Funtion/Diagnosis/DiagCheck.d \
./Sources/Funtion/Diagnosis/DiagFilter.d \
./Sources/Funtion/Diagnosis/DiagInterface.d \
./Sources/Funtion/Diagnosis/DiagMaker.d \
./Sources/Funtion/Diagnosis/DiagSos.d \
./Sources/Funtion/Diagnosis/DiagUser.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/Diagnosis/DiagCheck.o \
./Sources/Funtion/Diagnosis/DiagFilter.o \
./Sources/Funtion/Diagnosis/DiagInterface.o \
./Sources/Funtion/Diagnosis/DiagMaker.o \
./Sources/Funtion/Diagnosis/DiagSos.o \
./Sources/Funtion/Diagnosis/DiagUser.o \

OBJS += \
./Sources/Funtion/Diagnosis/DiagCheck.o \
./Sources/Funtion/Diagnosis/DiagFilter.o \
./Sources/Funtion/Diagnosis/DiagInterface.o \
./Sources/Funtion/Diagnosis/DiagMaker.o \
./Sources/Funtion/Diagnosis/DiagSos.o \
./Sources/Funtion/Diagnosis/DiagUser.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/Diagnosis/DiagCheck.o: ../Sources/Funtion/Diagnosis/DiagCheck.c
	@echo 'Building file: $<'
	@echo 'Executing target #148 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Diagnosis/DiagCheck.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Diagnosis/DiagCheck.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Diagnosis/DiagFilter.o: ../Sources/Funtion/Diagnosis/DiagFilter.c
	@echo 'Building file: $<'
	@echo 'Executing target #149 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Diagnosis/DiagFilter.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Diagnosis/DiagFilter.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Diagnosis/DiagInterface.o: ../Sources/Funtion/Diagnosis/DiagInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #150 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Diagnosis/DiagInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Diagnosis/DiagInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Diagnosis/DiagMaker.o: ../Sources/Funtion/Diagnosis/DiagMaker.c
	@echo 'Building file: $<'
	@echo 'Executing target #151 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Diagnosis/DiagMaker.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Diagnosis/DiagMaker.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Diagnosis/DiagSos.o: ../Sources/Funtion/Diagnosis/DiagSos.c
	@echo 'Building file: $<'
	@echo 'Executing target #152 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Diagnosis/DiagSos.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Diagnosis/DiagSos.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Diagnosis/DiagUser.o: ../Sources/Funtion/Diagnosis/DiagUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #153 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Diagnosis/DiagUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Diagnosis/DiagUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


