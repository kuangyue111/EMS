################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../SDK/platform/pal/src/spi/spi_pal.c" \

C_SRCS += \
../SDK/platform/pal/src/spi/spi_pal.c \

C_DEPS_QUOTED += \
"./SDK/platform/pal/src/spi/spi_pal.d" \

OBJS_QUOTED += \
"./SDK/platform/pal/src/spi/spi_pal.o" \

C_DEPS += \
./SDK/platform/pal/src/spi/spi_pal.d \

OBJS_OS_FORMAT += \
./SDK/platform/pal/src/spi/spi_pal.o \

OBJS += \
./SDK/platform/pal/src/spi/spi_pal.o \


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/pal/src/spi/spi_pal.o: ../SDK/platform/pal/src/spi/spi_pal.c
	@echo 'Building file: $<'
	@echo 'Executing target #77 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/platform/pal/src/spi/spi_pal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "SDK/platform/pal/src/spi/spi_pal.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


