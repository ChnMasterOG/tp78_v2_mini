################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/CherryUSB/core/usbd_core.c 

OBJS += \
./Packages/CherryUSB/core/usbd_core.o 

C_DEPS += \
./Packages/CherryUSB/core/usbd_core.d 


# Each subdirectory must supply rules for building sources it contributes
Packages/CherryUSB/core/%.o: ../Packages/CherryUSB/core/%.c
	@	@	riscv-none-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=1 -DDEBUG=3 -DLOW_MEM=1 -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs\port" -I"E:\Project\tp78_v2_mini\MRSproject\StdPeriphDriver\inc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\common" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\core" -I"E:\Project\tp78_v2_mini\MRSproject\Startup" -I"E:\Project\tp78_v2_mini\MRSproject\HAL\include" -I"E:\Project\tp78_v2_mini\MRSproject\APP\include" -I"E:\Project\tp78_v2_mini\MRSproject\Profile\include" -I"E:\Project\tp78_v2_mini\MRSproject\Ld" -I"E:\Project\tp78_v2_mini\MRSproject\LIB" -I"E:\Project\tp78_v2_mini\MRSproject\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

