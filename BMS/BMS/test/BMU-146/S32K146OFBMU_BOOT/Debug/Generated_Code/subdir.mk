################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Generated_Code/Cpu.c" \
"../Generated_Code/canCom1.c" \
"../Generated_Code/canCom2.c" \
"../Generated_Code/canCom3.c" \
"../Generated_Code/clockMan1.c" \
"../Generated_Code/dmaController1.c" \
"../Generated_Code/flash_config.c" \
"../Generated_Code/flexTimer_mc1.c" \
"../Generated_Code/flexio_spi1.c" \
"../Generated_Code/lpit1.c" \
"../Generated_Code/lpspiCom1.c" \
"../Generated_Code/pin_mux.c" \
"../Generated_Code/watchdog1.c" \

C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/canCom1.c \
../Generated_Code/canCom2.c \
../Generated_Code/canCom3.c \
../Generated_Code/clockMan1.c \
../Generated_Code/dmaController1.c \
../Generated_Code/flash_config.c \
../Generated_Code/flexTimer_mc1.c \
../Generated_Code/flexio_spi1.c \
../Generated_Code/lpit1.c \
../Generated_Code/lpspiCom1.c \
../Generated_Code/pin_mux.c \
../Generated_Code/watchdog1.c \

C_DEPS_QUOTED += \
"./Generated_Code/Cpu.d" \
"./Generated_Code/canCom1.d" \
"./Generated_Code/canCom2.d" \
"./Generated_Code/canCom3.d" \
"./Generated_Code/clockMan1.d" \
"./Generated_Code/dmaController1.d" \
"./Generated_Code/flash_config.d" \
"./Generated_Code/flexTimer_mc1.d" \
"./Generated_Code/flexio_spi1.d" \
"./Generated_Code/lpit1.d" \
"./Generated_Code/lpspiCom1.d" \
"./Generated_Code/pin_mux.d" \
"./Generated_Code/watchdog1.d" \

OBJS_QUOTED += \
"./Generated_Code/Cpu.o" \
"./Generated_Code/canCom1.o" \
"./Generated_Code/canCom2.o" \
"./Generated_Code/canCom3.o" \
"./Generated_Code/clockMan1.o" \
"./Generated_Code/dmaController1.o" \
"./Generated_Code/flash_config.o" \
"./Generated_Code/flexTimer_mc1.o" \
"./Generated_Code/flexio_spi1.o" \
"./Generated_Code/lpit1.o" \
"./Generated_Code/lpspiCom1.o" \
"./Generated_Code/pin_mux.o" \
"./Generated_Code/watchdog1.o" \

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/canCom1.d \
./Generated_Code/canCom2.d \
./Generated_Code/canCom3.d \
./Generated_Code/clockMan1.d \
./Generated_Code/dmaController1.d \
./Generated_Code/flash_config.d \
./Generated_Code/flexTimer_mc1.d \
./Generated_Code/flexio_spi1.d \
./Generated_Code/lpit1.d \
./Generated_Code/lpspiCom1.d \
./Generated_Code/pin_mux.d \
./Generated_Code/watchdog1.d \

OBJS_OS_FORMAT += \
./Generated_Code/Cpu.o \
./Generated_Code/canCom1.o \
./Generated_Code/canCom2.o \
./Generated_Code/canCom3.o \
./Generated_Code/clockMan1.o \
./Generated_Code/dmaController1.o \
./Generated_Code/flash_config.o \
./Generated_Code/flexTimer_mc1.o \
./Generated_Code/flexio_spi1.o \
./Generated_Code/lpit1.o \
./Generated_Code/lpspiCom1.o \
./Generated_Code/pin_mux.o \
./Generated_Code/watchdog1.o \

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/canCom1.o \
./Generated_Code/canCom2.o \
./Generated_Code/canCom3.o \
./Generated_Code/clockMan1.o \
./Generated_Code/dmaController1.o \
./Generated_Code/flash_config.o \
./Generated_Code/flexTimer_mc1.o \
./Generated_Code/flexio_spi1.o \
./Generated_Code/lpit1.o \
./Generated_Code/lpspiCom1.o \
./Generated_Code/pin_mux.o \
./Generated_Code/watchdog1.o \


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/Cpu.o: ../Generated_Code/Cpu.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/Cpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/Cpu.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/canCom1.o: ../Generated_Code/canCom1.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/canCom1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/canCom1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/canCom2.o: ../Generated_Code/canCom2.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/canCom2.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/canCom2.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/canCom3.o: ../Generated_Code/canCom3.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/canCom3.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/canCom3.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/clockMan1.o: ../Generated_Code/clockMan1.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/clockMan1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/clockMan1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/dmaController1.o: ../Generated_Code/dmaController1.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/dmaController1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/dmaController1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/flash_config.o: ../Generated_Code/flash_config.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/flash_config.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/flash_config.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/flexTimer_mc1.o: ../Generated_Code/flexTimer_mc1.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/flexTimer_mc1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/flexTimer_mc1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/flexio_spi1.o: ../Generated_Code/flexio_spi1.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/flexio_spi1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/flexio_spi1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/lpit1.o: ../Generated_Code/lpit1.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/lpit1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/lpit1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/lpspiCom1.o: ../Generated_Code/lpspiCom1.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/lpspiCom1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/lpspiCom1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/pin_mux.o: ../Generated_Code/pin_mux.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/pin_mux.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/pin_mux.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/watchdog1.o: ../Generated_Code/watchdog1.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/watchdog1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Generated_Code/watchdog1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


