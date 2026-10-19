################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Funtion/Parameter/GroupPara.c" \
"../Sources/Funtion/Parameter/ParaCopy.c" \
"../Sources/Funtion/Parameter/ParaDnload.c" \
"../Sources/Funtion/Parameter/ParaExtEep.c" \
"../Sources/Funtion/Parameter/ParaIntEep.c" \
"../Sources/Funtion/Parameter/ParaIntFlash.c" \
"../Sources/Funtion/Parameter/ParaInterface.c" \
"../Sources/Funtion/Parameter/ParaRead.c" \
"../Sources/Funtion/Parameter/ParaUser.c" \
"../Sources/Funtion/Parameter/ParaWrite.c" \
"../Sources/Funtion/Parameter/PublicPara.c" \

C_SRCS += \
../Sources/Funtion/Parameter/GroupPara.c \
../Sources/Funtion/Parameter/ParaCopy.c \
../Sources/Funtion/Parameter/ParaDnload.c \
../Sources/Funtion/Parameter/ParaExtEep.c \
../Sources/Funtion/Parameter/ParaIntEep.c \
../Sources/Funtion/Parameter/ParaIntFlash.c \
../Sources/Funtion/Parameter/ParaInterface.c \
../Sources/Funtion/Parameter/ParaRead.c \
../Sources/Funtion/Parameter/ParaUser.c \
../Sources/Funtion/Parameter/ParaWrite.c \
../Sources/Funtion/Parameter/PublicPara.c \

OBJS_OS_FORMAT += \
./Sources/Funtion/Parameter/GroupPara.o \
./Sources/Funtion/Parameter/ParaCopy.o \
./Sources/Funtion/Parameter/ParaDnload.o \
./Sources/Funtion/Parameter/ParaExtEep.o \
./Sources/Funtion/Parameter/ParaIntEep.o \
./Sources/Funtion/Parameter/ParaIntFlash.o \
./Sources/Funtion/Parameter/ParaInterface.o \
./Sources/Funtion/Parameter/ParaRead.o \
./Sources/Funtion/Parameter/ParaUser.o \
./Sources/Funtion/Parameter/ParaWrite.o \
./Sources/Funtion/Parameter/PublicPara.o \

C_DEPS_QUOTED += \
"./Sources/Funtion/Parameter/GroupPara.d" \
"./Sources/Funtion/Parameter/ParaCopy.d" \
"./Sources/Funtion/Parameter/ParaDnload.d" \
"./Sources/Funtion/Parameter/ParaExtEep.d" \
"./Sources/Funtion/Parameter/ParaIntEep.d" \
"./Sources/Funtion/Parameter/ParaIntFlash.d" \
"./Sources/Funtion/Parameter/ParaInterface.d" \
"./Sources/Funtion/Parameter/ParaRead.d" \
"./Sources/Funtion/Parameter/ParaUser.d" \
"./Sources/Funtion/Parameter/ParaWrite.d" \
"./Sources/Funtion/Parameter/PublicPara.d" \

OBJS += \
./Sources/Funtion/Parameter/GroupPara.o \
./Sources/Funtion/Parameter/ParaCopy.o \
./Sources/Funtion/Parameter/ParaDnload.o \
./Sources/Funtion/Parameter/ParaExtEep.o \
./Sources/Funtion/Parameter/ParaIntEep.o \
./Sources/Funtion/Parameter/ParaIntFlash.o \
./Sources/Funtion/Parameter/ParaInterface.o \
./Sources/Funtion/Parameter/ParaRead.o \
./Sources/Funtion/Parameter/ParaUser.o \
./Sources/Funtion/Parameter/ParaWrite.o \
./Sources/Funtion/Parameter/PublicPara.o \

OBJS_QUOTED += \
"./Sources/Funtion/Parameter/GroupPara.o" \
"./Sources/Funtion/Parameter/ParaCopy.o" \
"./Sources/Funtion/Parameter/ParaDnload.o" \
"./Sources/Funtion/Parameter/ParaExtEep.o" \
"./Sources/Funtion/Parameter/ParaIntEep.o" \
"./Sources/Funtion/Parameter/ParaIntFlash.o" \
"./Sources/Funtion/Parameter/ParaInterface.o" \
"./Sources/Funtion/Parameter/ParaRead.o" \
"./Sources/Funtion/Parameter/ParaUser.o" \
"./Sources/Funtion/Parameter/ParaWrite.o" \
"./Sources/Funtion/Parameter/PublicPara.o" \

C_DEPS += \
./Sources/Funtion/Parameter/GroupPara.d \
./Sources/Funtion/Parameter/ParaCopy.d \
./Sources/Funtion/Parameter/ParaDnload.d \
./Sources/Funtion/Parameter/ParaExtEep.d \
./Sources/Funtion/Parameter/ParaIntEep.d \
./Sources/Funtion/Parameter/ParaIntFlash.d \
./Sources/Funtion/Parameter/ParaInterface.d \
./Sources/Funtion/Parameter/ParaRead.d \
./Sources/Funtion/Parameter/ParaUser.d \
./Sources/Funtion/Parameter/ParaWrite.d \
./Sources/Funtion/Parameter/PublicPara.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/Funtion/Parameter/GroupPara.o: ../Sources/Funtion/Parameter/GroupPara.c
	@echo 'Building file: $<'
	@echo 'Executing target #176 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/GroupPara.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/GroupPara.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaCopy.o: ../Sources/Funtion/Parameter/ParaCopy.c
	@echo 'Building file: $<'
	@echo 'Executing target #177 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaCopy.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaCopy.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaDnload.o: ../Sources/Funtion/Parameter/ParaDnload.c
	@echo 'Building file: $<'
	@echo 'Executing target #178 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaDnload.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaDnload.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaExtEep.o: ../Sources/Funtion/Parameter/ParaExtEep.c
	@echo 'Building file: $<'
	@echo 'Executing target #179 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaExtEep.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaExtEep.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaIntEep.o: ../Sources/Funtion/Parameter/ParaIntEep.c
	@echo 'Building file: $<'
	@echo 'Executing target #180 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaIntEep.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaIntEep.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaIntFlash.o: ../Sources/Funtion/Parameter/ParaIntFlash.c
	@echo 'Building file: $<'
	@echo 'Executing target #181 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaIntFlash.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaIntFlash.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaInterface.o: ../Sources/Funtion/Parameter/ParaInterface.c
	@echo 'Building file: $<'
	@echo 'Executing target #182 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaInterface.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaInterface.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaRead.o: ../Sources/Funtion/Parameter/ParaRead.c
	@echo 'Building file: $<'
	@echo 'Executing target #183 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaRead.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaRead.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaUser.o: ../Sources/Funtion/Parameter/ParaUser.c
	@echo 'Building file: $<'
	@echo 'Executing target #184 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaUser.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaUser.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/ParaWrite.o: ../Sources/Funtion/Parameter/ParaWrite.c
	@echo 'Building file: $<'
	@echo 'Executing target #185 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/ParaWrite.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/ParaWrite.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Funtion/Parameter/PublicPara.o: ../Sources/Funtion/Parameter/PublicPara.c
	@echo 'Building file: $<'
	@echo 'Executing target #186 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/Funtion/Parameter/PublicPara.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/Funtion/Parameter/PublicPara.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


