################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 69
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Generated_Code/subdir.mk
-include Project_Settings/Linker_Files/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include SDK/platform/devices/S32K146/startup/subdir.mk
-include SDK/platform/devices/subdir.mk
-include SDK/platform/drivers/src/clock/S32K1xx/subdir.mk
-include SDK/platform/drivers/src/edma/subdir.mk
-include SDK/platform/drivers/src/flash/subdir.mk
-include SDK/platform/drivers/src/flexcan/subdir.mk
-include SDK/platform/drivers/src/flexio/subdir.mk
-include SDK/platform/drivers/src/ftm/subdir.mk
-include SDK/platform/drivers/src/interrupt/subdir.mk
-include SDK/platform/drivers/src/lpit/subdir.mk
-include SDK/platform/drivers/src/lpspi/subdir.mk
-include SDK/platform/drivers/src/pins/subdir.mk
-include SDK/platform/drivers/src/wdog/subdir.mk
-include SDK/rtos/osif/subdir.mk
-include Sources/BSPSys/BSPCfg/subdir.mk
-include Sources/BSPSys/BSPUser/subdir.mk
-include Sources/BSPSys/Device/subdir.mk
-include Sources/BSPSys/Device/FS65_driver/subdir.mk
-include Sources/BSPSys/Device/SST26VF064_driver/subdir.mk
-include Sources/BSPSys/Driver/subdir.mk
-include Sources/BSPSys/Util/subdir.mk
-include Sources/LoadApp/subdir.mk
-include Sources/OSUser/subdir.mk
-include Sources/PowerManag/subdir.mk
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

SECONDARY_FLASH += \
S32K146OFBMU.srec \

SECONDARY_FLASH_QUOTED += \
"S32K146OFBMU.srec" \

SECONDARY_FLASH_OS_FORMAT += \
S32K146OFBMU.srec \

SECONDARY_SIZE += \
S32K146OFBMU.siz \

SECONDARY_SIZE_QUOTED += \
"S32K146OFBMU.siz" \

SECONDARY_SIZE_OS_FORMAT += \
S32K146OFBMU.siz \


# All Target
all: warning-messages explicit-dependencies S32K146OFBMU.elf secondary-outputs

# Tool invocations
S32K146OFBMU.elf: $(OBJS) E:/SBK/SVN/code/S32K146OFBMU_BOOT/Project_Settings/Linker_Files/S32K146_128_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #70 $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "S32K146OFBMU.elf" "@S32K146OFBMU.args"  $(USER_OBJS_QUOTED) $(LIBS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

S32K146OFBMU.srec: $(EXECUTABLES)
	@echo 'Executing target #71 $@'
	@echo 'Invoking: Standard S32DS Create Flash Image'
	arm-none-eabi-objcopy -O srec $(EXECUTABLES)  "S32K146OFBMU.srec"
	@echo 'Finished building: $@'
	@echo ' '

S32K146OFBMU.siz: $(EXECUTABLES)
	@echo 'Executing target #72 $@'
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley -x $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_FLASH_QUOTED) ./*/*.d ./*/*/*.d ./*/*/*/*.d ./*/*/*/*/*.d ./*/*/*/*/*/*.d ./*/*/*/*/*/*/*.d  ./*/*.o ./*/*/*.o ./*/*/*/*.o ./*/*/*/*/*.o ./*/*/*/*/*/*.o ./*/*/*/*/*/*/*.o  $(SECONDARY_SIZE_QUOTED) $(EXECUTABLES_QUOTED)
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: 

warning-messages: 

