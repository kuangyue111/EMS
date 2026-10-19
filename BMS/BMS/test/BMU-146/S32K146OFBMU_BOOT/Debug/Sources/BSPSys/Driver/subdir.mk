################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Driver/BSPCAN.c" \
"../Sources/BSPSys/Driver/BSPEEPROM.c" \
"../Sources/BSPSys/Driver/BSPFLASH.c" \
"../Sources/BSPSys/Driver/BSPFTM.c" \
"../Sources/BSPSys/Driver/BSPGPIO.c" \
"../Sources/BSPSys/Driver/BSPIRQ.c" \
"../Sources/BSPSys/Driver/BSPLPIT.c" \
"../Sources/BSPSys/Driver/BSPMCG.c" \
"../Sources/BSPSys/Driver/BSPSPI.c" \
"../Sources/BSPSys/Driver/BSPUART.c" \
"../Sources/BSPSys/Driver/BSPWDOG.c" \

C_SRCS += \
../Sources/BSPSys/Driver/BSPCAN.c \
../Sources/BSPSys/Driver/BSPEEPROM.c \
../Sources/BSPSys/Driver/BSPFLASH.c \
../Sources/BSPSys/Driver/BSPFTM.c \
../Sources/BSPSys/Driver/BSPGPIO.c \
../Sources/BSPSys/Driver/BSPIRQ.c \
../Sources/BSPSys/Driver/BSPLPIT.c \
../Sources/BSPSys/Driver/BSPMCG.c \
../Sources/BSPSys/Driver/BSPSPI.c \
../Sources/BSPSys/Driver/BSPUART.c \
../Sources/BSPSys/Driver/BSPWDOG.c \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Driver/BSPCAN.d" \
"./Sources/BSPSys/Driver/BSPEEPROM.d" \
"./Sources/BSPSys/Driver/BSPFLASH.d" \
"./Sources/BSPSys/Driver/BSPFTM.d" \
"./Sources/BSPSys/Driver/BSPGPIO.d" \
"./Sources/BSPSys/Driver/BSPIRQ.d" \
"./Sources/BSPSys/Driver/BSPLPIT.d" \
"./Sources/BSPSys/Driver/BSPMCG.d" \
"./Sources/BSPSys/Driver/BSPSPI.d" \
"./Sources/BSPSys/Driver/BSPUART.d" \
"./Sources/BSPSys/Driver/BSPWDOG.d" \

OBJS_QUOTED += \
"./Sources/BSPSys/Driver/BSPCAN.o" \
"./Sources/BSPSys/Driver/BSPEEPROM.o" \
"./Sources/BSPSys/Driver/BSPFLASH.o" \
"./Sources/BSPSys/Driver/BSPFTM.o" \
"./Sources/BSPSys/Driver/BSPGPIO.o" \
"./Sources/BSPSys/Driver/BSPIRQ.o" \
"./Sources/BSPSys/Driver/BSPLPIT.o" \
"./Sources/BSPSys/Driver/BSPMCG.o" \
"./Sources/BSPSys/Driver/BSPSPI.o" \
"./Sources/BSPSys/Driver/BSPUART.o" \
"./Sources/BSPSys/Driver/BSPWDOG.o" \

C_DEPS += \
./Sources/BSPSys/Driver/BSPCAN.d \
./Sources/BSPSys/Driver/BSPEEPROM.d \
./Sources/BSPSys/Driver/BSPFLASH.d \
./Sources/BSPSys/Driver/BSPFTM.d \
./Sources/BSPSys/Driver/BSPGPIO.d \
./Sources/BSPSys/Driver/BSPIRQ.d \
./Sources/BSPSys/Driver/BSPLPIT.d \
./Sources/BSPSys/Driver/BSPMCG.d \
./Sources/BSPSys/Driver/BSPSPI.d \
./Sources/BSPSys/Driver/BSPUART.d \
./Sources/BSPSys/Driver/BSPWDOG.d \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Driver/BSPCAN.o \
./Sources/BSPSys/Driver/BSPEEPROM.o \
./Sources/BSPSys/Driver/BSPFLASH.o \
./Sources/BSPSys/Driver/BSPFTM.o \
./Sources/BSPSys/Driver/BSPGPIO.o \
./Sources/BSPSys/Driver/BSPIRQ.o \
./Sources/BSPSys/Driver/BSPLPIT.o \
./Sources/BSPSys/Driver/BSPMCG.o \
./Sources/BSPSys/Driver/BSPSPI.o \
./Sources/BSPSys/Driver/BSPUART.o \
./Sources/BSPSys/Driver/BSPWDOG.o \

OBJS += \
./Sources/BSPSys/Driver/BSPCAN.o \
./Sources/BSPSys/Driver/BSPEEPROM.o \
./Sources/BSPSys/Driver/BSPFLASH.o \
./Sources/BSPSys/Driver/BSPFTM.o \
./Sources/BSPSys/Driver/BSPGPIO.o \
./Sources/BSPSys/Driver/BSPIRQ.o \
./Sources/BSPSys/Driver/BSPLPIT.o \
./Sources/BSPSys/Driver/BSPMCG.o \
./Sources/BSPSys/Driver/BSPSPI.o \
./Sources/BSPSys/Driver/BSPUART.o \
./Sources/BSPSys/Driver/BSPWDOG.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Driver/BSPCAN.o: ../Sources/BSPSys/Driver/BSPCAN.c
	@echo 'Building file: $<'
	@echo 'Executing target #53 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPCAN.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPCAN.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPEEPROM.o: ../Sources/BSPSys/Driver/BSPEEPROM.c
	@echo 'Building file: $<'
	@echo 'Executing target #54 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPEEPROM.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPEEPROM.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPFLASH.o: ../Sources/BSPSys/Driver/BSPFLASH.c
	@echo 'Building file: $<'
	@echo 'Executing target #55 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPFLASH.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPFLASH.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPFTM.o: ../Sources/BSPSys/Driver/BSPFTM.c
	@echo 'Building file: $<'
	@echo 'Executing target #56 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPFTM.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPFTM.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPGPIO.o: ../Sources/BSPSys/Driver/BSPGPIO.c
	@echo 'Building file: $<'
	@echo 'Executing target #57 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPGPIO.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPGPIO.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPIRQ.o: ../Sources/BSPSys/Driver/BSPIRQ.c
	@echo 'Building file: $<'
	@echo 'Executing target #58 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPIRQ.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPIRQ.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPLPIT.o: ../Sources/BSPSys/Driver/BSPLPIT.c
	@echo 'Building file: $<'
	@echo 'Executing target #59 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPLPIT.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPLPIT.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPMCG.o: ../Sources/BSPSys/Driver/BSPMCG.c
	@echo 'Building file: $<'
	@echo 'Executing target #60 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPMCG.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPMCG.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPSPI.o: ../Sources/BSPSys/Driver/BSPSPI.c
	@echo 'Building file: $<'
	@echo 'Executing target #61 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPSPI.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPSPI.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPUART.o: ../Sources/BSPSys/Driver/BSPUART.c
	@echo 'Building file: $<'
	@echo 'Executing target #62 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPUART.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPUART.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Driver/BSPWDOG.o: ../Sources/BSPSys/Driver/BSPWDOG.c
	@echo 'Building file: $<'
	@echo 'Executing target #63 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Driver/BSPWDOG.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Driver/BSPWDOG.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


