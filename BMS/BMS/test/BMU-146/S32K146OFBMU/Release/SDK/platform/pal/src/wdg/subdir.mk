################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../SDK/platform/pal/src/wdg/wdg_pal.c" \

C_SRCS += \
../SDK/platform/pal/src/wdg/wdg_pal.c \

OBJS_OS_FORMAT += \
./SDK/platform/pal/src/wdg/wdg_pal.o \

C_DEPS_QUOTED += \
"./SDK/platform/pal/src/wdg/wdg_pal.d" \

OBJS += \
./SDK/platform/pal/src/wdg/wdg_pal.o \

OBJS_QUOTED += \
"./SDK/platform/pal/src/wdg/wdg_pal.o" \

C_DEPS += \
./SDK/platform/pal/src/wdg/wdg_pal.d \


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/pal/src/wdg/wdg_pal.o: ../SDK/platform/pal/src/wdg/wdg_pal.c
	@echo 'Building file: $<'
	@echo 'Executing target #79 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/platform/pal/src/wdg/wdg_pal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "SDK/platform/pal/src/wdg/wdg_pal.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


