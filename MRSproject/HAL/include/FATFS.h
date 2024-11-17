/********************************** (C) COPYRIGHT *******************************
 * File Name          : FATFS.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/2/5
 * Description        : FatFs应用层驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __FATFS_H
  #define __FATFS_H

  #include "CH58x_common.h"

  #define FS_LINE_START                   0
  #define FS_LINE_BLE_DEVICE              0
  #define FS_LINE_LED_STYLE               1
  #define FS_LINE_WORK_MODE               2
  #define FS_LINE_UDISK_MODE              3
  #define FS_LINE_LED_BRIGHTNESS          4
  #define FS_LINE_RF_FREQ_LEVEL           5
  #define FS_LINE_LP_MAX_CNT              6

  uint8_t unsigned_dec_to_string(uint16_t num, char *buff, uint8_t type);
  uint8_t string_dec_to_u16(char *buff, uint16_t *num);
  uint8_t string_dec_to_u8(char *buff, uint8_t *num);

  void HAL_Fs_Init(void);
  void HAL_Fs_Create_readme(void);
  void HAL_Fs_Create_keyboard_cfg(uint8_t len, uint16_t* p_cfg);
  void HAL_Fs_Write_keyboard_cfg(uint8_t fs_line, uint8_t len, uint16_t* p_cfg);
  void HAL_Fs_Read_keyboard_cfg(uint8_t fs_line, uint8_t len, uint16_t* p_cfg);
  void HAL_Fs_Write_ble_device_info(BLE_Device_Info_t *ble_device_info);
  void HAL_Fs_Read_ble_device_info(BLE_Device_Info_t *ble_device_info);
  void HAL_Fs_Write_keyboard_spkey(uint8_t* hid_arr);
  void HAL_Fs_Read_keyboard_spkey(uint8_t* hid_arr);
  void HAL_Fs_Write_keyboard_mat(const uint8_t* fp, const uint8_t* key_arr);
  void HAL_Fs_Read_keyboard_mat(const uint8_t* fp, uint8_t* key_arr);

#endif
