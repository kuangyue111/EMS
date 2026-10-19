################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/Recording/RecordHandle.c" \
"../Sources/Funtion/Recording/RecordInterface.c" \
"../Sources/Funtion/Recording/RecordUser.c" \

C_SRCS += \
../Sources/Funtion/Recording/RecordHandle.c \
../Sources/Funtion/Recording/RecordInterface.c \
../Sources/Funtion/Recording/RecordUser.c \

OBJS_OS_FORMAT += \
./Sources/Funtion/Recording/RecordHandle.o \
./Sources/Funtion/Recording/RecordInterface.o \
./Sources/Funtion/Recording/RecordUser.o \

C_DEPS_QUOTED += \
"./Sources/Funtion/Recording/RecordHandle.d" \
"./Sources/Funtion/Recording/RecordInterface.d" \
"./Sources/Funtion/Recording/RecordUser.d" \

OBJS += \
./Sources/Funtion/Recording/RecordHandle.o \
./Sources/Funtion/Recording/RecordInterface.o \
./Sources/Funtion/Recording/RecordUser.o \

OBJS_QUOTED += \
"./Sources/Funtion/Recording/RecordHandle.o" \
"./Sources/Funtion/Recording/RecordInterface.o" \
"./Sources/Funtion/Recording/RecordUser.o" \

C_DEPS += \
./Sources/Funtion/Recording/RecordHandle.d \
./Sources/Funtion/Recording/RecordInterface.d \
./Sources/Funtion/Recording/RecordUser.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/Recording/RecordHandle.o: ../Sources/Funtion/Recording/RecordHandle.c
	@echo 'Building file: $<'
	@echo 'Executing target #189 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Recording/RecordHandle.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Recording/RecordHandle.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Recording/RecordInterface.o: ../Sources/Funtion/Recording/RecordInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #190 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Recording/RecordInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Recording/RecordInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Recording/RecordUser.o: ../Sources/Funtion/Recording/RecordUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #191 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Recording/RecordUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Recording/RecordUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


