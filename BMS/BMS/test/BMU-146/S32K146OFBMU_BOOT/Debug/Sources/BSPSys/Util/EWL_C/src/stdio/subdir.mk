################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/conread.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fclose.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/feof.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/ferror.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fflush.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fgets.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fopen.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fputc.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fputs.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fread.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fseek.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/ftell.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/getc.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/getchar.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/gets.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/perror.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/printf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/putc.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/putchar.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/puts.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/remove.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/rename.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/rewind.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/scanf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.c" \
"../Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.c" \

C_SRCS += \
../Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/conread.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fclose.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/feof.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/ferror.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fflush.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fgets.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fopen.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fputc.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fputs.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fread.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fseek.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/ftell.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/getc.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/getchar.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/gets.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/perror.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/printf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/putc.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/putchar.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/puts.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/remove.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/rename.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/rewind.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/scanf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.c \
../Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.c \

OBJS_OS_FORMAT += \
./Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/conread.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fclose.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/feof.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ferror.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fflush.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgets.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fopen.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fputc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fputs.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fread.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fseek.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ftell.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/getc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/getchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/gets.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/perror.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/printf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/putc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/putchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/puts.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/remove.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/rename.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/rewind.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/scanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.o \

C_DEPS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/conread.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fclose.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/feof.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ferror.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fflush.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fgets.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fopen.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fputc.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fputs.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fread.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fseek.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ftell.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/getc.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/getchar.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/gets.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/perror.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/printf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/putc.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/putchar.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/puts.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/remove.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/rename.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/rewind.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/scanf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.d" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.d" \

OBJS += \
./Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/conread.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fclose.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/feof.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ferror.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fflush.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgets.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fopen.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fputc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fputs.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fread.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fseek.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ftell.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/getc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/getchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/gets.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/perror.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/printf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/putc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/putchar.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/puts.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/remove.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/rename.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/rewind.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/scanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.o \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.o \

OBJS_QUOTED += \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/conread.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fclose.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/feof.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ferror.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fflush.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fgets.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fopen.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fputc.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fputs.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fread.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fseek.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ftell.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/getc.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/getchar.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/gets.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/perror.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/printf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/putc.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/putchar.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/puts.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/remove.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/rename.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/rewind.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/scanf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.o" \
"./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.o" \

C_DEPS += \
./Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/conread.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fclose.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/feof.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/ferror.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fflush.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgets.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fopen.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fputc.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fputs.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fread.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fseek.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/ftell.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/getc.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/getchar.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/gets.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/perror.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/printf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/putc.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/putchar.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/puts.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/remove.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/rename.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/rewind.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/scanf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.d \
./Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.c
	@echo 'Building file: $<'
	@echo 'Executing target #410 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__buffer_io.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.c
	@echo 'Building file: $<'
	@echo 'Executing target #411 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__getchar.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.c
	@echo 'Building file: $<'
	@echo 'Executing target #412 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__printformat.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.c
	@echo 'Building file: $<'
	@echo 'Executing target #413 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__putchar.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.c
	@echo 'Building file: $<'
	@echo 'Executing target #414 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__scanformat.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.c
	@echo 'Building file: $<'
	@echo 'Executing target #415 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__stdio_atexit.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.c
	@echo 'Building file: $<'
	@echo 'Executing target #416 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/__strtold.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.c
	@echo 'Building file: $<'
	@echo 'Executing target #417 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/ansi_files.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.c
	@echo 'Building file: $<'
	@echo 'Executing target #418 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/clearerr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/conread.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/conread.c
	@echo 'Building file: $<'
	@echo 'Executing target #419 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/conread.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/conread.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.c
	@echo 'Building file: $<'
	@echo 'Executing target #420 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/conwrite.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fclose.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fclose.c
	@echo 'Building file: $<'
	@echo 'Executing target #421 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fclose.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fclose.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/feof.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/feof.c
	@echo 'Building file: $<'
	@echo 'Executing target #422 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/feof.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/feof.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/ferror.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/ferror.c
	@echo 'Building file: $<'
	@echo 'Executing target #423 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/ferror.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/ferror.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fflush.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fflush.c
	@echo 'Building file: $<'
	@echo 'Executing target #424 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fflush.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fflush.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.c
	@echo 'Building file: $<'
	@echo 'Executing target #425 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fgetc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fgets.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fgets.c
	@echo 'Building file: $<'
	@echo 'Executing target #426 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fgets.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fgets.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #427 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fgets_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fopen.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fopen.c
	@echo 'Building file: $<'
	@echo 'Executing target #428 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fopen.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fopen.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #429 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fopen_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #430 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #431 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fputc.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fputc.c
	@echo 'Building file: $<'
	@echo 'Executing target #432 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fputc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fputc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fputs.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fputs.c
	@echo 'Building file: $<'
	@echo 'Executing target #433 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fputs.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fputs.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fread.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fread.c
	@echo 'Building file: $<'
	@echo 'Executing target #434 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fread.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fread.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.c
	@echo 'Building file: $<'
	@echo 'Executing target #435 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fscanf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #436 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fscanf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fseek.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fseek.c
	@echo 'Building file: $<'
	@echo 'Executing target #437 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fseek.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fseek.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/ftell.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/ftell.c
	@echo 'Building file: $<'
	@echo 'Executing target #438 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/ftell.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/ftell.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.c
	@echo 'Building file: $<'
	@echo 'Executing target #439 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/fwrite.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/getc.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/getc.c
	@echo 'Building file: $<'
	@echo 'Executing target #440 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/getc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/getc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/getchar.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/getchar.c
	@echo 'Building file: $<'
	@echo 'Executing target #441 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/getchar.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/getchar.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/gets.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/gets.c
	@echo 'Building file: $<'
	@echo 'Executing target #442 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/gets.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/gets.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/perror.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/perror.c
	@echo 'Building file: $<'
	@echo 'Executing target #443 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/perror.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/perror.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/printf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/printf.c
	@echo 'Building file: $<'
	@echo 'Executing target #444 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/printf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/printf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #445 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/printf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/putc.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/putc.c
	@echo 'Building file: $<'
	@echo 'Executing target #446 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/putc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/putc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/putchar.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/putchar.c
	@echo 'Building file: $<'
	@echo 'Executing target #447 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/putchar.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/putchar.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/puts.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/puts.c
	@echo 'Building file: $<'
	@echo 'Executing target #448 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/puts.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/puts.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/remove.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/remove.c
	@echo 'Building file: $<'
	@echo 'Executing target #449 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/remove.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/remove.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/rename.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/rename.c
	@echo 'Building file: $<'
	@echo 'Executing target #450 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/rename.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/rename.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/rewind.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/rewind.c
	@echo 'Building file: $<'
	@echo 'Executing target #451 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/rewind.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/rewind.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/scanf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/scanf.c
	@echo 'Building file: $<'
	@echo 'Executing target #452 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/scanf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/scanf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #453 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/scanf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.c
	@echo 'Building file: $<'
	@echo 'Executing target #454 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/setvbuf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #455 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/snprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #456 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/snprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #457 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/sprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #458 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/sprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.c
	@echo 'Building file: $<'
	@echo 'Executing target #459 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/sscanf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #460 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/sscanf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.c
	@echo 'Building file: $<'
	@echo 'Executing target #461 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #462 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/tmpfile_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.c
	@echo 'Building file: $<'
	@echo 'Executing target #463 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #464 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/tmpnam_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.c
	@echo 'Building file: $<'
	@echo 'Executing target #465 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/ungetc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #466 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #467 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vfprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.c
	@echo 'Building file: $<'
	@echo 'Executing target #468 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #469 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vfscanf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #470 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #471 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.c
	@echo 'Building file: $<'
	@echo 'Executing target #472 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vscanf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #473 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vscanf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #474 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #475 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vsnprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.c
	@echo 'Building file: $<'
	@echo 'Executing target #476 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #477 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vsprintf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.c
	@echo 'Building file: $<'
	@echo 'Executing target #478 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.o: ../Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.c
	@echo 'Building file: $<'
	@echo 'Executing target #479 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Sources/BSPSys/Util/EWL_C/src/stdio/vsscanf_s.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


