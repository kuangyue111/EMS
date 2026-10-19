################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileTact/PileAutoCtrl.c" \
"../Sources/PileSys/PileTact/PileProtect.c" \
"../Sources/PileSys/PileTact/PileStartUp.c" \
"../Sources/PileSys/PileTact/PileSysDebug.c" \
"../Sources/PileSys/PileTact/PileSysMode.c" \
"../Sources/PileSys/PileTact/PileSysState.c" \
"../Sources/PileSys/PileTact/PileTactInterface.c" \
"../Sources/PileSys/PileTact/PileTactUser.c" \
"../Sources/PileSys/PileTact/PileTactics.c" \
"../Sources/PileSys/PileTact/PileWorkState.c" \

C_SRCS += \
../Sources/PileSys/PileTact/PileAutoCtrl.c \
../Sources/PileSys/PileTact/PileProtect.c \
../Sources/PileSys/PileTact/PileStartUp.c \
../Sources/PileSys/PileTact/PileSysDebug.c \
../Sources/PileSys/PileTact/PileSysMode.c \
../Sources/PileSys/PileTact/PileSysState.c \
../Sources/PileSys/PileTact/PileTactInterface.c \
../Sources/PileSys/PileTact/PileTactUser.c \
../Sources/PileSys/PileTact/PileTactics.c \
../Sources/PileSys/PileTact/PileWorkState.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileTact/PileAutoCtrl.d" \
"./Sources/PileSys/PileTact/PileProtect.d" \
"./Sources/PileSys/PileTact/PileStartUp.d" \
"./Sources/PileSys/PileTact/PileSysDebug.d" \
"./Sources/PileSys/PileTact/PileSysMode.d" \
"./Sources/PileSys/PileTact/PileSysState.d" \
"./Sources/PileSys/PileTact/PileTactInterface.d" \
"./Sources/PileSys/PileTact/PileTactUser.d" \
"./Sources/PileSys/PileTact/PileTactics.d" \
"./Sources/PileSys/PileTact/PileWorkState.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileTact/PileAutoCtrl.o" \
"./Sources/PileSys/PileTact/PileProtect.o" \
"./Sources/PileSys/PileTact/PileStartUp.o" \
"./Sources/PileSys/PileTact/PileSysDebug.o" \
"./Sources/PileSys/PileTact/PileSysMode.o" \
"./Sources/PileSys/PileTact/PileSysState.o" \
"./Sources/PileSys/PileTact/PileTactInterface.o" \
"./Sources/PileSys/PileTact/PileTactUser.o" \
"./Sources/PileSys/PileTact/PileTactics.o" \
"./Sources/PileSys/PileTact/PileWorkState.o" \

C_DEPS += \
./Sources/PileSys/PileTact/PileAutoCtrl.d \
./Sources/PileSys/PileTact/PileProtect.d \
./Sources/PileSys/PileTact/PileStartUp.d \
./Sources/PileSys/PileTact/PileSysDebug.d \
./Sources/PileSys/PileTact/PileSysMode.d \
./Sources/PileSys/PileTact/PileSysState.d \
./Sources/PileSys/PileTact/PileTactInterface.d \
./Sources/PileSys/PileTact/PileTactUser.d \
./Sources/PileSys/PileTact/PileTactics.d \
./Sources/PileSys/PileTact/PileWorkState.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileTact/PileAutoCtrl.o \
./Sources/PileSys/PileTact/PileProtect.o \
./Sources/PileSys/PileTact/PileStartUp.o \
./Sources/PileSys/PileTact/PileSysDebug.o \
./Sources/PileSys/PileTact/PileSysMode.o \
./Sources/PileSys/PileTact/PileSysState.o \
./Sources/PileSys/PileTact/PileTactInterface.o \
./Sources/PileSys/PileTact/PileTactUser.o \
./Sources/PileSys/PileTact/PileTactics.o \
./Sources/PileSys/PileTact/PileWorkState.o \

OBJS += \
./Sources/PileSys/PileTact/PileAutoCtrl.o \
./Sources/PileSys/PileTact/PileProtect.o \
./Sources/PileSys/PileTact/PileStartUp.o \
./Sources/PileSys/PileTact/PileSysDebug.o \
./Sources/PileSys/PileTact/PileSysMode.o \
./Sources/PileSys/PileTact/PileSysState.o \
./Sources/PileSys/PileTact/PileTactInterface.o \
./Sources/PileSys/PileTact/PileTactUser.o \
./Sources/PileSys/PileTact/PileTactics.o \
./Sources/PileSys/PileTact/PileWorkState.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileTact/PileAutoCtrl.o: ../Sources/PileSys/PileTact/PileAutoCtrl.c
	@echo 'Building file: $<'
	@echo 'Executing target #246 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileAutoCtrl.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileAutoCtrl.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileProtect.o: ../Sources/PileSys/PileTact/PileProtect.c
	@echo 'Building file: $<'
	@echo 'Executing target #247 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileProtect.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileProtect.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileStartUp.o: ../Sources/PileSys/PileTact/PileStartUp.c
	@echo 'Building file: $<'
	@echo 'Executing target #248 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileStartUp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileStartUp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileSysDebug.o: ../Sources/PileSys/PileTact/PileSysDebug.c
	@echo 'Building file: $<'
	@echo 'Executing target #249 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileSysDebug.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileSysDebug.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileSysMode.o: ../Sources/PileSys/PileTact/PileSysMode.c
	@echo 'Building file: $<'
	@echo 'Executing target #250 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileSysMode.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileSysMode.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileSysState.o: ../Sources/PileSys/PileTact/PileSysState.c
	@echo 'Building file: $<'
	@echo 'Executing target #251 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileSysState.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileSysState.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileTactInterface.o: ../Sources/PileSys/PileTact/PileTactInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #252 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileTactInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileTactInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileTactUser.o: ../Sources/PileSys/PileTact/PileTactUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #253 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileTactUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileTactUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileTactics.o: ../Sources/PileSys/PileTact/PileTactics.c
	@echo 'Building file: $<'
	@echo 'Executing target #254 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileTactics.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileTactics.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileTact/PileWorkState.o: ../Sources/PileSys/PileTact/PileWorkState.c
	@echo 'Building file: $<'
	@echo 'Executing target #255 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileTact/PileWorkState.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileTact/PileWorkState.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


