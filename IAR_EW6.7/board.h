#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32f0xx.h"

//--------GPIOA---------
// PWM2 - digital OUTPUT
#define PWM2_PORT   GPIOA
#define PMW2_PIN    GPIO_Pin_3
// DAC OUT - analog OUTPUT
#define DAC_PORT  GPIOA
#define DAC_PIN   GPIO_Pin_4
// SIGNAL OUT - analog INPUT
#define SIGNAL_PORT   GPIOA
#define SIGNAL_PIN    GPIO_Pin_5
// LCD RS - digital OUTPUT
#define LCD_RS_PORT  GPIOA 
#define LCD_RS_PIN   GPIO_Pin_6
// LCD D7 - digital OUTPUT
#define LCD_D7_PORT  GPIOA 
#define LCD_D7_PIN   GPIO_Pin_7
// PWM1 - digital OUTPUT
#define PWM1_PORT   GPIOA
#define PMW1_PIN    GPIO_Pin_8
// USART - digital I/O
#define USART_PORT    GPIOA
#define USART_TX_PIN  GPIO_Pin_9
#define USART_RX_PIN  GPIO_Pin_10
//-----------end GPIOA------------

//--------GPIOB---------
// LCD E - digital OUTPUT
#define LCD_EN_PORT  GPIOB 
#define LCD_EN_PIN   GPIO_Pin_0
// LCD D4 - digital OUTPUT
#define LCD_D4_PORT  GPIOB 
#define LCD_D4_PIN   GPIO_Pin_1
// LCD D5 - digital OUTPUT
#define LCD_D5_PORT  GPIOB 
#define LCD_D5_PIN   GPIO_Pin_2
// LCD D6 - digital OUTPUT
#define LCD_D6_PORT  GPIOB 
#define LCD_D6_PIN   GPIO_Pin_10
// LCD BACKLIGHT - digital OUTPUT
#define LCD_LIGHT_PORT  GPIOB 
#define LCD_LIGHT_PIN   GPIO_Pin_11
#define LCD_LIGHT_STATE ((u16)(LCD_LIGHT_PORT->IDR & LCD_LIGHT_PIN) != (u16)0 ? (u8)1 : (u8)0)
#define LCD_LIGHT_LOW   (LCD_LIGHT_PORT->ODR &= (~LCD_LIGHT_PIN))
#define LCD_LIGHT_HIGH  (LCD_LIGHT_PORT->ODR |= LCD_LIGHT_PIN)
// BTN1, BTN2, BTN3, BTN4 - digital INPUT
#define BTN_PORT           GPIOB
#define BTN_FREQINC_PIN    GPIO_Pin_15
#define BTN_FREQDEC_PIN    GPIO_Pin_14
#define BTN_FREQDUTY_PIN   GPIO_Pin_13
#define BTN_CHGWAVE_PIN    GPIO_Pin_12
#define BTN_FREQINC_STATE  ((u16)(BTN_PORT->IDR & BTN_FREQINC_PIN) != (u16)0 ? (u8)1 : (u8)0)
#define BTN_FREQDEC_STATE  ((u16)(BTN_PORT->IDR & BTN_FREQDEC_PIN) != (u16)0 ? (u8)1 : (u8)0)
#define BTN_FREQDUTY_STATE ((u16)(BTN_PORT->IDR & BTN_FREQDUTY_PIN) != (u16)0 ? (u8)1 : (u8)0)
#define BTN_CHGWAVE_STATE  ((u16)(BTN_PORT->IDR & BTN_CHGWAVE_PIN) != (u16)0 ? (u8)1 : (u8)0)
#define LCD_D7(x) (x==(u8)0 ? (LCD_D7_PORT->ODR &= (~LCD_D7_PIN)) : (LCD_D7_PORT->ODR |= LCD_D7_PIN))
#define LCD_D6(x) (x==(u8)0 ? (LCD_D6_PORT->ODR &= (~LCD_D6_PIN)) : (LCD_D6_PORT->ODR |= LCD_D6_PIN))
#define LCD_D5(x) (x==(u8)0 ? (LCD_D5_PORT->ODR &= (~LCD_D5_PIN)) : (LCD_D5_PORT->ODR |= LCD_D5_PIN))
#define LCD_D4(x) (x==(u8)0 ? (LCD_D4_PORT->ODR &= (~LCD_D4_PIN)) : (LCD_D4_PORT->ODR |= LCD_D4_PIN))
#define LCD_EN(x) (x==(u8)0 ? (LCD_EN_PORT->ODR &= (~LCD_EN_PIN)) : (LCD_EN_PORT->ODR |= LCD_EN_PIN))
#define LCD_RS(x) (x==(u8)0 ? (LCD_RS_PORT->ODR &= (~LCD_RS_PIN)) : (LCD_RS_PORT->ODR |= LCD_RS_PIN))
//-----------end GPIOB------------

#endif