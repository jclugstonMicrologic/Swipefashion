/** H Header  ******************************************************************
*
* NAME      gpio.h
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


#ifndef GPIO_H
#define GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_gpio.h"

#define TP204_PIN             GPIO_Pin_7
#define TP204_PORT            GPIOA

#define TP206_PIN             GPIO_Pin_8
#define TP206_PORT            GPIOC

#define TP208_PIN             GPIO_Pin_8
#define TP208_PORT            GPIOE

#define TP209_PIN             GPIO_Pin_9
#define TP209_PORT            GPIOE

#define POWER_5V_ENABLE_PIN   GPIO_Pin_8
#define POWER_5V_ENABLE_PORT  GPIOB

#define GREEN_LED_PIN         TP204_PIN //TP204
#define GREEN_LED_PORT        TP204_PORT

#define RED_LED_PIN           TP208_PIN //TP208
#define RED_LED_PORT          TP208_PORT

#define TST_PIN               TP209_PIN //TP209
#define TST_PORT              TP209_PORT

#define S_VALVE1_PIN          GPIO_Pin_1
#define S_VALVE1_PORT         GPIOA

#define S_VALVE2_PIN          GPIO_Pin_2
#define S_VALVE2_PORT         GPIOA

#define S_VALVE3_PIN          GPIO_Pin_3
#define S_VALVE3_PORT         GPIOA

#define S_VALVE4_PIN          GPIO_Pin_4
#define S_VALVE4_PORT         GPIOA

#define S_VALVE5_PIN          GPIO_Pin_5
#define S_VALVE5_PORT         GPIOA

#define S_VALVE6_PIN          GPIO_Pin_6
#define S_VALVE6_PORT         GPIOA

#define S_VALVE7_PIN          GPIO_Pin_7
#define S_VALVE7_PORT         GPIOA


#define WAKEUP_BTN_PIN        GPIO_Pin_6
#define WAKEUP_BTN_PORT       GPIOE

/* ADC channels */
#define ADC_1_PIN             GPIO_Pin_0
#define ADC_1_PORT            GPIOC

#define ADC_2_PIN             GPIO_Pin_1
#define ADC_2_PORT            GPIOC

#define ADC_VSENSE1_PIN       GPIO_Pin_2
#define ADC_VSENSE1_PORT      GPIOC

#define ADC_VSENSE2_PIN       GPIO_Pin_3
#define ADC_VSENSE2_PORT      GPIOC

#define ADC_PORT              GPIOC

#define FLASH_M1_CS_PIN       GPIO_Pin_12
#define FLASH_M1_CS_PORT      GPIOB

#define FLASH_M2_CS_PIN       GPIO_Pin_10
#define FLASH_M2_CS_PORT      GPIOD

/************** MACROS *****************/
#define GREEN_LED_ON     GPIO_SetBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define GREEN_LED_OFF    GPIO_ResetBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define GREEN_LED_TOGGLE GPIO_ToggleBits(GREEN_LED_PORT, GREEN_LED_PIN)

#define BLUE_LED_ON      GPIO_SetBits(BLUE_LED_PORT, BLUE_LED_PIN)
#define BLUE_LED_OFF     GPIO_ResetBits(BLUE_LED_PORT, BLUE_LED_PIN)
#define BLUE_LED_TOGGLE  GPIO_ToggleBits(BLUE_LED_PORT, BLUE_LED_PIN)

#define RED_LED_ON       GPIO_SetBits(RED_LED_PORT, RED_LED_PIN)
#define RED_LED_OFF      GPIO_ResetBits(RED_LED_PORT, RED_LED_PIN)
#define RED_LED_TOGGLE   GPIO_ToggleBits(RED_LED_PORT, RED_LED_PIN)

#define S_VALVE1_OPEN    GPIO_SetBits(S_VALVE1_PORT, S_VALVE1_PIN)
#define S_VALVE1_CLOSE   GPIO_ResetBits(S_VALVE1_PORT, S_VALVE1_PIN)

#define TST_PIN_ON       GPIO_SetBits(TST_PORT, TST_PIN)
#define TST_PIN_OFF      GPIO_ResetBits(TST_PORT, TST_PIN)
#define TST_PIN_TOGGLE   GPIO_ToggleBits(TST_PORT, TST_PIN)
 
#define FLASH_ASSERT_M1_CS    GPIO_ResetBits(FLASH_M1_CS_PORT, FLASH_M1_CS_PIN)
#define FLASH_NEGATE_M1_CS    GPIO_SetBits(FLASH_M1_CS_PORT, FLASH_M1_CS_PIN)

#define FLASH_ASSERT_M2_CS    GPIO_ResetBits(FLASH_M2_CS_PORT, FLASH_M2_CS_PIN)
#define FLASH_NEGATE_M2_CS    GPIO_SetBits(FLASH_M2_CS_PORT, FLASH_M2_CS_PIN)

#define POWER_5V_ON           GPIO_SetBits(POWER_5V_ENABLE_PORT, POWER_5V_ENABLE_PIN)
#define POWER_5V_OFF          GPIO_ResetBits(POWER_5V_ENABLE_PORT, POWER_5V_ENABLE_PIN)
#define POWER_5V_TOGGLE       GPIO_ToggleBits(POWER_5V_ENABLE_PORT, POWER_5V_ENABLE_PIN)

void GpioInit(void);
void GpioSleep(void);

#endif


