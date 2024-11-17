################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/BATTERY.c \
../HAL/BLE.c \
../HAL/CORE.c \
../HAL/FATFS.c \
../HAL/HW_I2C.c \
../HAL/I2C_TP.c \
../HAL/KEYBOARD.c \
../HAL/MCU.c \
../HAL/RF_PHY.c \
../HAL/RTC.c \
../HAL/SLEEP.c \
../HAL/USB.c \
../HAL/WS2812.c 

OBJS += \
./HAL/BATTERY.o \
./HAL/BLE.o \
./HAL/CORE.o \
./HAL/FATFS.o \
./HAL/HW_I2C.o \
./HAL/I2C_TP.o \
./HAL/KEYBOARD.o \
./HAL/MCU.o \
./HAL/RF_PHY.o \
./HAL/RTC.o \
./HAL/SLEEP.o \
./HAL/USB.o \
./HAL/WS2812.o 

C_DEPS += \
./HAL/BATTERY.d \
./HAL/BLE.d \
./HAL/CORE.d \
./HAL/FATFS.d \
./HAL/HW_I2C.d \
./HAL/I2C_TP.d \
./HAL/KEYBOARD.d \
./HAL/MCU.d \
./HAL/RF_PHY.d \
./HAL/RTC.d \
./HAL/SLEEP.d \
./HAL/USB.d \
./HAL/WS2812.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=1 -DDEBUG=3 -DLOW_MEM=1 -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\FatFs\port" -I"E:\Project\tp78_v2_mini\MRSproject\StdPeriphDriver\inc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\common" -I"E:\Project\tp78_v2_mini\MRSproject\Packages\CherryUSB\core" -I"E:\Project\tp78_v2_mini\MRSproject\Startup" -I"E:\Project\tp78_v2_mini\MRSproject\HAL\include" -I"E:\Project\tp78_v2_mini\MRSproject\APP\include" -I"E:\Project\tp78_v2_mini\MRSproject\Profile\include" -I"E:\Project\tp78_v2_mini\MRSproject\Ld" -I"E:\Project\tp78_v2_mini\MRSproject\LIB" -I"E:\Project\tp78_v2_mini\MRSproject\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

