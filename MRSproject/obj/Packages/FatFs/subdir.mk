################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/FatFs/diskio.c \
../Packages/FatFs/ff.c \
../Packages/FatFs/fftools.c \
../Packages/FatFs/ffunicode.c 

OBJS += \
./Packages/FatFs/diskio.o \
./Packages/FatFs/ff.o \
./Packages/FatFs/fftools.o \
./Packages/FatFs/ffunicode.o 

C_DEPS += \
./Packages/FatFs/diskio.d \
./Packages/FatFs/ff.d \
./Packages/FatFs/fftools.d \
./Packages/FatFs/ffunicode.d 


# Each subdirectory must supply rules for building sources it contributes
Packages/FatFs/%.o: ../Packages/FatFs/%.c
	@	@	riscv-none-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=1 -DDEBUG=3 -DLOW_MEM=1 -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs\port" -I"E:\Project\tp78_v2_mini\MRSproject\StdPeriphDriver\inc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\common" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\core" -I"E:\Project\tp78_v2_mini\MRSproject\Startup" -I"E:\Project\tp78_v2_mini\MRSproject\HAL\include" -I"E:\Project\tp78_v2_mini\MRSproject\APP\include" -I"E:\Project\tp78_v2_mini\MRSproject\Profile\include" -I"E:\Project\tp78_v2_mini\MRSproject\Ld" -I"E:\Project\tp78_v2_mini\MRSproject\LIB" -I"E:\Project\tp78_v2_mini\MRSproject\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

