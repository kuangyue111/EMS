################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/extras/extras.c" \
"../Sources/BSPSys/Util/EWL_C/src/extras/old_posix.c" \
"../Sources/BSPSys/Util/EWL_C/src/extras/stat.c" \
"../Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.c" \
"../Sources/BSPSys/Util/EWL_C/src/extras/str_extra.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/extras/extras.c \
../Sources/BSPSys/Util/EWL_C/src/extras/old_posix.c \
../Sources/BSPSys/Util/EWL_C/src/extras/stat.c \
../Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.c \
../Sources/BSPSys/Util/EWL_C/src/extras/str_extra.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/extras/extras.o \
./Sources/BSPSys/Util/EWL_C/src/extras/old_posix.o \
./Sources/BSPSys/Util/EWL_C/src/extras/stat.o \
./Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.o \
./Sources/BSPSys/Util/EWL_C/src/extras/str_extra.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/extras/extras.d" \
"./Sources/BSPSys/Util/EWL_C/src/extras/old_posix.d" \
"./Sources/BSPSys/Util/EWL_C/src/extras/stat.d" \
"./Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.d" \
"./Sources/BSPSys/Util/EWL_C/src/extras/str_extra.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/extras/extras.o \
./Sources/BSPSys/Util/EWL_C/src/extras/old_posix.o \
./Sources/BSPSys/Util/EWL_C/src/extras/stat.o \
./Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.o \
./Sources/BSPSys/Util/EWL_C/src/extras/str_extra.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/extras/extras.o" \
"./Sources/BSPSys/Util/EWL_C/src/extras/old_posix.o" \
"./Sources/BSPSys/Util/EWL_C/src/extras/stat.o" \
"./Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.o" \
"./Sources/BSPSys/Util/EWL_C/src/extras/str_extra.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/extras/extras.d \
./Sources/BSPSys/Util/EWL_C/src/extras/old_posix.d \
./Sources/BSPSys/Util/EWL_C/src/extras/stat.d \
./Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.d \
./Sources/BSPSys/Util/EWL_C/src/extras/str_extra.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/extras/extras.o: ../Sources/BSPSys/Util/EWL_C/src/extras/extras.c
	@echo 'Building file: $<'
	@echo 'Executing target #170 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/extras/extras.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/extras/extras.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/extras/old_posix.o: ../Sources/BSPSys/Util/EWL_C/src/extras/old_posix.c
	@echo 'Building file: $<'
	@echo 'Executing target #171 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/extras/old_posix.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/extras/old_posix.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/extras/stat.o: ../Sources/BSPSys/Util/EWL_C/src/extras/stat.c
	@echo 'Building file: $<'
	@echo 'Executing target #172 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/extras/stat.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/extras/stat.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.o: ../Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.c
	@echo 'Building file: $<'
	@echo 'Executing target #173 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/extras/stdio_posix.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/extras/str_extra.o: ../Sources/BSPSys/Util/EWL_C/src/extras/str_extra.c
	@echo 'Building file: $<'
	@echo 'Executing target #174 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/extras/str_extra.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/extras/str_extra.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


