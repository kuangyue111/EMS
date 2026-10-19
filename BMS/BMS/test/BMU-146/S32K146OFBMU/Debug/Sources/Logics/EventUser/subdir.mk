################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/EventUser/EventUser.c" \
"../Sources/Logics/EventUser/RcdDisplayUser.c" \

C_SRCS += \
../Sources/Logics/EventUser/EventUser.c \
../Sources/Logics/EventUser/RcdDisplayUser.c \

C_DEPS_QUOTED += \
"./Sources/Logics/EventUser/EventUser.d" \
"./Sources/Logics/EventUser/RcdDisplayUser.d" \

OBJS_QUOTED += \
"./Sources/Logics/EventUser/EventUser.o" \
"./Sources/Logics/EventUser/RcdDisplayUser.o" \

C_DEPS += \
./Sources/Logics/EventUser/EventUser.d \
./Sources/Logics/EventUser/RcdDisplayUser.d \

OBJS_OS_FORMAT += \
./Sources/Logics/EventUser/EventUser.o \
./Sources/Logics/EventUser/RcdDisplayUser.o \

OBJS += \
./Sources/Logics/EventUser/EventUser.o \
./Sources/Logics/EventUser/RcdDisplayUser.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/EventUser/EventUser.o: ../Sources/Logics/EventUser/EventUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #212 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/EventUser/EventUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/EventUser/EventUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/EventUser/RcdDisplayUser.o: ../Sources/Logics/EventUser/RcdDisplayUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #213 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/EventUser/RcdDisplayUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/EventUser/RcdDisplayUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


