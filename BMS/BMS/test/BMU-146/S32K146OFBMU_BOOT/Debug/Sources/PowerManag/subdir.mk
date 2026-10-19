################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PowerManag/Fs6503_PowerManag.c" \
"../Sources/PowerManag/wait.c" \

C_SRCS += \
../Sources/PowerManag/Fs6503_PowerManag.c \
../Sources/PowerManag/wait.c \

C_DEPS_QUOTED += \
"./Sources/PowerManag/Fs6503_PowerManag.d" \
"./Sources/PowerManag/wait.d" \

OBJS_QUOTED += \
"./Sources/PowerManag/Fs6503_PowerManag.o" \
"./Sources/PowerManag/wait.o" \

C_DEPS += \
./Sources/PowerManag/Fs6503_PowerManag.d \
./Sources/PowerManag/wait.d \

OBJS_OS_FORMAT += \
./Sources/PowerManag/Fs6503_PowerManag.o \
./Sources/PowerManag/wait.o \

OBJS += \
./Sources/PowerManag/Fs6503_PowerManag.o \
./Sources/PowerManag/wait.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PowerManag/Fs6503_PowerManag.o: ../Sources/PowerManag/Fs6503_PowerManag.c
	@echo 'Building file: $<'
	@echo 'Executing target #67 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PowerManag/Fs6503_PowerManag.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PowerManag/Fs6503_PowerManag.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PowerManag/wait.o: ../Sources/PowerManag/wait.c
	@echo 'Building file: $<'
	@echo 'Executing target #68 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PowerManag/wait.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PowerManag/wait.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


