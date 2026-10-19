################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/sys/no_io.c" \
"../Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/sys/no_io.c \
../Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/sys/no_io.o \
./Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/sys/no_io.d" \
"./Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/sys/no_io.o \
./Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/sys/no_io.o" \
"./Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/sys/no_io.d \
./Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/sys/no_io.o: ../Sources/BSPSys/Util/EWL_C/src/sys/no_io.c
	@echo 'Building file: $<'
	@echo 'Executing target #554 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/sys/no_io.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/sys/no_io.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.o: ../Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.c
	@echo 'Building file: $<'
	@echo 'Executing target #555 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/sys/uart_console_io.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


