################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/BSPUser/BSPISRHook.c" \
"../Sources/BSPSys/BSPUser/BSPUserAPI.c" \
"../Sources/BSPSys/BSPUser/BSPUserHook.c" \

C_SRCS += \
../Sources/BSPSys/BSPUser/BSPISRHook.c \
../Sources/BSPSys/BSPUser/BSPUserAPI.c \
../Sources/BSPSys/BSPUser/BSPUserHook.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/BSPUser/BSPISRHook.d" \
"./Sources/BSPSys/BSPUser/BSPUserAPI.d" \
"./Sources/BSPSys/BSPUser/BSPUserHook.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/BSPUser/BSPISRHook.o" \
"./Sources/BSPSys/BSPUser/BSPUserAPI.o" \
"./Sources/BSPSys/BSPUser/BSPUserHook.o" \

C_DEPS += \
./Sources/BSPSys/BSPUser/BSPISRHook.d \
./Sources/BSPSys/BSPUser/BSPUserAPI.d \
./Sources/BSPSys/BSPUser/BSPUserHook.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/BSPUser/BSPISRHook.o \
./Sources/BSPSys/BSPUser/BSPUserAPI.o \
./Sources/BSPSys/BSPUser/BSPUserHook.o \

OBJS += \
./Sources/BSPSys/BSPUser/BSPISRHook.o \
./Sources/BSPSys/BSPUser/BSPUserAPI.o \
./Sources/BSPSys/BSPUser/BSPUserHook.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/BSPUser/BSPISRHook.o: ../Sources/BSPSys/BSPUser/BSPISRHook.c
	@echo 'Building file: $<'
	@echo 'Executing target #98 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/BSPUser/BSPISRHook.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/BSPUser/BSPISRHook.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/BSPUser/BSPUserAPI.o: ../Sources/BSPSys/BSPUser/BSPUserAPI.c
	@echo 'Building file: $<'
	@echo 'Executing target #99 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/BSPUser/BSPUserAPI.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/BSPUser/BSPUserAPI.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/BSPUser/BSPUserHook.o: ../Sources/BSPSys/BSPUser/BSPUserHook.c
	@echo 'Building file: $<'
	@echo 'Executing target #100 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/BSPUser/BSPUserHook.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/BSPUser/BSPUserHook.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


