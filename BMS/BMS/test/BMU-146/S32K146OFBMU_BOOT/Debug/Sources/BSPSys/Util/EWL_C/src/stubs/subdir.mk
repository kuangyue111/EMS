################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.c" \
"../Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.c" \
"../Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.c \
../Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.c \
../Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.o \
./Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.o \
./Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.d" \
"./Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.d" \
"./Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.o \
./Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.o \
./Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.o" \
"./Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.o" \
"./Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.d \
./Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.d \
./Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.o: ../Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.c
	@echo 'Building file: $<'
	@echo 'Executing target #551 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stubs/file_io.stub.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.o: ../Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.c
	@echo 'Building file: $<'
	@echo 'Executing target #552 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stubs/pool_alloc.stub.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.o: ../Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.c
	@echo 'Building file: $<'
	@echo 'Executing target #553 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stubs/time.stub.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


