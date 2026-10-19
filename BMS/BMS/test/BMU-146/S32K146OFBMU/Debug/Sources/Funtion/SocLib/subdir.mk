################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/SocLib/CapEnerCorr.c" \
"../Sources/Funtion/SocLib/SocInterface.c" \
"../Sources/Funtion/SocLib/SocSoeCorr.c" \
"../Sources/Funtion/SocLib/SocSoeSync.c" \
"../Sources/Funtion/SocLib/SocUserAPI.c" \

C_SRCS += \
../Sources/Funtion/SocLib/CapEnerCorr.c \
../Sources/Funtion/SocLib/SocInterface.c \
../Sources/Funtion/SocLib/SocSoeCorr.c \
../Sources/Funtion/SocLib/SocSoeSync.c \
../Sources/Funtion/SocLib/SocUserAPI.c \

C_DEPS_QUOTED += \
"./Sources/Funtion/SocLib/CapEnerCorr.d" \
"./Sources/Funtion/SocLib/SocInterface.d" \
"./Sources/Funtion/SocLib/SocSoeCorr.d" \
"./Sources/Funtion/SocLib/SocSoeSync.d" \
"./Sources/Funtion/SocLib/SocUserAPI.d" \

OBJS_QUOTED += \
"./Sources/Funtion/SocLib/CapEnerCorr.o" \
"./Sources/Funtion/SocLib/SocInterface.o" \
"./Sources/Funtion/SocLib/SocSoeCorr.o" \
"./Sources/Funtion/SocLib/SocSoeSync.o" \
"./Sources/Funtion/SocLib/SocUserAPI.o" \

C_DEPS += \
./Sources/Funtion/SocLib/CapEnerCorr.d \
./Sources/Funtion/SocLib/SocInterface.d \
./Sources/Funtion/SocLib/SocSoeCorr.d \
./Sources/Funtion/SocLib/SocSoeSync.d \
./Sources/Funtion/SocLib/SocUserAPI.d \

OBJS_OS_FORMAT += \
./Sources/Funtion/SocLib/CapEnerCorr.o \
./Sources/Funtion/SocLib/SocInterface.o \
./Sources/Funtion/SocLib/SocSoeCorr.o \
./Sources/Funtion/SocLib/SocSoeSync.o \
./Sources/Funtion/SocLib/SocUserAPI.o \

OBJS += \
./Sources/Funtion/SocLib/CapEnerCorr.o \
./Sources/Funtion/SocLib/SocInterface.o \
./Sources/Funtion/SocLib/SocSoeCorr.o \
./Sources/Funtion/SocLib/SocSoeSync.o \
./Sources/Funtion/SocLib/SocUserAPI.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/SocLib/CapEnerCorr.o: ../Sources/Funtion/SocLib/CapEnerCorr.c
	@echo 'Building file: $<'
	@echo 'Executing target #194 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SocLib/CapEnerCorr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SocLib/CapEnerCorr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/SocLib/SocInterface.o: ../Sources/Funtion/SocLib/SocInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #195 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SocLib/SocInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SocLib/SocInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/SocLib/SocSoeCorr.o: ../Sources/Funtion/SocLib/SocSoeCorr.c
	@echo 'Building file: $<'
	@echo 'Executing target #196 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SocLib/SocSoeCorr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SocLib/SocSoeCorr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/SocLib/SocSoeSync.o: ../Sources/Funtion/SocLib/SocSoeSync.c
	@echo 'Building file: $<'
	@echo 'Executing target #197 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SocLib/SocSoeSync.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SocLib/SocSoeSync.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/SocLib/SocUserAPI.o: ../Sources/Funtion/SocLib/SocUserAPI.c
	@echo 'Building file: $<'
	@echo 'Executing target #198 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/SocLib/SocUserAPI.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/SocLib/SocUserAPI.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


