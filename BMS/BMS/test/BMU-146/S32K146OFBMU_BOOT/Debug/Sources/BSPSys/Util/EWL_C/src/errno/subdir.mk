################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/errno/errno.c" \
"../Sources/BSPSys/Util/EWL_C/src/errno/secure_error.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/errno/errno.c \
../Sources/BSPSys/Util/EWL_C/src/errno/secure_error.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/errno/errno.o \
./Sources/BSPSys/Util/EWL_C/src/errno/secure_error.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/errno/errno.d" \
"./Sources/BSPSys/Util/EWL_C/src/errno/secure_error.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/errno/errno.o \
./Sources/BSPSys/Util/EWL_C/src/errno/secure_error.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/errno/errno.o" \
"./Sources/BSPSys/Util/EWL_C/src/errno/secure_error.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/errno/errno.d \
./Sources/BSPSys/Util/EWL_C/src/errno/secure_error.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/errno/errno.o: ../Sources/BSPSys/Util/EWL_C/src/errno/errno.c
	@echo 'Building file: $<'
	@echo 'Executing target #152 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/errno/errno.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/errno/errno.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/errno/secure_error.o: ../Sources/BSPSys/Util/EWL_C/src/errno/secure_error.c
	@echo 'Building file: $<'
	@echo 'Executing target #153 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/errno/secure_error.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/errno/secure_error.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


