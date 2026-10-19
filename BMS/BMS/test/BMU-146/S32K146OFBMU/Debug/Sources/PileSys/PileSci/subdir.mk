################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/PileSys/PileSci/PileSciCtrlHd.c" \
"../Sources/PileSys/PileSci/PileSciInfoPt.c" \
"../Sources/PileSys/PileSci/PileSciParaHd.c" \

C_SRCS += \
../Sources/PileSys/PileSci/PileSciCtrlHd.c \
../Sources/PileSys/PileSci/PileSciInfoPt.c \
../Sources/PileSys/PileSci/PileSciParaHd.c \

C_DEPS_QUOTED += \
"./Sources/PileSys/PileSci/PileSciCtrlHd.d" \
"./Sources/PileSys/PileSci/PileSciInfoPt.d" \
"./Sources/PileSys/PileSci/PileSciParaHd.d" \

OBJS_QUOTED += \
"./Sources/PileSys/PileSci/PileSciCtrlHd.o" \
"./Sources/PileSys/PileSci/PileSciInfoPt.o" \
"./Sources/PileSys/PileSci/PileSciParaHd.o" \

C_DEPS += \
./Sources/PileSys/PileSci/PileSciCtrlHd.d \
./Sources/PileSys/PileSci/PileSciInfoPt.d \
./Sources/PileSys/PileSci/PileSciParaHd.d \

OBJS_OS_FORMAT += \
./Sources/PileSys/PileSci/PileSciCtrlHd.o \
./Sources/PileSys/PileSci/PileSciInfoPt.o \
./Sources/PileSys/PileSci/PileSciParaHd.o \

OBJS += \
./Sources/PileSys/PileSci/PileSciCtrlHd.o \
./Sources/PileSys/PileSci/PileSciInfoPt.o \
./Sources/PileSys/PileSci/PileSciParaHd.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/PileSys/PileSci/PileSciCtrlHd.o: ../Sources/PileSys/PileSci/PileSciCtrlHd.c
	@echo 'Building file: $<'
	@echo 'Executing target #242 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileSci/PileSciCtrlHd.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileSci/PileSciCtrlHd.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileSci/PileSciInfoPt.o: ../Sources/PileSys/PileSci/PileSciInfoPt.c
	@echo 'Building file: $<'
	@echo 'Executing target #243 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileSci/PileSciInfoPt.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileSci/PileSciInfoPt.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PileSys/PileSci/PileSciParaHd.o: ../Sources/PileSys/PileSci/PileSciParaHd.c
	@echo 'Building file: $<'
	@echo 'Executing target #244 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/PileSys/PileSci/PileSciParaHd.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/PileSys/PileSci/PileSciParaHd.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


