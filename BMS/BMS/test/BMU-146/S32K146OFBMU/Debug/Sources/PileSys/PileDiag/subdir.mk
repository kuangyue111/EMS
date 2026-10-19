################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileDiag/PileDiagCheck.c" \
"../Sources/PileSys/PileDiag/PileDiagMaker.c" \
"../Sources/PileSys/PileDiag/PileDiagUser.c" \

C_SRCS += \
../Sources/PileSys/PileDiag/PileDiagCheck.c \
../Sources/PileSys/PileDiag/PileDiagMaker.c \
../Sources/PileSys/PileDiag/PileDiagUser.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileDiag/PileDiagCheck.d" \
"./Sources/PileSys/PileDiag/PileDiagMaker.d" \
"./Sources/PileSys/PileDiag/PileDiagUser.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileDiag/PileDiagCheck.o" \
"./Sources/PileSys/PileDiag/PileDiagMaker.o" \
"./Sources/PileSys/PileDiag/PileDiagUser.o" \

C_DEPS += \
./Sources/PileSys/PileDiag/PileDiagCheck.d \
./Sources/PileSys/PileDiag/PileDiagMaker.d \
./Sources/PileSys/PileDiag/PileDiagUser.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileDiag/PileDiagCheck.o \
./Sources/PileSys/PileDiag/PileDiagMaker.o \
./Sources/PileSys/PileDiag/PileDiagUser.o \

OBJS += \
./Sources/PileSys/PileDiag/PileDiagCheck.o \
./Sources/PileSys/PileDiag/PileDiagMaker.o \
./Sources/PileSys/PileDiag/PileDiagUser.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileDiag/PileDiagCheck.o: ../Sources/PileSys/PileDiag/PileDiagCheck.c
	@echo 'Building file: $<'
	@echo 'Executing target #236 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileDiag/PileDiagCheck.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileDiag/PileDiagCheck.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileDiag/PileDiagMaker.o: ../Sources/PileSys/PileDiag/PileDiagMaker.c
	@echo 'Building file: $<'
	@echo 'Executing target #237 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileDiag/PileDiagMaker.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileDiag/PileDiagMaker.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileDiag/PileDiagUser.o: ../Sources/PileSys/PileDiag/PileDiagUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #238 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileDiag/PileDiagUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileDiag/PileDiagUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


