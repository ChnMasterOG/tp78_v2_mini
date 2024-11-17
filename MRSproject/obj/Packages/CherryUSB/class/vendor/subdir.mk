################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/CherryUSB/class/vendor/axusbnet.c \
../Packages/CherryUSB/class/vendor/usbh_air724.c 

OBJS += \
./Packages/CherryUSB/class/vendor/axusbnet.o \
./Packages/CherryUSB/class/vendor/usbh_air724.o 

C_DEPS += \
./Packages/CherryUSB/class/vendor/axusbnet.d \
./Packages/CherryUSB/class/vendor/usbh_air724.d 


# Each subdirectory must supply rules for building sources it contributes
Packages/CherryUSB/class/vendor/%.o: ../Packages/CherryUSB/class/vendor/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=3 -I"../BLE_CH582M/StdPeriphDriver/inc" -I"../BLE_CH582M/Packages/CherryUSB" -I"../BLE_CH582M/Packages/CherryUSB/class/hid" -I"../BLE_CH582M/Packages/CherryUSB/class/msc" -I"../BLE_CH582M/Packages/CherryUSB/common" -I"../BLE_CH582M/Packages/CherryUSB/core" -I"../BLE_CH582M/Startup" -I"../BLE_CH582M/HAL/include" -I"../BLE_CH582M/APP/include" -I"../BLE_CH582M/Profile/include" -I"../BLE_CH582M/Ld" -I"../BLE_CH582M/LIB" -I"../BLE_CH582M/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

