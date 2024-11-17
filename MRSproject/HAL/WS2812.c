/********************************** (C) COPYRIGHT *******************************
 * File Name          : WS2812.c
 * Author             : ChnMasterOG
 * Version            : V1.2
 * Date               : 2022/4/13
 * Description        : WS2812驱动源文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(4))) UINT32 LED_DMA_Buffer[LED_NUMBER*24 + RESET_FRAME_SIZE] = { TIMING_RESET };  // LED的PWM脉冲翻转计数值缓冲区
UINT8 LED_BYTE_Buffer[LED_NUMBER][3] = { 0 };
WS2812_Style_Func led_style_func = WS2812_Style_Off;  // 默认背光函数
uint8_t g_LED_brightness = LED_DEFAULT_BRIGHTNESS;
WS2812_Style_E g_LED_Style = ws2812_style_off;
static uint8_t style_dir = 0;
static uint32_t style_cnt = 0;

const WS2812_Style_Func g_LED_Style_arr[] = {
                                             WS2812_Style_Custom,
                                             WS2812_Style_Breath,
                                             WS2812_Style_Waterful,
                                             WS2812_Style_Touch,
                                             WS2812_Style_Rainbow,
                                             WS2812_Style_Normal,
};

/*******************************************************************************
 * Function Name  : DATAFLASH_Read_LEDStyle
 * Description    : 从DataFlash读取LED样式
 * Input          : None
 * Return         : LED_Style_Number
 *******************************************************************************/
uint8_t DATAFLASH_Read_LEDStyle( void )
{
  uint16_t LED_Style_Number;
  HAL_Fs_Read_keyboard_cfg(FS_LINE_LED_STYLE, 1, &LED_Style_Number);
  g_LED_Style = LED_Style_Number;
#if (defined HAL_TPM) && (HAL_TPM == TRUE)
  TPM_set_backlight(g_LED_Style);
#endif
  g_keyboard_status.changeBL = TRUE;
  switch (LED_Style_Number)
  {
    case 0:
      led_style_func = WS2812_Style_Off;
      break;
    case 1:
      led_style_func = WS2812_Style_Breath;
      break;
    case 2:
      led_style_func = WS2812_Style_Waterful;
      break;
    case 3:
      led_style_func = WS2812_Style_Touch;
      break;
    case 4:
      led_style_func = WS2812_Style_Rainbow;
      break;
    case 5:
      led_style_func = WS2812_Style_Normal;
      break;
    default:
      led_style_func = WS2812_Style_Off;
      LED_Style_Number = 0;
      break;
  }
  return LED_Style_Number;
}

/*******************************************************************************
 * Function Name  : DATAFLASH_Write_LEDStyle
 * Description    : 将LED样式写入DataFlash
 * Input          : LED_Style_Number
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Write_LEDStyle( uint16_t LED_Style_Number )
{
  uint8_t check;

  HAL_Fs_Write_keyboard_cfg(FS_LINE_LED_STYLE, 1, &LED_Style_Number);
}

/*******************************************************************************
 * Function Name  : WS2812_PWM_Init
 * Description    : 初始化PWM
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_PWM_Init( void )
{
#ifdef USE_PWM1
  // 初始化 TMR1 PA10 用于输出PWM
  WS2812_GPIO_(ResetBits)( WS2812_Pin );
  WS2812_GPIO_(ModeCfg)( WS2812_Pin, GPIO_ModeOut_PP_5mA );

#if 0 // DMA搬运前重新初始化
  TMR1_PWMInit( High_Level, PWM_Times_1 );
  TMR1_PWMCycleCfg( 75 );        // 周期 1.25us
#endif

  WS2812_Style_Off( );

#else
  // 初始化 PB14 输出PWM
  WS2812_GPIO_(ModeCfg)( WS2812_Pin, GPIO_ModeOut_PP_5mA );    // PB14 - PWM10
  PWMX_CLKCfg( 1 );                   // cycle = 1/Fsys = 1/60us
  PWMX_CycleCfg( PWMX_Cycle_64 );     // 周期T = 64*cycle = 1.067us ~ 1.25us +- 600ns
  PWMX_ACTOUT( CH_PWM10, 0, Low_Level, ENABLE );     // 0% 占空比
#endif
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Off
 * Description    : 关闭WS2812
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Off( void )
{
  uint16_t i;

  for (i = 0; i < LED_NUMBER*24; i++) LED_DMA_Buffer[i] = TIMING_ZERO;
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Normal
 * Description    : 固定WS2812亮度
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Normal( void )
{
  uint16_t i, j, memaddr = 0;

  for (i = 0; i < LED_NUMBER; i++)
  {
    LED_BYTE_Buffer[i][GREEN_INDEX] = LED_BYTE_Buffer[i][RED_INDEX] = LED_BYTE_Buffer[i][BLUE_INDEX] = g_LED_brightness;
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
  }
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Breath
 * Description    : PWM驱动WS2812呼吸灯变化函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Breath( void )
{
  uint16_t i, j, memaddr = 0;

#if (defined HAL_TPM) && (HAL_TPM == TRUE)
  /* main keyboard is faster */
  if (TPM_get_backlight_ready()) {
    style_dir = 0;
    style_cnt = 0;
  }
#endif
  for (i = 0; i < LED_NUMBER; i++)
  {
    LED_BYTE_Buffer[i][GREEN_INDEX] = LED_BYTE_Buffer[i][RED_INDEX] = LED_BYTE_Buffer[i][BLUE_INDEX] = style_cnt;
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
  }
  /* change LED state */
  if (style_dir == 0) {  // 逐渐变亮 - GRB分量递增
    ++style_cnt;
  }
  else {  // 逐渐变暗 - GRB分量递减
    --style_cnt;
  }
  if (style_cnt == g_LED_brightness || style_cnt == 0 ) {
#if (defined HAL_TPM) && (HAL_TPM == TRUE)
    /* main keyboard is slower */
    if (style_cnt == 0 && TPM_get_TP78_alive())
      style_cnt = 1;
    else
#endif
      style_dir = !style_dir;
  }
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Waterful
 * Description    : PWM驱动WS2812流水灯变化函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Waterful( void )
{
  uint16_t j;
  uint32_t slow_cnt;

  if (style_cnt % Waterful_Repeat_Times != 0) {  // 控制周期*Waterful_Repeat_Times = 流水灯周期
    ++style_cnt;
    if (style_cnt >= LED_NUMBER * 3 * Waterful_Repeat_Times ) { // GRB轮流切换 + 120ms移动一个灯
      style_cnt = 0;
    }
    return;
  } else {
    slow_cnt = style_cnt / Waterful_Repeat_Times;
  }

  // 关闭上一个灯
  uint32_t last_cnt = slow_cnt == 0 ? LED_NUMBER-1 : slow_cnt-1;
  for (j = 0; j < 24; j++) {
    LED_DMA_Buffer[(last_cnt % LED_NUMBER) * 24 + j] = TIMING_ZERO;
  }
  // 开启下一个灯
  for (j = 0; j < 24; j++) {
    if ( j >= slow_cnt / LED_NUMBER * 8 + 4 && j < slow_cnt / LED_NUMBER * 8 + 8 ) {
      LED_DMA_Buffer[(slow_cnt % LED_NUMBER) * 24 + j] = TIMING_ONE;
    } else {
      LED_DMA_Buffer[(slow_cnt % LED_NUMBER) * 24 + j] = TIMING_ZERO;
    }
  }
  ++style_cnt;
  if (style_cnt >= LED_NUMBER * 3 * Waterful_Repeat_Times ) { // GRB轮流切换 + 120ms移动一个灯
    style_cnt = 0;
  }
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Touch
 * Description    : PWM驱动WS2812触控呼吸灯变化函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Touch( void )
{
  uint16_t i, j, memaddr = 0;

  for (i = 0; i < LED_NUMBER; i++)
  {
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    /* change LED state */
    if (LED_BYTE_Buffer[i][GREEN_INDEX] > 0) --LED_BYTE_Buffer[i][GREEN_INDEX];
    if (LED_BYTE_Buffer[i][RED_INDEX] > 0) --LED_BYTE_Buffer[i][RED_INDEX];
    if (LED_BYTE_Buffer[i][BLUE_INDEX] > 0) --LED_BYTE_Buffer[i][BLUE_INDEX];
  }
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Rainbow
 * Description    : PWM驱动WS2812彩虹灯变化函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Rainbow( void )
{
  signed int i;
  uint16_t j, memaddr = 0;

#if (defined HAL_TPM) && (HAL_TPM == TRUE)
  /* main keyboard is faster */
  if (TPM_get_backlight_ready()) {
    style_dir = 0;
    style_cnt = 0;
  }
#endif
  for (i = 0; i < LED_NUMBER; i++)
  {
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    /* 其他键盘布局需修改此处 - TODO */
    if (i >= 0 && i <= 3) { // first row - CYAN
      LED_BYTE_Buffer[i][GREEN_INDEX] = 128 - ABS((i - 0) - (signed int)style_cnt * 10 / 42) * 9;
      LED_BYTE_Buffer[i][RED_INDEX] = 0;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 128 - ABS((i - 0) - (signed int)style_cnt * 10 / 42) * 9;
    } else if (i >= 4 && i <= 7) { // second row - YELLOW
      LED_BYTE_Buffer[i][GREEN_INDEX] = 128 - ABS((i - 4) - (signed int)style_cnt * 10 / 42) * 9;
      LED_BYTE_Buffer[i][RED_INDEX] = 128 - ABS((i - 4) - (signed int)style_cnt * 10 / 42) * 9;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 0;
    } else if (i >= 8 && i <= 10) { // third row - PURPLE
      LED_BYTE_Buffer[i][GREEN_INDEX] = 0;
      LED_BYTE_Buffer[i][RED_INDEX] = 128 - ABS((i - 8) - (signed int)style_cnt * 10 / 42) * 9;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 128 - ABS((i - 8) - (signed int)style_cnt * 10 / 42) * 9;
    } else if (i >= 11 && i <= 13) { // fourth row - BLUE
      LED_BYTE_Buffer[i][GREEN_INDEX] = 0;
      LED_BYTE_Buffer[i][RED_INDEX] = 0;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 128 - ABS((i - 11) - (signed int)style_cnt * 10 / 42) * 9;
    } else if (i >= 14 && i <= 17) { // fifth row - GREEN
      LED_BYTE_Buffer[i][GREEN_INDEX] = 128 - ABS((i - 14) - (signed int)style_cnt * 10 / 42) * 9;
      LED_BYTE_Buffer[i][RED_INDEX] = 0;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 0;
    }
  }
  if (style_dir == 0) { // 从左向右
    ++style_cnt;
  } else {  // 从右向左
    --style_cnt;
  }
  if (style_cnt == 42 || style_cnt == 0) {
#if (defined HAL_TPM) && (HAL_TPM == TRUE)
    /* main keyboard is slower */
    if (style_cnt == 0 && TPM_get_TP78_alive())
      style_cnt = 1;
    else
#endif
      style_dir = !style_dir;
  }
}

/*******************************************************************************
 * Function Name  : WS2812_Style_Custom
 * Description    : PWM驱动WS2812自定义变化函数(先将RGB图案写入LED_BYTE_Buffer)
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Style_Custom( void )
{
  uint16_t i, j, memaddr = 0;

  /* transfer data */
  for (i = 0; i < LED_NUMBER; i++)
  {
    for (j = 0; j < 8; j++) // GREEN data
     {
       LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
       memaddr++;
     }
     for (j = 0; j < 8; j++) // RED data
     {
       LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
       memaddr++;
     }
     for (j = 0; j < 8; j++) // BLUE data
     {
       LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
       memaddr++;
     }
  }
}

/*******************************************************************************
 * Function Name  : WS2812发送RGB数据
 * Description    : DMA+PWM驱动WS2812
 * Input          : None
 * Return         : None
 *******************************************************************************/
void WS2812_Send( void )
{
  uint16_t i;

  if ( g_keyboard_status.changeBL == TRUE ) {
    g_keyboard_status.changeBL = FALSE;
    for (i = 0; i < LED_NUMBER; i++) {  // memory set zero
      LED_BYTE_Buffer[i][0] = LED_BYTE_Buffer[i][1] = LED_BYTE_Buffer[i][2] = 0;
    }
    style_dir = style_cnt = 0;
    WS2812_Style_Off( );
  } else {
    led_style_func();  // 调用变化函数
  }

  { // WCH CH582M bug: 不重新初始化TMR_PWM则发送PWM+DMA偶现第一个非空Byte丢失
    TMR1_PWMInit( High_Level, PWM_Times_1 );
    TMR1_PWMCycleCfg( 75 );        // 周期 1.25us
  }
  TMR1_DMACfg( ENABLE, (UINT16) (UINT32) LED_DMA_Buffer, (UINT16) (UINT32) (LED_DMA_Buffer + LED_NUMBER*24 + RESET_FRAME_SIZE), Mode_Single );  // 启用DMA转换，从内存到外设
}

