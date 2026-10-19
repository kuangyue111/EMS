################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/PowerManag/Fs6503_PowerManag.c" \
"../Sources/Funtion/PowerManag/wait.c" \

C_SRCS += \
../Sources/Funtion/PowerManag/Fs6503_PowerManag.c \
../Sources/Funtion/PowerManag/wait.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/PowerManag/Fs6503_PowerManag.d" \
"./Sources/Funtion/PowerManag/wait.d" \

OBJS_QUOTED += \
"./Sources/Funtion/PowerManag/Fs6503_PowerManag.o" \
"./Sources/Funtion/PowerManag/wait.o" \

C_DEPS += \
./Sources/Funtion/PowerManag/Fs6503_PowerManag.d \
./Sources/Funtion/PowerManag/wait.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/PowerManag/Fs6503_PowerManag.o \
./Sources/Funtion/PowerManag/wait.o \

OBJS += \
./Sources/Funtion/PowerManag/Fs6503_PowerManag.o \
./Sources/Funtion/PowerManag/wait.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/PowerManag/Fs6503_PowerManag.o: ../Sources/Funtion/PowerManag/Fs6503_PowerManag.c
	@echo 'Building file: $<'
	@echo 'Executing target #188 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/PowerManag/Fs6503_PowerManag.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/PowerManag/Fs6503_PowerManag.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/PowerManag/wait.o: ../Sources/Funtion/PowerManag/wait.c
	@echo 'Building file: $<'
	@echo 'Executing target #189 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/PowerManag/wait.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/PowerManag/wait.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


