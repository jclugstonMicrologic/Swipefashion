/** H Header  ******************************************************************
*
* NAME      gpio.h
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v8.20.2
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2020, MICROLOGIC
* Calgary, Alberta, Canada, www.micrologic.ab.ca
*******************************************************************************/


#ifndef GPIO_H
#define GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_gpio.h"

#define TP204_PIN             GPIO_Pin_7
#define TP204_PORT            GPIOA

//#define TP206_PIN             GPIO_Pin_8
//#define TP206_PORT            GPIOC

#define TP208_PIN             GPIO_Pin_8
#define TP208_PORT            GPIOE

//#define TP209_PIN             GPIO_Pin_9
//#define TP209_PORT            GPIOE


//#define POWER_5V_ENABLE_PIN   GPIO_Pin_8
//#define POWER_5V_ENABLE_PORT  GPIOB

#define GREEN_LED_PIN         TP204_PIN //TP204
#define GREEN_LED_PORT        TP204_PORT

#define RED_LED_PIN           TP208_PIN //TP208
#define RED_LED_PORT          TP208_PORT

//#define TST_PIN               TP209_PIN //TP209
//#define TST_PORT              TP209_PORT


/* solenoid valves */
#define S_VALVE1_PIN          GPIO_Pin_3
#define S_VALVE1_PORT         GPIOC

#define S_VALVE2_PIN          GPIO_Pin_4
#define S_VALVE2_PORT         GPIOC

#define S_VALVE3_PIN          GPIO_Pin_5
#define S_VALVE3_PORT         GPIOC

#define S_VALVE4_PIN          GPIO_Pin_0
#define S_VALVE4_PORT         GPIOB

#define S_VALVE5_PIN          GPIO_Pin_1
#define S_VALVE5_PORT         GPIOB

#define S_VALVE6_PIN          GPIO_Pin_2
#define S_VALVE6_PORT         GPIOB

#define S_VALVE7_PIN          GPIO_Pin_3
#define S_VALVE7_PORT         GPIOB

/* solenoid pulse */
#define S_PULSE1_PIN          GPIO_Pin_0
#define S_PULSE1_PORT         GPIOA

#define S_PULSE2_PIN          GPIO_Pin_1
#define S_PULSE2_PORT         GPIOA

#define S_PULSE3_PIN          GPIO_Pin_2
#define S_PULSE3_PORT         GPIOA

#define S_PULSE4_PIN          GPIO_Pin_3
#define S_PULSE4_PORT         GPIOA

#define S_PULSE5_PIN          GPIO_Pin_4
#define S_PULSE5_PORT         GPIOA

#define S_PULSE6_PIN          GPIO_Pin_5
#define S_PULSE6_PORT         GPIOA

#define S_PULSE7_PIN          GPIO_Pin_6
#define S_PULSE7_PORT         GPIOA

/* ac-drives1 */
#define AC_DRIVE1_PIN          GPIO_Pin_0
#define AC_DRIVE1_PORT         GPIOC
#define AC_DRIVE2_PIN          GPIO_Pin_1
#define AC_DRIVE2_PORT         GPIOC
#define AC_DRIVE3_PIN          GPIO_Pin_2
#define AC_DRIVE3_PORT         GPIOC
#define AC_DRIVE4_PIN          GPIO_Pin_10
#define AC_DRIVE4_PORT         GPIOB


#define PRESS_SENSOR1_CS_PIN   GPIO_Pin_11
#define PRESS_SENSOR1_CS_PORT  GPIOB
#define PRESS_SENSOR2_CS_PIN   GPIO_Pin_12
#define PRESS_SENSOR2_CS_PORT  GPIOB
#define PRESS_SENSOR3_CS_PIN   GPIO_Pin_6
#define PRESS_SENSOR3_CS_PORT  GPIOC
#define PRESS_SENSOR4_CS_PIN   GPIO_Pin_7
#define PRESS_SENSOR4_CS_PORT  GPIOC
#define PRESS_SENSOR5_CS_PIN   GPIO_Pin_8
#define PRESS_SENSOR5_CS_PORT  GPIOC
#define PRESS_SENSOR6_CS_PIN   GPIO_Pin_9
#define PRESS_SENSOR6_CS_PORT  GPIOC
#define PRESS_SENSOR7_CS_PIN   GPIO_Pin_13
#define PRESS_SENSOR7_CS_PORT  GPIOC
#define PRESS_SENSOR8_CS_PIN   GPIO_Pin_8
#define PRESS_SENSOR8_CS_PORT  GPIOA


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
#define S_VALVE2_OPEN    GPIO_SetBits(S_VALVE2_PORT, S_VALVE2_PIN)
#define S_VALVE2_CLOSE   GPIO_ResetBits(S_VALVE2_PORT, S_VALVE2_PIN)
#define S_VALVE3_OPEN    GPIO_SetBits(S_VALVE3_PORT, S_VALVE3_PIN)
#define S_VALVE3_CLOSE   GPIO_ResetBits(S_VALVE3_PORT, S_VALVE3_PIN)
#define S_VALVE4_OPEN    GPIO_SetBits(S_VALVE4_PORT, S_VALVE4_PIN)
#define S_VALVE4_CLOSE   GPIO_ResetBits(S_VALVE4_PORT, S_VALVE4_PIN)
#define S_VALVE5_OPEN    GPIO_SetBits(S_VALVE5_PORT, S_VALVE5_PIN)
#define S_VALVE5_CLOSE   GPIO_ResetBits(S_VALVE5_PORT, S_VALVE5_PIN)
#define S_VALVE6_OPEN    GPIO_SetBits(S_VALVE6_PORT, S_VALVE6_PIN)
#define S_VALVE6_CLOSE   GPIO_ResetBits(S_VALVE6_PORT, S_VALVE6_PIN)
#define S_VALVE7_OPEN    GPIO_SetBits(S_VALVE7_PORT, S_VALVE7_PIN)
#define S_VALVE7_CLOSE   GPIO_ResetBits(S_VALVE7_PORT, S_VALVE7_PIN)

#define TST_PIN_ON       GPIO_SetBits(TST_PORT, TST_PIN)
#define TST_PIN_OFF      GPIO_ResetBits(TST_PORT, TST_PIN)
#define TST_PIN_TOGGLE   GPIO_ToggleBits(TST_PORT, TST_PIN)
 
#define POWER_5V_ON           GPIO_SetBits(POWER_5V_ENABLE_PORT, POWER_5V_ENABLE_PIN)
#define POWER_5V_OFF          GPIO_ResetBits(POWER_5V_ENABLE_PORT, POWER_5V_ENABLE_PIN)
#define POWER_5V_TOGGLE       GPIO_ToggleBits(POWER_5V_ENABLE_PORT, POWER_5V_ENABLE_PIN)

#define PRESS_SENSOR1_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR1_CS_PORT, PRESS_SENSOR1_CS_PIN)
#define PRESS_SENSOR1_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR1_CS_PORT, PRESS_SENSOR1_CS_PIN)
#define PRESS_SENSOR2_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR2_CS_PORT, PRESS_SENSOR2_CS_PIN)
#define PRESS_SENSOR2_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR2_CS_PORT, PRESS_SENSOR2_CS_PIN)

#define PRESS_SENSOR3_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR3_CS_PORT, PRESS_SENSOR3_CS_PIN)
#define PRESS_SENSOR3_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR3_CS_PORT, PRESS_SENSOR3_CS_PIN)
#define PRESS_SENSOR4_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR4_CS_PORT, PRESS_SENSOR4_CS_PIN)
#define PRESS_SENSOR4_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR4_CS_PORT, PRESS_SENSOR4_CS_PIN)

#define PRESS_SENSOR5_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR5_CS_PORT, PRESS_SENSOR5_CS_PIN)
#define PRESS_SENSOR5_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR5_CS_PORT, PRESS_SENSOR5_CS_PIN)
#define PRESS_SENSOR6_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR6_CS_PORT, PRESS_SENSOR6_CS_PIN)
#define PRESS_SENSOR6_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR6_CS_PORT, PRESS_SENSOR6_CS_PIN)

#define PRESS_SENSOR7_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR7_CS_PORT, PRESS_SENSOR7_CS_PIN)
#define PRESS_SENSOR7_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR7_CS_PORT, PRESS_SENSOR7_CS_PIN)
#define PRESS_SENSOR8_ASSERT_CS  GPIO_ResetBits(PRESS_SENSOR8_CS_PORT, PRESS_SENSOR8_CS_PIN)
#define PRESS_SENSOR8_NEGATE_CS  GPIO_SetBits(PRESS_SENSOR8_CS_PORT, PRESS_SENSOR8_CS_PIN)

void Gpio_Init(void);
void GpioSleep(void);

void OpenValve(uint8_t valveNbr);
void CloseValve(uint8_t valveNbr);

#endif


