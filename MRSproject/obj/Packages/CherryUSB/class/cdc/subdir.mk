################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/CherryUSB/class/cdc/usbd_cdc.c \
../Packages/CherryUSB/class/cdc/usbh_cdc_acm.c 

OBJS += \
./Packages/CherryUSB/class/cdc/usbd_cdc.o \
./Packages/CherryUSB/class/cdc/usbh_cdc_acm.o 

C_DEPS += \
./Packages/CherryUSB/class/cdc/usbd_cdc.d \
./Packages/CherryUSB/class/cdc/usbh_cdc_acm.d 


# Each subdirectory must supply rules for building sources it contributes
Packages/CherryUSB/class/cdc/%.o: ../Packages/CherryUSB/class/cdc/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=3 -I"../BLE_CH582M/StdPeriphDriver/inc" -I"../BLE_CH582M/Packages/CherryUSB" -I"../BLE_CH582M/Packages/CherryUSB/class/hid" -I"../BLE_CH582M/Packages/CherryUSB/class/msc" -I"../BLE_CH582M/Packages/CherryUSB/common" -I"../BLE_CH582M/Packages/CherryUSB/core" -I"../BLE_CH582M/Startup" -I"../BLE_CH582M/HAL/include" -I"../BLE_CH582M/APP/include" -I"../BLE_CH582M/Profile/include" -I"../BLE_CH582M/Ld" -I"../BLE_CH582M/LIB" -I"../BLE_CH582M/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

