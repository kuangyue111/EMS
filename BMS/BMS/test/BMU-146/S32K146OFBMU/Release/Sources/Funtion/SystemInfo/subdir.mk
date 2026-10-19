################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/SystemInfo/GroupInfo.c" \
"../Sources/Funtion/SystemInfo/PileInfo.c" \
"../Sources/Funtion/SystemInfo/PublicInfo.c" \

C_SRCS += \
../Sources/Funtion/SystemInfo/GroupInfo.c \
../Sources/Funtion/SystemInfo/PileInfo.c \
../Sources/Funtion/SystemInfo/PublicInfo.c \

OBJS_OS_FORMAT += \
./Sources/Funtion/SystemInfo/GroupInfo.o \
./Sources/Funtion/SystemInfo/PileInfo.o \
./Sources/Funtion/SystemInfo/PublicInfo.o \

C_DEPS_QUOTED += \
"./Sources/Funtion/SystemInfo/GroupInfo.d" \
"./Sources/Funtion/SystemInfo/PileInfo.d" \
"./Sources/Funtion/SystemInfo/PublicInfo.d" \

OBJS += \
./Sources/Funtion/SystemInfo/GroupInfo.o \
./Sources/Funtion/SystemInfo/PileInfo.o \
./Sources/Funtion/SystemInfo/PublicInfo.o \

OBJS_QUOTED += \
"./Sources/Funtion/SystemInfo/GroupInfo.o" \
"./Sources/Funtion/SystemInfo/PileInfo.o" \
"./Sources/Funtion/SystemInfo/PublicInfo.o" \

C_DEPS += \
./Sources/Funtion/SystemInfo/GroupInfo.d \
./Sources/Funtion/SystemInfo/PileInfo.d \
./Sources/Funtion/SystemInfo/PublicInfo.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/SystemInfo/GroupInfo.o: ../Sources/Funtion/SystemInfo/GroupInfo.c
	@echo 'Building file: $<'
	@echo 'Executing target #196 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SystemInfo/GroupInfo.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SystemInfo/GroupInfo.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/SystemInfo/PileInfo.o: ../Sources/Funtion/SystemInfo/PileInfo.c
	@echo 'Building file: $<'
	@echo 'Executing target #197 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SystemInfo/PileInfo.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SystemInfo/PileInfo.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/SystemInfo/PublicInfo.o: ../Sources/Funtion/SystemInfo/PublicInfo.c
	@echo 'Building file: $<'
	@echo 'Executing target #198 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SystemInfo/PublicInfo.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SystemInfo/PublicInfo.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


