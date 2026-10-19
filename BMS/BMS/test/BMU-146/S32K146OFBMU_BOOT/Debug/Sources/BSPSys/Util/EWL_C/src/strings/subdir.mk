################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memchr.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memcmp.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memcpy.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memmove.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/memset.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcat.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strchr.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcmp.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcoll.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcpy.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strcspn.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strerror.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strlen.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strncat.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strncmp.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strncpy.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strrchr.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strspn.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strstr.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strtok.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.c \
../Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.c \
../Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.c \
../Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.c \
../Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memchr.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memcmp.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memcpy.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memmove.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/memset.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcat.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strchr.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcmp.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcoll.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcpy.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strcspn.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strerror.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strlen.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strncat.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strncmp.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strncpy.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strrchr.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strspn.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strstr.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strtok.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.c \
../Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memcmp.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memcpy.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memmove.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memset.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcat.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcmp.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcoll.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcpy.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcspn.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strerror.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strlen.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncat.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncmp.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncpy.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strrchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strspn.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strstr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strtok.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memchr.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memcmp.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memcpy.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memmove.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memset.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcat.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strchr.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcmp.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcoll.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcpy.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcspn.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strerror.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strlen.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncat.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncmp.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncpy.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strrchr.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strspn.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strstr.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strtok.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memcmp.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memcpy.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memmove.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/memset.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcat.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcmp.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcoll.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcpy.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strcspn.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strerror.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strlen.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncat.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncmp.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncpy.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strrchr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strspn.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strstr.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strtok.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.o \
./Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memchr.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memcmp.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memcpy.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memmove.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/memset.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcat.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strchr.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcmp.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcoll.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcpy.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strcspn.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strerror.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strlen.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncat.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncmp.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncpy.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strrchr.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strspn.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strstr.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strtok.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.d \
./Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.d \
./Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.d \
./Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.d \
./Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memchr.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memcmp.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memcpy.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memmove.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/memset.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcat.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strchr.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcmp.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcoll.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcpy.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strcspn.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strerror.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strlen.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strncat.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strncmp.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strncpy.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strrchr.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strspn.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strstr.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strtok.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.d \
./Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.o: ../Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.c
	@echo 'Building file: $<'
	@echo 'Executing target #516 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/__copy_long_mem.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.o: ../Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.c
	@echo 'Building file: $<'
	@echo 'Executing target #517 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/__copy_mem.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.o: ../Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.c
	@echo 'Building file: $<'
	@echo 'Executing target #518 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/__filll_mem.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.o: ../Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.c
	@echo 'Building file: $<'
	@echo 'Executing target #519 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/__memrchr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.o: ../Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.c
	@echo 'Building file: $<'
	@echo 'Executing target #520 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/__move_mem.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memchr.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memchr.c
	@echo 'Building file: $<'
	@echo 'Executing target #521 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memchr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memchr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memcmp.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memcmp.c
	@echo 'Building file: $<'
	@echo 'Executing target #522 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memcmp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memcmp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memcpy.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memcpy.c
	@echo 'Building file: $<'
	@echo 'Executing target #523 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memcpy.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memcpy.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #524 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memcpy_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memmove.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memmove.c
	@echo 'Building file: $<'
	@echo 'Executing target #525 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memmove.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memmove.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #526 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memmove_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/memset.o: ../Sources/BSPSys/Util/EWL_C/src/strings/memset.c
	@echo 'Building file: $<'
	@echo 'Executing target #527 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/memset.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/memset.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcat.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcat.c
	@echo 'Building file: $<'
	@echo 'Executing target #528 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcat.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcat.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #529 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcat_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strchr.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strchr.c
	@echo 'Building file: $<'
	@echo 'Executing target #530 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strchr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strchr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcmp.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcmp.c
	@echo 'Building file: $<'
	@echo 'Executing target #531 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcmp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcmp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcoll.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcoll.c
	@echo 'Building file: $<'
	@echo 'Executing target #532 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcoll.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcoll.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcpy.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcpy.c
	@echo 'Building file: $<'
	@echo 'Executing target #533 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcpy.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcpy.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #534 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcpy_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strcspn.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strcspn.c
	@echo 'Building file: $<'
	@echo 'Executing target #535 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strcspn.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strcspn.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strerror.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strerror.c
	@echo 'Building file: $<'
	@echo 'Executing target #536 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strerror.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strerror.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strlen.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strlen.c
	@echo 'Building file: $<'
	@echo 'Executing target #537 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strlen.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strlen.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strncat.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strncat.c
	@echo 'Building file: $<'
	@echo 'Executing target #538 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strncat.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strncat.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #539 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strncat_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strncmp.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strncmp.c
	@echo 'Building file: $<'
	@echo 'Executing target #540 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strncmp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strncmp.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strncpy.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strncpy.c
	@echo 'Building file: $<'
	@echo 'Executing target #541 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strncpy.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strncpy.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #542 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strncpy_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #543 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strnlen_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.c
	@echo 'Building file: $<'
	@echo 'Executing target #544 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strpbrk.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strrchr.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strrchr.c
	@echo 'Building file: $<'
	@echo 'Executing target #545 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strrchr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strrchr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strspn.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strspn.c
	@echo 'Building file: $<'
	@echo 'Executing target #546 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strspn.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strspn.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strstr.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strstr.c
	@echo 'Building file: $<'
	@echo 'Executing target #547 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strstr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strstr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strtok.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strtok.c
	@echo 'Building file: $<'
	@echo 'Executing target #548 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strtok.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strtok.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #549 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strtok_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.o: ../Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.c
	@echo 'Building file: $<'
	@echo 'Executing target #550 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/strings/strxfrm.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


