/********************************** (C) COPYRIGHT *******************************
 * File Name          : KEYBOARD.c
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/12/1
 * Description        : 机械键盘驱动源文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include <stdio.h>
#include "HAL.h"

const uint32_t Row_Pin[ROW_SIZE] = {GPIO_Pin_13, GPIO_Pin_12, GPIO_Pin_11, GPIO_Pin_15, GPIO_Pin_14};   //row 5 - 其它键盘布局需修改此处
const uint32_t Colum_Pin[COL_SIZE] = {GPIO_Pin_6, GPIO_Pin_0, GPIO_Pin_4, GPIO_Pin_5};   //colum 4 - 其它键盘布局需修改此处
//row*colum = 5*4 = 20
const uint8_t KeyArrary[ROW_SIZE][COL_SIZE] = {
          //---- COL 0 ----|-- COL 1 --|--- COL 2 ---|--- COL 3 ---|
/* ROW0 */{ KEYPAD_NumLock, KEYPAD_Div, KEYPAD_Mul,   KEYPAD_Sub   },
/* ROW1 */{ KEYPAD_7,       KEYPAD_8,   KEYPAD_9,     KEYPAD_Add   },
/* ROW2 */{ KEYPAD_4,       KEYPAD_5,   KEYPAD_6,     KEY_None     },
/* ROW3 */{ KEYPAD_1,       KEYPAD_2,   KEYPAD_3,     KEY_None     },
/* ROW4 */{ KEY_Fn,         KEYPAD_0,   KEYPAD_Delete,KEYPAD_Enter },
};  // 默认键盘布局 - 其它键盘布局需修改此处
const uint8_t SP_KeyArrary[][8] = {
        { 0x5,      0x0,      KEY_Delete,       0x0,      0x0,      0x0,      0x0,      0x0 }, //1 - Ctrl+Alt+Del
        { 0x3,      0x0,      KEY_Delete,       0x0,      0x0,      0x0,      0x0,      0x0 }, //2 - Ctrl+Shift+Del
        { 0x0,      0x0,      0x0,              0x0,      0x0,      0x0,      0x0,      0x0 }, //3
        { 0x0,      0x0,      0x0,              0x0,      0x0,      0x0,      0x0,      0x0 }, //4
        { 0x0,      0x0,      0x0,              0x0,      0x0,      0x0,      0x0,      0x0 }, //5
        { 0x9,      0x0,      KEY_LeftArrow,    0x0,      0x0,      0x0,      0x0,      0x0 }, //6 - GUI+Ctrl+Left
        { 0x9,      0x0,      KEY_RightArrow,   0x0,      0x0,      0x0,      0x0,      0x0 }, //7 - GUI+Ctrl+Right
};  // SP按键默认映射
const uint8_t Key_To_LEDNumber[ROW_SIZE][COL_SIZE] = {
/* ROW0 */{ 0,  1,  2,  3 },
/* ROW1 */{ 4,  5,  6,  7 },
/* ROW2 */{ 8,  9,  10, 0 },
/* ROW3 */{ 11, 12, 13, 0},
/* ROW4 */{ 14, 15, 16, 17},
};  // 矩阵键盘位置转LED编号
uint32_t Row_Pin_ALL = 0, Colum_Pin_ALL = 0;
uint8_t CustomKey[ROW_SIZE][COL_SIZE];  //当前按键层
uint8_t Extra_CustomKey[LAYER_SIZE][ROW_SIZE][COL_SIZE];   //额外按键层
uint8_t KeyMatrix[ROW_SIZE][COL_SIZE] = { 0 };  //按键矩阵-标记按下和未按下
uint8_t SP_Key_Map[SP_KEY_NUMBER][8] = { 0 }; //复合按键
Keyboard_Status_t g_keyboard_status;

static uint8_t (*KeyArr_Ptr)[COL_SIZE] = CustomKey;

/*******************************************************************************
 * Function Name  : DATAFLASH_Read_KeyArray
 * Description    : 从DataFlash读取按键矩阵
 * Input          : None
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Read_KeyArray( void )
{
  uint8_t i;
  char buf[64];

  HAL_Fs_Read_keyboard_mat("0:keyboard_mat.txt", (uint8_t*)CustomKey);
  for (i = 0; i < LAYER_SIZE; i++) {
    sprintf(buf, "0:keyboard_ext_%d_mat.txt", i);
    HAL_Fs_Read_keyboard_mat(buf, (uint8_t*)Extra_CustomKey[i]);
  }
}

/*******************************************************************************
 * Function Name  : DATAFLASH_Write_KeyArray
 * Description    : 将按键矩阵写入DataFlash
 * Input          : None
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Write_KeyArray( void )
{
  uint8_t i;
  char buf[64];

  HAL_Fs_Write_keyboard_mat("0:keyboard_mat.txt", (const uint8_t*)CustomKey);
  for (i = 0; i < LAYER_SIZE; i++) {
    sprintf(buf, "0:keyboard_ext_%d_mat.txt", i);
    HAL_Fs_Write_keyboard_mat(buf, (uint8_t*)Extra_CustomKey[i]);
  }
}

/*******************************************************************************
 * Function Name  : KEYBOARD_Reset
 * Description    : 重置FLASH
 * Input          : None
 * Return         : None
 *******************************************************************************/
void KEYBOARD_Reset( void )
{
  uint16_t param[] = { 0, 1, 0, 0,
                       LED_DEFAULT_BRIGHTNESS, 2,  // RF频率默认2.405G
                       240
  };

  memcpy(CustomKey, KeyArrary, COL_SIZE*ROW_SIZE);
  for (uint8_t i = 0; i < LAYER_SIZE; i++)
    memcpy(Extra_CustomKey[i], KeyArrary, COL_SIZE*ROW_SIZE);
  memcpy(SP_Key_Map, SP_KeyArrary, SP_KEY_NUMBER*8);
  DATAFLASH_Write_KeyArray( );
  HAL_Fs_Write_keyboard_spkey((uint8_t*)SP_Key_Map);
  HAL_Fs_Create_keyboard_cfg(sizeof(param)/sizeof(uint16_t), param);
  HAL_Fs_Create_readme();
}

/*******************************************************************************
 * Function Name  : KEYBOARD_ChangeKey
 * Description    : 键盘交换2个按键
 * Input          : dst_key - 目标键, src_key - 原始键
 * Return         : None
 *******************************************************************************/
void KEYBOARD_ChangeKey( uint8_t dst_key, uint8_t src_key )
{
  uint8_t i = COL_SIZE*ROW_SIZE;
  uint8_t* memaddr = &CustomKey[0][0];
  while (i) {
    --i;
    /* 交换src键和dst键 */
    if ( *(memaddr + i) == src_key ) *(memaddr + i) = dst_key;
    else if ( *(memaddr + i) == dst_key ) *(memaddr + i) = src_key;
  }
  DATAFLASH_Write_KeyArray( );
}

/*******************************************************************************
 * Function Name  : KEYBOARD_Custom_Function
 * Description    : 键盘定制化功能(检测Fn键触发的相关功能)
 * Input          : None
 * Return         : 如果不需要发送按键信息则返回0
 *******************************************************************************/
UINT8 KEYBOARD_Custom_Function( void )
{
  static uint16_t last_state = 0;
  static uint32_t press_time = 0;
  static uint8_t Fn_Mode = 0;
  static uint8_t Fn_cnt = 0;  // 4/2/1/1 bit for Reset/ChangeKey/SaveDevices/Reserved
  static uint8_t src_key = 0, dst_key = 0;

  if ( g_keyboard_status.Fn == TRUE && KeyboardDat->Key2 == KEY_None ) { // 按下Fn+1个键或者只有Fn键
    last_state = 1;
    press_time = sys_time;
    if ( KeyboardDat->Key1 == KEY_None && Fn_Mode == Fn_Mode_None ) { // 键盘布局复位
      Fn_Mode = Fn_Mode_Reset;
      Fn_cnt &= 0xF0;   // 清除其余计数器
    } else if ( Fn_Mode == Fn_Mode_ChangeKey ) {  // 设置改键 - 按Fn+src和Fn+dst
      if ( Fn_cnt == 0x04 ) src_key = KeyboardDat->Key1;
      else if ( Fn_cnt == 0x08 ) dst_key = KeyboardDat->Key1;
    } else if ( KeyboardDat->Key1 == KEY_C && Fn_Mode != Fn_Mode_ChangeKey ) { // 设置改键 - 先按Fn+C
      Fn_Mode = Fn_Mode_ChangeKey;
      Fn_cnt &= 0x0C;
    } else if ( KeyboardDat->Key1 == KEYPAD_Delete && Fn_Mode != Fn_Mode_SoftReset ) { // 软件复位模式
      Fn_Mode = Fn_Mode_SoftReset;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_NumLock && Fn_Mode != Fn_Mode_JumpBoot ) { // 跳转BOOT模式
      Fn_Mode = Fn_Mode_JumpBoot;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_Sub && Fn_Mode != Fn_Mode_VolumeDown ) { // 音量减模式
      Fn_Mode = Fn_Mode_VolumeDown;
      HIDVolume[0] |= Volume_Decr;
      HID_VOL_Process();
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_Add && Fn_Mode != Fn_Mode_VolumeUp ) { // 音量加模式
      Fn_Mode = Fn_Mode_VolumeUp;
      HIDVolume[0] |= Volume_Incr;
      HID_VOL_Process();
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 >= KEYPAD_1 && KeyboardDat->Key1 <= KEYPAD_5 && Fn_Mode != Fn_Mode_ChangeLayer0 + (KeyboardDat->Key1 - KEYPAD_1) ) { // 切换按键层
      Fn_Mode = Fn_Mode_ChangeLayer0 + (KeyboardDat->Key1 - KEYPAD_1);
      Fn_cnt = 0;
    }  else if ( KeyboardDat->Key1 == KEYPAD_7 && Fn_Mode != Fn_Mode_USBMode ) { // 切换USB模式
      Fn_Mode = Fn_Mode_USBMode;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_8 && Fn_Mode != Fn_Mode_BLEMode && g_Enable_Status.ble == FALSE ) { // 切换蓝牙模式
      Fn_Mode = Fn_Mode_BLEMode;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_9 && Fn_Mode != Fn_Mode_RFMode ) { // 切换RF模式
      Fn_Mode = Fn_Mode_RFMode;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_0 && Fn_Mode != Fn_Mode_UDiskMode && g_Enable_Status.ble == FALSE ) { // 开启U盘模式
      Fn_Mode = Fn_Mode_UDiskMode;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_8 && Fn_Mode != Fn_Mode_BLE_ClearSNV && g_Enable_Status.ble == TRUE ) { // 清除蓝牙SNV信息
      Fn_Mode = Fn_Mode_BLE_ClearSNV;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_0 && Fn_Mode != Fn_Mode_SelectNextDevice && g_Enable_Status.ble == TRUE ) { // 切换至下一个蓝牙设备
      Fn_Mode = Fn_Mode_SelectNextDevice;
      Fn_cnt = 0;
    } else if ( KeyboardDat->Key1 == KEYPAD_Div && Fn_Mode != Fn_Mode_LED_NextStyle ) { // 背光模式
      Fn_Mode = Fn_Mode_LED_NextStyle;
      Fn_cnt = 0;
    } else {
      if ( Fn_Mode == Fn_Mode_Reset ) return 0;
      if ( Fn_Mode != Fn_Mode_None && KeyboardDat->Key1 == KEY_None ) { // 先松开其他按键(后松开Fn)
        goto fn_do_operations;
      }
      Fn_Mode = Fn_Mode_None;
      Fn_cnt = 0;
    }
    return 0;
  } else if ( last_state > 0 && g_keyboard_status.Fn == FALSE ) {  // 松开Fn执行指令
    fn_do_operations:
    switch ( Fn_Mode ) {
      case Fn_Mode_Reset: { // 按下5次Fn重置按键
        Fn_cnt += 0x10;
        if ( Fn_cnt == 0x50 ) {
          Fn_cnt = 0;
          Fn_Mode = Fn_Mode_None;
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
          WWDG_ResetCfg(DISABLE); // 关看门狗
#endif
          KEYBOARD_Reset( );
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
          WWDG_ResetCfg(ENABLE);  // 开看门狗
#endif
//          OLED_UI_add_SHOWINFO_task("Reset OK!");
//          OLED_UI_add_CANCELINFO_delay_task(3000);
        } else if ( Fn_cnt >= 0x30 ) {
//          OLED_UI_add_SHOWINFO_task("%d/5 Reset", Fn_cnt >> 4);
//          OLED_UI_add_CANCELINFO_delay_task(1500);
        } else if ( Fn_cnt == 0x20 && sys_time - press_time >= 3 * (1000 / SYS_PERIOD) ) {  // 第二下Fn长按进board boot --- 防止误擦除固件，该功能暂时屏蔽
          //-APPJumpBoot();
        }
        break;
      }
      case Fn_Mode_ChangeKey: { // 交换按键 - 小键盘不支持
        Fn_cnt += 0x04;
        if ( Fn_cnt == 0x0C ) {
          Fn_cnt = 0;
          Fn_Mode = Fn_Mode_None;
//          OLED_UI_add_SHOWINFO_task("Change OK!");
//          OLED_UI_add_CANCELINFO_delay_task(3000);
          KEYBOARD_ChangeKey( dst_key, src_key );
        }
        break;
      }
      case Fn_Mode_SoftReset: { // Fn+Del软件复位
        Fn_Mode = Fn_Mode_None;
        SoftReset();
        break;
      }
      case Fn_Mode_RFJumptoBoot: {  // 发送0x7A让接收器进BOOT - 小键盘不支持
        Fn_Mode = Fn_Mode_None;
        if (g_Enable_Status.rf == TRUE) {
          tmos_set_event( RFTaskId, SBP_RF_JUMPBOOT_REPORT_EVT );  // RF JUMPBOOT事件
        }
        break;
      }
      case Fn_Mode_JumpBoot: {  // Fn+Numlock跳转kboot
        enter_boot_mode:
        Fn_Mode = Fn_Mode_None;
        APPJumpKBoot();
        break;
      }
      case Fn_Mode_VolumeDown: {  // Fn+减号减小音量 - 松开停止
        Fn_Mode = Fn_Mode_None;
        HIDVolume[0] &= ~Volume_Decr;
        HID_VOL_Process();
        break;
      }
      case Fn_Mode_VolumeUp: {  // Fn+加号增加音量 - 松开停止
        Fn_Mode = Fn_Mode_None;
        HIDVolume[0] &= ~Volume_Incr;
        HID_VOL_Process();
        break;
      }
      case Fn_Mode_ChangeLayer0 ... Fn_Mode_ChangeLayer4: { // Fn+1~5切换按键层
        if (Fn_Mode == Fn_Mode_ChangeLayer0) {
          KeyArr_Ptr = CustomKey;
        } else {
          KeyArr_Ptr = Extra_CustomKey[Fn_Mode - Fn_Mode_ChangeLayer0 - 1];
        }
        Fn_Mode = Fn_Mode_None;
        break;
      }
      case Fn_Mode_USBMode: { // Fn+7切换USB模式
        Fn_Mode = Fn_Mode_None;
        if (g_Enable_Status.usb == TRUE) {
//          OLED_UI_add_SHOWINFO_task("USB Mode");
//          OLED_UI_add_CANCELINFO_delay_task(2000);
          break;
        }
        uint16_t Usb_mode = 0;
        HAL_Fs_Write_keyboard_cfg(FS_LINE_WORK_MODE, 1, &Usb_mode);
        SoftReset();
        break;
      }
      case Fn_Mode_BLEMode: { // Fn+8切换蓝牙模式
        Fn_Mode = Fn_Mode_None;
        if (g_Enable_Status.ble == TRUE) {
//          OLED_UI_add_SHOWINFO_task("BLE Mode");
//          OLED_UI_add_CANCELINFO_delay_task(2000);
          break;
        }
        uint16_t Ble_mode = 1;
        HAL_Fs_Write_keyboard_cfg(FS_LINE_WORK_MODE, 1, &Ble_mode);
        SoftReset();
        break;
      }
      case Fn_Mode_RFMode: { // Fn+9切换RF模式
        Fn_Mode = Fn_Mode_None;
        if (g_Enable_Status.rf == TRUE) {
//          OLED_UI_add_SHOWINFO_task("RF Mode");
//          OLED_UI_add_CANCELINFO_delay_task(2000);
          break;
        }
        uint16_t Rf_mode = 2;
        HAL_Fs_Write_keyboard_cfg(FS_LINE_WORK_MODE, 1, &Rf_mode);
        SoftReset();
        break;
      }
      case Fn_Mode_UDiskMode: { // Fn+0开启U盘模式
        Fn_Mode = Fn_Mode_None;
        uint16_t Usb_mode = 0;
        uint16_t Udisk_mode = 1;
        HAL_Fs_Write_keyboard_cfg(FS_LINE_WORK_MODE, 1, &Usb_mode);
        HAL_Fs_Write_keyboard_cfg(FS_LINE_UDISK_MODE, 1, &Udisk_mode);
        SoftReset();
        break;
      }
      case Fn_Mode_GameMode: { // 开关性能模式 - 小键盘不支持
        Fn_Mode = Fn_Mode_None;
        g_Game_Mode = !g_Game_Mode;
        if (g_Game_Mode == FALSE) {
//          OLED_UI_add_SHOWINFO_task("G Mode OFF");
//          OLED_UI_add_CANCELINFO_delay_task(2000);
          tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 ); // 主循环
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
          tmos_start_task( halTaskID, MPR121_EVENT, MS1_TO_SYSTEM_TIME(30) );  // MPR121
#endif
#if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)) || ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE))
          tmos_start_task( halTaskID, HAL_MOUSE_EVENT, MS1_TO_SYSTEM_TIME(40) ); // 鼠标事件
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
          tmos_start_task( halTaskID, OLED_UI_EVENT, MS1_TO_SYSTEM_TIME(60) );  // OLED UI
#endif
#if ((defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)) && ((defined HAL_TPM) && (HAL_TPM == TRUE))
          tmos_start_task( halTaskID, TPM_EVENT, MS1_TO_SYSTEM_TIME(30) );  // 扩展模块
#endif
        } else {
//          OLED_PRINT("G Mode ON");
        }
        break;
      }
      case Fn_Mode_BLE_ClearSNV: {  // Fn+8清除蓝牙SNV信息(蓝牙模式下)
        Fn_Mode = Fn_Mode_None;
        uint32_t SNV_size, p_addr = BLE_SNV_ADDR;
        uint16_t i;
        uint8_t e_buf[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
        WWDG_ResetCfg(DISABLE); // 关看门狗
#endif
        SNV_size = 0x8000 - BLE_SNV_ADDR;
        EEPROM_ERASE(BLE_SNV_ADDR, SNV_size);
        SNV_size >>= 2;
        for (i = 0; i < SNV_size; i++) {
          EEPROM_WRITE(p_addr, e_buf, 4); // 填充0xFF
          p_addr += 4;
        }
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
        WWDG_ResetCfg(ENABLE);  // 开看门狗
#endif
        if (g_Enable_Status.ble == TRUE) {
//          OLED_PRINT("Erase OK");
          DelayMs(50);
          SoftReset();
        }
        break;
      }
      case Fn_Mode_SelectNextDevice: { // 按Fn+0切换设备(蓝牙模式下)
        DeviceAddress[5] = DeviceAddress[5] + 1;
        if (DeviceAddress[5] > 4)
          DeviceAddress[5] = 0;
        Fn_Mode = Fn_Mode_None;
        DATAFLASH_Write_DeviceID(DeviceAddress[5] - 1);
        SoftReset();
        break;
      }
      case Fn_Mode_LED_NextStyle: {
        Fn_Mode = Fn_Mode_None;
        g_keyboard_status.changeBL = TRUE;
#ifdef OLED_0_91
        OLED_UI_add_SHOWSTRING_task(20, 1, "S0");
#endif
#ifdef OLED_0_66
        OLED_UI_add_SHOW_ICONINFO_task(OLED_UI_ICON_LED_STYLE_IDX, " OFF");
        OLED_UI_add_CANCELINFO_delay_task(2000);
#endif
        g_LED_Style = g_LED_Style + 1;
        if (g_LED_Style == ws2812_style_max)
          g_LED_Style = ws2812_style_off;
        led_style_func = g_LED_Style_arr[g_LED_Style];
        break;
      }
      case Fn_Mode_GiveUp: {
        Fn_cnt = 0;
        Fn_Mode = Fn_Mode_None;
        break;
      }
      default:  // 未识别的指令
        ;
    }
    last_state = 0;
    return 0;
  }
  return 1;
}

/*******************************************************************************
 * Function Name  : KEYBOARD_Init
 * Description    : 键盘初始化函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void KEYBOARD_Init( void )
{
    uint8_t i;
    DATAFLASH_Read_KeyArray( );   // DataFlash载入按键
    HAL_Fs_Read_keyboard_spkey((uint8_t*)SP_Key_Map);   // DataFlash载入spKEY
    CustomKey[4][0] = KEY_Fn;   // 保证上电Fn键在对应位置 - 其它键盘布局需修改此处

    for (i = 0; i < ROW_SIZE; i++) {
        Row_Pin_ALL |= Row_Pin[i];
    }
#if (ROW_SCAN_MODE)
    Row_GPIO_(ResetBits)( Row_Pin_ALL );
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeOut_PP_5mA );
    Row_GPIO_(SetBits)( Row_Pin[0] );
#else
    Row_GPIO_(ResetBits)( Row_Pin_ALL );
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeIN_PU );
#endif

    for (i = 0; i < COL_SIZE; i++) {
        Colum_Pin_ALL |= Colum_Pin[i];
    }
#if (ROW_SCAN_MODE)
    Colum_GPIO_(ResetBits)( Colum_Pin_ALL );
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PD );
#else
    Colum_GPIO_(SetBits)( Colum_Pin_ALL );
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeOut_PP_20mA );
    Colum_GPIO_(ResetBits)( Colum_Pin[0] );
#endif
}

/*******************************************************************************
 * Function Name  : KEYBOARD_Detection
 * Description    : 键盘检测按键信息函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void KEYBOARD_Detection( void )
{
#if (ROW_SCAN_MODE)
    static uint8_t current_row = 0;
    uint8_t current_colum;
#else
    static uint8_t current_colum = 0;
    uint8_t current_row;
#endif
    static BOOL press_Normal_Key = FALSE;
    uint8_t key_idx, i, j;

    /* 基础功能 */
#if (ROW_SCAN_MODE)
    for (current_colum = 0; current_colum < COL_SIZE; current_colum++) {    // 查询哪一列改变
        if (KeyMatrix[current_row][current_colum] == 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) != 0) {  // 按下
#else
    for (current_row = 0; current_row < ROW_SIZE; current_row++) {    // 查询哪一行改变
        if (KeyMatrix[current_row][current_colum] == 0 && Row_GPIO_(ReadPortPin)( Row_Pin[current_row] ) == 0) {  // 按下
#endif
            if (g_keyboard_status.SP_Key != 0) continue;  // SP键生效后不接受其余按键
            KeyMatrix[current_row][current_colum] = 1;  // 矩阵状态变成按下
            if (led_style_func == WS2812_Style_Touch && Key_To_LEDNumber[current_row][current_colum] != 0xFF) { // 触控呼吸灯模式
                LED_BYTE_Buffer[Key_To_LEDNumber[current_row][current_colum]][0] =
                LED_BYTE_Buffer[Key_To_LEDNumber[current_row][current_colum]][1] =
                LED_BYTE_Buffer[Key_To_LEDNumber[current_row][current_colum]][2] = g_LED_brightness;
            }
            g_Ready_Status.keyboard_key_data = TRUE; // 产生事件

            if (KeyArr_Ptr[current_row][current_colum] == KEY_None) {  // 不做处理
                continue;
            } else if (KeyArr_Ptr[current_row][current_colum] == KEY_Fn) {  // 功能键
                g_keyboard_status.Fn = TRUE;
            } else if (KeyArr_Ptr[current_row][current_colum] >= KEY_SP_1) {  // SP键(单键复合)
                g_keyboard_status.SP_Key = KeyArr_Ptr[current_row][current_colum] - KEY_SP_1 + 1;
                press_Normal_Key = TRUE;
                memcpy(KeyboardDat->data, SP_Key_Map[g_keyboard_status.SP_Key - 1], 8);
            } else if (KeyArr_Ptr[current_row][current_colum] >= KEY_MouseL) {    // 鼠标操作
                MouseDat->data[0] |= 1 << KeyArr_Ptr[current_row][current_colum] - KEY_MouseL;
                press_Normal_Key = TRUE;
                g_Ready_Status.keyboard_mouse_data = TRUE;  // 产生鼠标事件
            } else if (KeyArr_Ptr[current_row][current_colum] >= KEY_LeftCTRL) {    // Ctrl等特殊键
                press_Normal_Key = TRUE;
                KeyboardDat->data[0] |= 1 << (KeyArr_Ptr[current_row][current_colum] - KEY_LeftCTRL);
            } else {
                press_Normal_Key = TRUE;
                for (key_idx = 2; key_idx < 8; key_idx++) {
                    if (KeyboardDat->data[key_idx] == KEY_None) {
                        KeyboardDat->data[key_idx] = KeyArr_Ptr[current_row][current_colum];
                        break;
                    }
                }
            }
#if (ROW_SCAN_MODE)
        } else if (KeyMatrix[current_row][current_colum] != 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) == 0) {   // 弹起
#else
        } else if (KeyMatrix[current_row][current_colum] != 0 && Row_GPIO_(ReadPortPin)( Row_Pin[current_row] ) != 0) {   // 弹起
#endif
            KeyMatrix[current_row][current_colum] = 0;
            g_Ready_Status.keyboard_key_data = TRUE; // 产生事件
            if (KeyArr_Ptr[current_row][current_colum] == KEY_Fn) {  // 功能键
                g_keyboard_status.Fn = FALSE;
            } else if (KeyArr_Ptr[current_row][current_colum] >= KEY_SP_1) {  // SP键(单键复合)
                g_keyboard_status.SP_Key = 0;
                memset(KeyboardDat->data, 0, HID_KEYBOARD_DATA_LENGTH);
            } else if (KeyArr_Ptr[current_row][current_colum] >= KEY_MouseL) {    // 鼠标操作
                MouseDat->data[0] &= ~(1 << KeyArr_Ptr[current_row][current_colum] - KEY_MouseL);
                g_Ready_Status.keyboard_mouse_data = TRUE;  // 产生鼠标事件
            } else if (KeyArr_Ptr[current_row][current_colum] >= KEY_LeftCTRL) {    // Ctrl等特殊键
                KeyboardDat->data[0] &= ~(1 << (KeyArr_Ptr[current_row][current_colum] - KEY_LeftCTRL));
            } else {
                for (key_idx = 2; key_idx < 8; key_idx++) {
                    if (KeyboardDat->data[key_idx] == KeyArr_Ptr[current_row][current_colum]) {
                        KeyboardDat->data[key_idx] = KEY_None;
                    }
                }
            }
        }
    }
#if (ROW_SCAN_MODE)
    Row_GPIO_(ResetBits)( Row_Pin[current_row++] );
    if (current_row >= ROW_SIZE) {
        current_row = 0;
    }
    Row_GPIO_(SetBits)( Row_Pin[current_row] );
#else
    Colum_GPIO_(SetBits)( Colum_Pin[current_colum++] );
    if (current_colum >= COL_SIZE) {
        current_colum = 0;
    }
    Colum_GPIO_(ResetBits)( Colum_Pin[current_colum] );
#endif
}
