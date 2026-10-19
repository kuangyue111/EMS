################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/Statistics/DodRcdStatis.c" \
"../Sources/Funtion/Statistics/EnergyStatis.c" \
"../Sources/Funtion/Statistics/StaInterface.c" \
"../Sources/Funtion/Statistics/StatisUser.c" \
"../Sources/Funtion/Statistics/TimesStatis.c" \

C_SRCS += \
../Sources/Funtion/Statistics/DodRcdStatis.c \
../Sources/Funtion/Statistics/EnergyStatis.c \
../Sources/Funtion/Statistics/StaInterface.c \
../Sources/Funtion/Statistics/StatisUser.c \
../Sources/Funtion/Statistics/TimesStatis.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/Statistics/DodRcdStatis.d" \
"./Sources/Funtion/Statistics/EnergyStatis.d" \
"./Sources/Funtion/Statistics/StaInterface.d" \
"./Sources/Funtion/Statistics/StatisUser.d" \
"./Sources/Funtion/Statistics/TimesStatis.d" \

OBJS_QUOTED += \
"./Sources/Funtion/Statistics/DodRcdStatis.o" \
"./Sources/Funtion/Statistics/EnergyStatis.o" \
"./Sources/Funtion/Statistics/StaInterface.o" \
"./Sources/Funtion/Statistics/StatisUser.o" \
"./Sources/Funtion/Statistics/TimesStatis.o" \

C_DEPS += \
./Sources/Funtion/Statistics/DodRcdStatis.d \
./Sources/Funtion/Statistics/EnergyStatis.d \
./Sources/Funtion/Statistics/StaInterface.d \
./Sources/Funtion/Statistics/StatisUser.d \
./Sources/Funtion/Statistics/TimesStatis.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/Statistics/DodRcdStatis.o \
./Sources/Funtion/Statistics/EnergyStatis.o \
./Sources/Funtion/Statistics/StaInterface.o \
./Sources/Funtion/Statistics/StatisUser.o \
./Sources/Funtion/Statistics/TimesStatis.o \

OBJS += \
./Sources/Funtion/Statistics/DodRcdStatis.o \
./Sources/Funtion/Statistics/EnergyStatis.o \
./Sources/Funtion/Statistics/StaInterface.o \
./Sources/Funtion/Statistics/StatisUser.o \
./Sources/Funtion/Statistics/TimesStatis.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/Statistics/DodRcdStatis.o: ../Sources/Funtion/Statistics/DodRcdStatis.c
	@echo 'Building file: $<'
	@echo 'Executing target #199 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/DodRcdStatis.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/DodRcdStatis.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/EnergyStatis.o: ../Sources/Funtion/Statistics/EnergyStatis.c
	@echo 'Building file: $<'
	@echo 'Executing target #200 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/EnergyStatis.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/EnergyStatis.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/StaInterface.o: ../Sources/Funtion/Statistics/StaInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #201 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/StaInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/StaInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/StatisUser.o: ../Sources/Funtion/Statistics/StatisUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #202 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/StatisUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/StatisUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Statistics/TimesStatis.o: ../Sources/Funtion/Statistics/TimesStatis.c
	@echo 'Building file: $<'
	@echo 'Executing target #203 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Statistics/TimesStatis.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Statistics/TimesStatis.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


