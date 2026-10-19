################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/EnergyCalc/CapInfoCalc.c" \
"../Sources/Funtion/EnergyCalc/CellFadeCalc.c" \
"../Sources/Funtion/EnergyCalc/CurrIntegral.c" \
"../Sources/Funtion/EnergyCalc/EnerInfoCalc.c" \
"../Sources/Funtion/EnergyCalc/EnerInterface.c" \
"../Sources/Funtion/EnergyCalc/EnerUser.c" \
"../Sources/Funtion/EnergyCalc/SocDisplay.c" \
"../Sources/Funtion/EnergyCalc/SoeDisplay.c" \
"../Sources/Funtion/EnergyCalc/SohDisplay.c" \

C_SRCS += \
../Sources/Funtion/EnergyCalc/CapInfoCalc.c \
../Sources/Funtion/EnergyCalc/CellFadeCalc.c \
../Sources/Funtion/EnergyCalc/CurrIntegral.c \
../Sources/Funtion/EnergyCalc/EnerInfoCalc.c \
../Sources/Funtion/EnergyCalc/EnerInterface.c \
../Sources/Funtion/EnergyCalc/EnerUser.c \
../Sources/Funtion/EnergyCalc/SocDisplay.c \
../Sources/Funtion/EnergyCalc/SoeDisplay.c \
../Sources/Funtion/EnergyCalc/SohDisplay.c \

OBJS_OS_FORMAT += \
./Sources/Funtion/EnergyCalc/CapInfoCalc.o \
./Sources/Funtion/EnergyCalc/CellFadeCalc.o \
./Sources/Funtion/EnergyCalc/CurrIntegral.o \
./Sources/Funtion/EnergyCalc/EnerInfoCalc.o \
./Sources/Funtion/EnergyCalc/EnerInterface.o \
./Sources/Funtion/EnergyCalc/EnerUser.o \
./Sources/Funtion/EnergyCalc/SocDisplay.o \
./Sources/Funtion/EnergyCalc/SoeDisplay.o \
./Sources/Funtion/EnergyCalc/SohDisplay.o \

C_DEPS_QUOTED += \
"./Sources/Funtion/EnergyCalc/CapInfoCalc.d" \
"./Sources/Funtion/EnergyCalc/CellFadeCalc.d" \
"./Sources/Funtion/EnergyCalc/CurrIntegral.d" \
"./Sources/Funtion/EnergyCalc/EnerInfoCalc.d" \
"./Sources/Funtion/EnergyCalc/EnerInterface.d" \
"./Sources/Funtion/EnergyCalc/EnerUser.d" \
"./Sources/Funtion/EnergyCalc/SocDisplay.d" \
"./Sources/Funtion/EnergyCalc/SoeDisplay.d" \
"./Sources/Funtion/EnergyCalc/SohDisplay.d" \

OBJS += \
./Sources/Funtion/EnergyCalc/CapInfoCalc.o \
./Sources/Funtion/EnergyCalc/CellFadeCalc.o \
./Sources/Funtion/EnergyCalc/CurrIntegral.o \
./Sources/Funtion/EnergyCalc/EnerInfoCalc.o \
./Sources/Funtion/EnergyCalc/EnerInterface.o \
./Sources/Funtion/EnergyCalc/EnerUser.o \
./Sources/Funtion/EnergyCalc/SocDisplay.o \
./Sources/Funtion/EnergyCalc/SoeDisplay.o \
./Sources/Funtion/EnergyCalc/SohDisplay.o \

OBJS_QUOTED += \
"./Sources/Funtion/EnergyCalc/CapInfoCalc.o" \
"./Sources/Funtion/EnergyCalc/CellFadeCalc.o" \
"./Sources/Funtion/EnergyCalc/CurrIntegral.o" \
"./Sources/Funtion/EnergyCalc/EnerInfoCalc.o" \
"./Sources/Funtion/EnergyCalc/EnerInterface.o" \
"./Sources/Funtion/EnergyCalc/EnerUser.o" \
"./Sources/Funtion/EnergyCalc/SocDisplay.o" \
"./Sources/Funtion/EnergyCalc/SoeDisplay.o" \
"./Sources/Funtion/EnergyCalc/SohDisplay.o" \

C_DEPS += \
./Sources/Funtion/EnergyCalc/CapInfoCalc.d \
./Sources/Funtion/EnergyCalc/CellFadeCalc.d \
./Sources/Funtion/EnergyCalc/CurrIntegral.d \
./Sources/Funtion/EnergyCalc/EnerInfoCalc.d \
./Sources/Funtion/EnergyCalc/EnerInterface.d \
./Sources/Funtion/EnergyCalc/EnerUser.d \
./Sources/Funtion/EnergyCalc/SocDisplay.d \
./Sources/Funtion/EnergyCalc/SoeDisplay.d \
./Sources/Funtion/EnergyCalc/SohDisplay.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/EnergyCalc/CapInfoCalc.o: ../Sources/Funtion/EnergyCalc/CapInfoCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #153 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/CapInfoCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/CapInfoCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/CellFadeCalc.o: ../Sources/Funtion/EnergyCalc/CellFadeCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #154 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/CellFadeCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/CellFadeCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/CurrIntegral.o: ../Sources/Funtion/EnergyCalc/CurrIntegral.c
	@echo 'Building file: $<'
	@echo 'Executing target #155 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/CurrIntegral.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/CurrIntegral.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/EnerInfoCalc.o: ../Sources/Funtion/EnergyCalc/EnerInfoCalc.c
	@echo 'Building file: $<'
	@echo 'Executing target #156 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/EnerInfoCalc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/EnerInfoCalc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/EnerInterface.o: ../Sources/Funtion/EnergyCalc/EnerInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #157 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/EnerInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/EnerInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/EnerUser.o: ../Sources/Funtion/EnergyCalc/EnerUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #158 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/EnerUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/EnerUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/SocDisplay.o: ../Sources/Funtion/EnergyCalc/SocDisplay.c
	@echo 'Building file: $<'
	@echo 'Executing target #159 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/SocDisplay.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/SocDisplay.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/SoeDisplay.o: ../Sources/Funtion/EnergyCalc/SoeDisplay.c
	@echo 'Building file: $<'
	@echo 'Executing target #160 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/SoeDisplay.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/SoeDisplay.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/EnergyCalc/SohDisplay.o: ../Sources/Funtion/EnergyCalc/SohDisplay.c
	@echo 'Building file: $<'
	@echo 'Executing target #161 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/EnergyCalc/SohDisplay.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/EnergyCalc/SohDisplay.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


