################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/Balance/BalanTactic.c" \

C_SRCS += \
../Sources/Logics/Balance/BalanTactic.c \

OBJS_OS_FORMAT += \
./Sources/Logics/Balance/BalanTactic.o \

C_DEPS_QUOTED += \
"./Sources/Logics/Balance/BalanTactic.d" \

OBJS += \
./Sources/Logics/Balance/BalanTactic.o \

OBJS_QUOTED += \
"./Sources/Logics/Balance/BalanTactic.o" \

C_DEPS += \
./Sources/Logics/Balance/BalanTactic.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/Balance/BalanTactic.o: ../Sources/Logics/Balance/BalanTactic.c
	@echo 'Building file: $<'
	@echo 'Executing target #199 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Balance/BalanTactic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Balance/BalanTactic.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


