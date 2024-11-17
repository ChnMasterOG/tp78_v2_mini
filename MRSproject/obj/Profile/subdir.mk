################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Profile/battservice.c \
../Profile/devinfoservice.c \
../Profile/hiddev.c \
../Profile/hidkbmservice.c \
../Profile/scanparamservice.c 

OBJS += \
./Profile/battservice.o \
./Profile/devinfoservice.o \
./Profile/hiddev.o \
./Profile/hidkbmservice.o \
./Profile/scanparamservice.o 

C_DEPS += \
./Profile/battservice.d \
./Profile/devinfoservice.d \
./Profile/hiddev.d \
./Profile/hidkbmservice.d \
./Profile/scanparamservice.d 


# Each subdirectory must supply rules for building sources it contributes
Profile/%.o: ../Profile/%.c
	@	@	riscv-none-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=1 -DDEBUG=3 -DLOW_MEM=1 -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs\port" -I"E:\Project\tp78_v2_mini\MRSproject\StdPeriphDriver\inc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\common" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\core" -I"E:\Project\tp78_v2_mini\MRSproject\Startup" -I"E:\Project\tp78_v2_mini\MRSproject\HAL\include" -I"E:\Project\tp78_v2_mini\MRSproject\APP\include" -I"E:\Project\tp78_v2_mini\MRSproject\Profile\include" -I"E:\Project\tp78_v2_mini\MRSproject\Ld" -I"E:\Project\tp78_v2_mini\MRSproject\LIB" -I"E:\Project\tp78_v2_mini\MRSproject\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

