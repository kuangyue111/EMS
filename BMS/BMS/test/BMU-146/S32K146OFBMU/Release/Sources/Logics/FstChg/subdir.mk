################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/FstChg/ChgTactics.c" \

C_SRCS += \
../Sources/Logics/FstChg/ChgTactics.c \

OBJS_OS_FORMAT += \
./Sources/Logics/FstChg/ChgTactics.o \

C_DEPS_QUOTED += \
"./Sources/Logics/FstChg/ChgTactics.d" \

OBJS += \
./Sources/Logics/FstChg/ChgTactics.o \

OBJS_QUOTED += \
"./Sources/Logics/FstChg/ChgTactics.o" \

C_DEPS += \
./Sources/Logics/FstChg/ChgTactics.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/FstChg/ChgTactics.o: ../Sources/Logics/FstChg/ChgTactics.c
	@echo 'Building file: $<'
	@echo 'Executing target #203 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/FstChg/ChgTactics.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/FstChg/ChgTactics.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


