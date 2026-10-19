################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.c" \
"../Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.c \
../Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.o \
./Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.d" \
"./Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.o \
./Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.o" \
"./Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.d \
./Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.o: ../Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.c
	@echo 'Building file: $<'
	@echo 'Executing target #135 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/critical_regions_pthreads.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.o: ../Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.c
	@echo 'Building file: $<'
	@echo 'Executing target #136 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/thread_local_data_pthreads.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


