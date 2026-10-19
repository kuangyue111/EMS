################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Logics/CanUser/AuxDevMsg.c" \
"../Sources/Logics/CanUser/GTBmmMsg.c" \
"../Sources/Logics/CanUser/GroupMsg.c" \
"../Sources/Logics/CanUser/SlaveMsg.c" \

C_SRCS += \
../Sources/Logics/CanUser/AuxDevMsg.c \
../Sources/Logics/CanUser/GTBmmMsg.c \
../Sources/Logics/CanUser/GroupMsg.c \
../Sources/Logics/CanUser/SlaveMsg.c \

C_DEPS_QUOTED += \
"./Sources/Logics/CanUser/AuxDevMsg.d" \
"./Sources/Logics/CanUser/GTBmmMsg.d" \
"./Sources/Logics/CanUser/GroupMsg.d" \
"./Sources/Logics/CanUser/SlaveMsg.d" \

OBJS_QUOTED += \
"./Sources/Logics/CanUser/AuxDevMsg.o" \
"./Sources/Logics/CanUser/GTBmmMsg.o" \
"./Sources/Logics/CanUser/GroupMsg.o" \
"./Sources/Logics/CanUser/SlaveMsg.o" \

C_DEPS += \
./Sources/Logics/CanUser/AuxDevMsg.d \
./Sources/Logics/CanUser/GTBmmMsg.d \
./Sources/Logics/CanUser/GroupMsg.d \
./Sources/Logics/CanUser/SlaveMsg.d \

OBJS_OS_FORMAT += \
./Sources/Logics/CanUser/AuxDevMsg.o \
./Sources/Logics/CanUser/GTBmmMsg.o \
./Sources/Logics/CanUser/GroupMsg.o \
./Sources/Logics/CanUser/SlaveMsg.o \

OBJS += \
./Sources/Logics/CanUser/AuxDevMsg.o \
./Sources/Logics/CanUser/GTBmmMsg.o \
./Sources/Logics/CanUser/GroupMsg.o \
./Sources/Logics/CanUser/SlaveMsg.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Logics/CanUser/AuxDevMsg.o: ../Sources/Logics/CanUser/AuxDevMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #208 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/CanUser/AuxDevMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/CanUser/AuxDevMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/CanUser/GTBmmMsg.o: ../Sources/Logics/CanUser/GTBmmMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #209 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/CanUser/GTBmmMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/CanUser/GTBmmMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/CanUser/GroupMsg.o: ../Sources/Logics/CanUser/GroupMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #210 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/CanUser/GroupMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/CanUser/GroupMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Logics/CanUser/SlaveMsg.o: ../Sources/Logics/CanUser/SlaveMsg.c
	@echo 'Building file: $<'
	@echo 'Executing target #211 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Logics/CanUser/SlaveMsg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Logics/CanUser/SlaveMsg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


