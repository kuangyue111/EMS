################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/Statistics/EnergyStatis.c" \
"../Sources/Funtion/Statistics/StaInterface.c" \
"../Sources/Funtion/Statistics/StatisUser.c" \
"../Sources/Funtion/Statistics/TimesStatis.c" \

C_SRCS += \
../Sources/Funtion/Statistics/EnergyStatis.c \
../Sources/Funtion/Statistics/StaInterface.c \
../Sources/Funtion/Statistics/StatisUser.c \
../Sources/Funtion/Statistics/TimesStatis.c \

OBJS_OS_FORMAT += \
./Sources/Funtion/Statistics/EnergyStatis.o \
./Sources/Funtion/Statistics/StaInterface.o \
./Sources/Funtion/Statistics/StatisUser.o \
./Sources/Funtion/Statistics/TimesStatis.o \

C_DEPS_QUOTED += \
"./Sources/Funtion/Statistics/EnergyStatis.d" \
"./Sources/Funtion/Statistics/StaInterface.d" \
"./Sources/Funtion/Statistics/StatisUser.d" \
"./Sources/Funtion/Statistics/TimesStatis.d" \

OBJS += \
./Sources/Funtion/Statistics/EnergyStatis.o \
./Sources/Funtion/Statistics/StaInterface.o \
./Sources/Funtion/Statistics/StatisUser.o \
./Sources/Funtion/Statistics/TimesStatis.o \

OBJS_QUOTED += \
"./Sources/Funtion/Statistics/EnergyStatis.o" \
"./Sources/Funtion/Statistics/StaInterface.o" \
"./Sources/Funtion/Statistics/StatisUser.o" \
"./Sources/Funtion/Statistics/TimesStatis.o" \

C_DEPS += \
./Sources/Funtion/Statistics/EnergyStatis.d \
./Sources/Funtion/Statistics/StaInterface.d \
./Sources/Funtion/Statistics/StatisUser.d \
./Sources/Funtion/Statistics/TimesStatis.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/Statistics/EnergyStatis.o: ../Sources/Funtion/Statistics/EnergyStatis.c
	@echo 'Building file: $<'
	@echo 'Executing target #192 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/EnergyStatis.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/EnergyStatis.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/StaInterface.o: ../Sources/Funtion/Statistics/StaInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #193 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/StaInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/StaInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/StatisUser.o: ../Sources/Funtion/Statistics/StatisUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #194 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/StatisUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/StatisUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/TimesStatis.o: ../Sources/Funtion/Statistics/TimesStatis.c
	@echo 'Building file: $<'
	@echo 'Executing target #195 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/TimesStatis.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/TimesStatis.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


