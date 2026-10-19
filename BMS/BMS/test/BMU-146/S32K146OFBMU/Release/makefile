################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 217
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Generated_Code/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include SDK/platform/devices/S32K146/startup/subdir.mk
-include SDK/platform/devices/subdir.mk
-include SDK/platform/drivers/src/adc/subdir.mk
-include SDK/platform/drivers/src/clock/S32K1xx/subdir.mk
-include SDK/platform/drivers/src/edma/subdir.mk
-include SDK/platform/drivers/src/flash/subdir.mk
-include SDK/platform/drivers/src/flexcan/subdir.mk
-include SDK/platform/drivers/src/flexio/subdir.mk
-include SDK/platform/drivers/src/ftm/subdir.mk
-include SDK/platform/drivers/src/interrupt/subdir.mk
-include SDK/platform/drivers/src/lpi2c/subdir.mk
-include SDK/platform/drivers/src/lpit/subdir.mk
-include SDK/platform/drivers/src/lpspi/subdir.mk
-include SDK/platform/drivers/src/lptmr/subdir.mk
-include SDK/platform/drivers/src/lpuart/subdir.mk
-include SDK/platform/drivers/src/pdb/subdir.mk
-include SDK/platform/drivers/src/pins/subdir.mk
-include SDK/platform/drivers/src/rtc/subdir.mk
-include SDK/platform/drivers/src/trgmux/subdir.mk
-include SDK/platform/drivers/src/wdog/subdir.mk
-include SDK/platform/pal/src/adc/subdir.mk
-include SDK/platform/pal/src/can/subdir.mk
-include SDK/platform/pal/src/i2c/subdir.mk
-include SDK/platform/pal/src/pwm/subdir.mk
-include SDK/platform/pal/src/spi/subdir.mk
-include SDK/platform/pal/src/uart/subdir.mk
-include SDK/platform/pal/src/wdg/subdir.mk
-include SDK/rtos/FreeRTOS_S32K/Source/subdir.mk
-include SDK/rtos/FreeRTOS_S32K/Source/portable/GCC/ARM_CM4F/subdir.mk
-include SDK/rtos/FreeRTOS_S32K/Source/portable/MemMang/subdir.mk
-include SDK/rtos/osif/subdir.mk
-include Sources/Addition/Calibration/subdir.mk
-include Sources/Addition/LoadApp/subdir.mk
-include Sources/Addition/OperAuth/subdir.mk
-include Sources/Addition/UserDef/subdir.mk
-include Sources/Addition/UserProto/subdir.mk
-include Sources/BSPSys/BSPCfg/subdir.mk
-include Sources/BSPSys/BSPUser/subdir.mk
-include Sources/BSPSys/Device/subdir.mk
-include Sources/BSPSys/Device/FS65_driver/subdir.mk
-include Sources/BSPSys/Device/LTC2949_driver/subdir.mk
-include Sources/BSPSys/Device/LTC6813_driver/subdir.mk
-include Sources/BSPSys/Device/M24512_driver/subdir.mk
-include Sources/BSPSys/Device/MCP3913_driver/subdir.mk
-include Sources/BSPSys/Device/RX8010_driver/subdir.mk
-include Sources/BSPSys/Device/SST26VF064_driver/subdir.mk
-include Sources/BSPSys/Driver/subdir.mk
-include Sources/BSPSys/Util/subdir.mk
-include Sources/Funtion/CanLib/subdir.mk
-include Sources/Funtion/ChgCan/subdir.mk
-include Sources/Funtion/DataCalc/subdir.mk
-include Sources/Funtion/Diagnosis/subdir.mk
-include Sources/Funtion/EnergyCalc/subdir.mk
-include Sources/Funtion/Measurement/subdir.mk
-include Sources/Funtion/ModbusLib/subdir.mk
-include Sources/Funtion/Parameter/subdir.mk
-include Sources/Funtion/PowerManag/subdir.mk
-include Sources/Funtion/Recording/subdir.mk
-include Sources/Funtion/Statistics/subdir.mk
-include Sources/Funtion/SystemInfo/subdir.mk
-include Sources/Logics/Balance/subdir.mk
-include Sources/Logics/CanUser/subdir.mk
-include Sources/Logics/EventUser/subdir.mk
-include Sources/Logics/FstChg/subdir.mk
-include Sources/Logics/SciUser/subdir.mk
-include Sources/Logics/Tactics/subdir.mk
-include Sources/OSUser/subdir.mk
-include Sources/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
S32K146OFBMU.elf \

EXECUTABLES_QUOTED += \
"S32K146OFBMU.elf" \

EXECUTABLES_OS_FORMAT += \
S32K146OFBMU.elf \

SECONDARY_SIZE += \
S32K146OFBMU.siz \

SECONDARY_SIZE_QUOTED += \
"S32K146OFBMU.siz" \

SECONDARY_SIZE_OS_FORMAT += \
S32K146OFBMU.siz \


# All Target
all: warning-messages explicit-dependencies S32K146OFBMU.elf secondary-outputs

# Tool invocations
S32K146OFBMU.elf: $(OBJS) E:/SVN-SBK(NEW)/code/S32K146OFBMU/Project_Settings/Linker_Files/S32K146_128_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #218 $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "S32K146OFBMU.elf" "@S32K146OFBMU.args"  $(USER_OBJS_QUOTED) $(LIBS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

S32K146OFBMU.siz: $(EXECUTABLES)
	@echo 'Executing target #219 $@'
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*/*.d ./*/*/*.d ./*/*/*/*.d ./*/*/*/*/*.d ./*/*/*/*/*/*/*.d ./*/*/*/*/*/*/*/*.d ./*/*/*/*/*/*.d  ./*/*.o ./*/*/*.o ./*/*/*/*.o ./*/*/*/*/*.o ./*/*/*/*/*/*/*.o ./*/*/*/*/*/*/*/*.o ./*/*/*/*/*/*.o  $(SECONDARY_SIZE_QUOTED) $(EXECUTABLES_QUOTED)
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: 

warning-messages: 

