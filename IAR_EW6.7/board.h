#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32f0xx.h"

//---------LCD Pins----------
// LCD RS - digital OUTPUT
#define LCD_RS_PORT  GPIOA 
#define LCD_RS_PIN   GPIO_Pin_6
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
// LCD D7 - digital OUTPUT
#define LCD_D7_PORT  GPIOA 
#define LCD_D7_PIN   GPIO_Pin_7
// LCD BACKLIGHT - digital OUTPUT
#define LCD_LIGHT_PORT  GPIOB 
#define LCD_LIGHT_PIN   GPIO_Pin_11
#define LCD_LIGHT_STATE ((u16)(LCD_LIGHT_PORT->IDR & LCD_LIGHT_PIN) != (u16)0 ? (u8)1 : (u8)0)
#define LCD_LIGHT_LOW   (LCD_LIGHT_PORT->ODR &= (~LCD_LIGHT_PIN))
#define LCD_LIGHT_HIGH  (LCD_LIGHT_PORT->ODR |= LCD_LIGHT_PIN)
#define LCD_D7(x) (x==(u8)0 ? (LCD_D7_PORT->ODR &= (~LCD_D7_PIN)) : (LCD_D7_PORT->ODR |= LCD_D7_PIN))
#define LCD_D6(x) (x==(u8)0 ? (LCD_D6_PORT->ODR &= (~LCD_D6_PIN)) : (LCD_D6_PORT->ODR |= LCD_D6_PIN))
#define LCD_D5(x) (x==(u8)0 ? (LCD_D5_PORT->ODR &= (~LCD_D5_PIN)) : (LCD_D5_PORT->ODR |= LCD_D5_PIN))
#define LCD_D4(x) (x==(u8)0 ? (LCD_D4_PORT->ODR &= (~LCD_D4_PIN)) : (LCD_D4_PORT->ODR |= LCD_D4_PIN))
#define LCD_EN(x) (x==(u8)0 ? (LCD_EN_PORT->ODR &= (~LCD_EN_PIN)) : (LCD_EN_PORT->ODR |= LCD_EN_PIN))
#define LCD_RS(x) (x==(u8)0 ? (LCD_RS_PORT->ODR &= (~LCD_RS_PIN)) : (LCD_RS_PORT->ODR |= LCD_RS_PIN))

//---------RS232 Pins---------
// USART - digital I/O
#define USART_PORT    GPIOA
#define USART_TX_PIN  GPIO_Pin_9
#define USART_RX_PIN  GPIO_Pin_10

//---------DAC Output Pin-----
// DAC OUT - analog OUTPUT
#define DAC_PORT  GPIOA
#define DAC_PIN   GPIO_Pin_4

//---------Analog Input Pins----
// POTENTIOMETER - analog INPUT
//#define VOLTAGE_PORT  GPIOA
//#define VOLTAGE_PIN   GPIO_Pin_3
// CURRENT - analog INPUT
//#define CURRENT_PORT        GPIOA
//#define CURRENT_PIN         GPIO_Pin_5

// ------ PWM out pin ----------
#define PWM_OUT_PORT  GPIOA
#define PWM_OUT_PIN   GPIO_Pin_3

// ------ I2C pins ----------
#define I2C_PORT      GPIOF
#define I2C_SDA_PIN   GPIO_Pin_7
#define I2C_SCL_PIN   GPIO_Pin_6

//---------Button Input Pins-------------
// BTN1, BTN2, BTN3, BTN4 - digital INPUT
#define BTN_PORT       GPIOB
#define BTN_INC_PIN    GPIO_Pin_13
#define BTN_DEC_PIN    GPIO_Pin_15
#define BTN_MODE_PIN   GPIO_Pin_14
#define BTN_FREQINC_STATE  ((u16)(BTN_PORT->IDR & BTN_INC_PIN)  != (u16)0 ? (u8)1 : (u8)0)
#define BTN_FREQDEC_STATE  ((u16)(BTN_PORT->IDR & BTN_DEC_PIN)  != (u16)0 ? (u8)1 : (u8)0)
#define BTN_FREQDUTY_STATE ((u16)(BTN_PORT->IDR & BTN_MODE_PIN) != (u16)0 ? (u8)1 : (u8)0)

// Debug Pin
//#define DEBUGPIN_PORT   GPIOA
//#define DEBUGPIN_PIN    GPIO_Pin_8
//#define DEBUGPIN_LOW    (DEBUGPIN_PORT->ODR &= (~DEBUGPIN_PIN))
//#define DEBUGPIN_HIGH   (DEBUGPIN_PORT->ODR |= DEBUGPIN_PIN)
//#define DEBUGPIN_TOGGLE ((DEBUGPIN_PORT->ODR & DEBUGPIN_PIN) ? DEBUGPIN_LOW : DEBUGPIN_HIGH )

#endif