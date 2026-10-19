################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/Tactics/AuxDevTactic.c" \
"../Sources/Logics/Tactics/GroupDebug.c" \
"../Sources/Logics/Tactics/GroupMode.c" \
"../Sources/Logics/Tactics/GroupProtect.c" \
"../Sources/Logics/Tactics/GroupState.c" \
"../Sources/Logics/Tactics/GroupTactic.c" \
"../Sources/Logics/Tactics/TacInterface.c" \
"../Sources/Logics/Tactics/TacUser.c" \

C_SRCS += \
../Sources/Logics/Tactics/AuxDevTactic.c \
../Sources/Logics/Tactics/GroupDebug.c \
../Sources/Logics/Tactics/GroupMode.c \
../Sources/Logics/Tactics/GroupProtect.c \
../Sources/Logics/Tactics/GroupState.c \
../Sources/Logics/Tactics/GroupTactic.c \
../Sources/Logics/Tactics/TacInterface.c \
../Sources/Logics/Tactics/TacUser.c \

C_DEPS_QUOTED += \
"./Sources/Logics/Tactics/AuxDevTactic.d" \
"./Sources/Logics/Tactics/GroupDebug.d" \
"./Sources/Logics/Tactics/GroupMode.d" \
"./Sources/Logics/Tactics/GroupProtect.d" \
"./Sources/Logics/Tactics/GroupState.d" \
"./Sources/Logics/Tactics/GroupTactic.d" \
"./Sources/Logics/Tactics/TacInterface.d" \
"./Sources/Logics/Tactics/TacUser.d" \

OBJS_QUOTED += \
"./Sources/Logics/Tactics/AuxDevTactic.o" \
"./Sources/Logics/Tactics/GroupDebug.o" \
"./Sources/Logics/Tactics/GroupMode.o" \
"./Sources/Logics/Tactics/GroupProtect.o" \
"./Sources/Logics/Tactics/GroupState.o" \
"./Sources/Logics/Tactics/GroupTactic.o" \
"./Sources/Logics/Tactics/TacInterface.o" \
"./Sources/Logics/Tactics/TacUser.o" \

C_DEPS += \
./Sources/Logics/Tactics/AuxDevTactic.d \
./Sources/Logics/Tactics/GroupDebug.d \
./Sources/Logics/Tactics/GroupMode.d \
./Sources/Logics/Tactics/GroupProtect.d \
./Sources/Logics/Tactics/GroupState.d \
./Sources/Logics/Tactics/GroupTactic.d \
./Sources/Logics/Tactics/TacInterface.d \
./Sources/Logics/Tactics/TacUser.d \

OBJS_OS_FORMAT += \
./Sources/Logics/Tactics/AuxDevTactic.o \
./Sources/Logics/Tactics/GroupDebug.o \
./Sources/Logics/Tactics/GroupMode.o \
./Sources/Logics/Tactics/GroupProtect.o \
./Sources/Logics/Tactics/GroupState.o \
./Sources/Logics/Tactics/GroupTactic.o \
./Sources/Logics/Tactics/TacInterface.o \
./Sources/Logics/Tactics/TacUser.o \

OBJS += \
./Sources/Logics/Tactics/AuxDevTactic.o \
./Sources/Logics/Tactics/GroupDebug.o \
./Sources/Logics/Tactics/GroupMode.o \
./Sources/Logics/Tactics/GroupProtect.o \
./Sources/Logics/Tactics/GroupState.o \
./Sources/Logics/Tactics/GroupTactic.o \
./Sources/Logics/Tactics/TacInterface.o \
./Sources/Logics/Tactics/TacUser.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/Tactics/AuxDevTactic.o: ../Sources/Logics/Tactics/AuxDevTactic.c
	@echo 'Building file: $<'
	@echo 'Executing target #221 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/AuxDevTactic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/AuxDevTactic.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/GroupDebug.o: ../Sources/Logics/Tactics/GroupDebug.c
	@echo 'Building file: $<'
	@echo 'Executing target #222 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/GroupDebug.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/GroupDebug.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/GroupMode.o: ../Sources/Logics/Tactics/GroupMode.c
	@echo 'Building file: $<'
	@echo 'Executing target #223 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/GroupMode.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/GroupMode.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/GroupProtect.o: ../Sources/Logics/Tactics/GroupProtect.c
	@echo 'Building file: $<'
	@echo 'Executing target #224 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/GroupProtect.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/GroupProtect.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/GroupState.o: ../Sources/Logics/Tactics/GroupState.c
	@echo 'Building file: $<'
	@echo 'Executing target #225 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/GroupState.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/GroupState.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/GroupTactic.o: ../Sources/Logics/Tactics/GroupTactic.c
	@echo 'Building file: $<'
	@echo 'Executing target #226 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/GroupTactic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/GroupTactic.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/TacInterface.o: ../Sources/Logics/Tactics/TacInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #227 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/TacInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/TacInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/Tactics/TacUser.o: ../Sources/Logics/Tactics/TacUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #228 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/Tactics/TacUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/Tactics/TacUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


