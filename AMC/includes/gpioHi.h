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

#define USER_BTN_PIN          GPIO_Pin_13
#define USER_BTN_PORT         GPIOC

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


#define BRD_ID_BIT0_PIN       GPIO_Pin_7
#define BRD_ID_BIT0_PORT      GPIOA

#define BRD_ID_BIT1_PIN       GPIO_Pin_9
#define BRD_ID_BIT1_PORT      GPIOB

/* solenoid valves */
#define S_DRIVE1_PIN          GPIO_Pin_3
#define S_DRIVE1_PORT         GPIOC

#define S_DRIVE2_PIN          GPIO_Pin_4
#define S_DRIVE2_PORT         GPIOC

#define S_DRIVE3_PIN          GPIO_Pin_5
#define S_DRIVE3_PORT         GPIOC

#define S_DRIVE4_PIN          GPIO_Pin_0
#define S_DRIVE4_PORT         GPIOB

#define S_DRIVE5_PIN          GPIO_Pin_1
#define S_DRIVE5_PORT         GPIOB

#define S_DRIVE6_PIN          GPIO_Pin_2
#define S_DRIVE6_PORT         GPIOB

#define S_DRIVE7_PIN          GPIO_Pin_8
#define S_DRIVE7_PORT         GPIOB

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

/* ac-drives */
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
//#define ADC_1_PIN             GPIO_Pin_0
//#define ADC_1_PORT            GPIOC

//#define ADC_2_PIN             GPIO_Pin_1
//#define ADC_2_PORT            GPIOC
//#define ADC_PORT              GPIOC

/********************* MACROS **************************************/
#define GREEN_LED_ON     GPIO_SetBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define GREEN_LED_OFF    GPIO_ResetBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define GREEN_LED_TOGGLE GPIO_ToggleBits(GREEN_LED_PORT, GREEN_LED_PIN)

#define BLUE_LED_ON      GPIO_SetBits(BLUE_LED_PORT, BLUE_LED_PIN)
#define BLUE_LED_OFF     GPIO_ResetBits(BLUE_LED_PORT, BLUE_LED_PIN)
#define BLUE_LED_TOGGLE  GPIO_ToggleBits(BLUE_LED_PORT, BLUE_LED_PIN)

#define RED_LED_ON       GPIO_SetBits(RED_LED_PORT, RED_LED_PIN)
#define RED_LED_OFF      GPIO_ResetBits(RED_LED_PORT, RED_LED_PIN)
#define RED_LED_TOGGLE   GPIO_ToggleBits(RED_LED_PORT, RED_LED_PIN)

#define BOARD_ID         GPIO_ReadInputDataBit(BRD_ID_BIT0_PORT, BRD_ID_BIT0_PIN) | \
                         (GPIO_ReadInputDataBit(BRD_ID_BIT1_PORT, BRD_ID_BIT1_PIN)<<1)

#define S_DRIVE1_ASSERT  GPIO_SetBits(S_DRIVE1_PORT, S_DRIVE1_PIN)
#define S_DRIVE1_NEGATE  GPIO_ResetBits(S_DRIVE1_PORT, S_DRIVE1_PIN)
#define S_DRIVE2_ASSERT  GPIO_SetBits(S_DRIVE2_PORT, S_DRIVE2_PIN)
#define S_DRIVE2_NEGATE  GPIO_ResetBits(S_DRIVE2_PORT, S_DRIVE2_PIN)
#define S_DRIVE3_ASSERT  GPIO_SetBits(S_DRIVE3_PORT, S_DRIVE3_PIN)
#define S_DRIVE3_NEGATE  GPIO_ResetBits(S_DRIVE3_PORT, S_DRIVE3_PIN)
#define S_DRIVE4_ASSERT  GPIO_SetBits(S_DRIVE4_PORT, S_DRIVE4_PIN)
#define S_DRIVE4_NEGATE  GPIO_ResetBits(S_DRIVE4_PORT, S_DRIVE4_PIN)
#define S_DRIVE5_ASSERT  GPIO_SetBits(S_DRIVE5_PORT, S_DRIVE5_PIN)
#define S_DRIVE5_NEGATE  GPIO_ResetBits(S_DRIVE5_PORT, S_DRIVE5_PIN)
#define S_DRIVE6_ASSERT  GPIO_SetBits(S_DRIVE6_PORT, S_DRIVE6_PIN)
#define S_DRIVE6_NEGATE  GPIO_ResetBits(S_DRIVE6_PORT, S_DRIVE6_PIN)
#define S_DRIVE7_ASSERT  GPIO_SetBits(S_DRIVE7_PORT, S_DRIVE7_PIN)
#define S_DRIVE7_NEGATE  GPIO_ResetBits(S_DRIVE7_PORT, S_DRIVE7_PIN)

#define S_PULSE1_ON      GPIO_SetBits(S_PULSE1_PORT, S_PULSE1_PIN)
#define S_PULSE1_OFF     GPIO_ResetBits(S_PULSE1_PORT, S_PULSE1_PIN)
#define S_PULSE2_ON      GPIO_SetBits(S_PULSE2_PORT, S_PULSE2_PIN)
#define S_PULSE2_OFF     GPIO_ResetBits(S_PULSE2_PORT, S_PULSE2_PIN)

#define S_PULSE3_ON      GPIO_SetBits(S_PULSE3_PORT, S_PULSE3_PIN)
#define S_PULSE3_OFF     GPIO_ResetBits(S_PULSE3_PORT, S_PULSE3_PIN)
#define S_PULSE4_ON      GPIO_SetBits(S_PULSE4_PORT, S_PULSE4_PIN)
#define S_PULSE4_OFF     GPIO_ResetBits(S_PULSE4_PORT, S_PULSE4_PIN)
#define S_PULSE5_ON      GPIO_SetBits(S_PULSE5_PORT, S_PULSE5_PIN)
#define S_PULSE5_OFF     GPIO_ResetBits(S_PULSE5_PORT, S_PULSE5_PIN)
#define S_PULSE6_ON      GPIO_SetBits(S_PULSE6_PORT, S_PULSE6_PIN)
#define S_PULSE6_OFF     GPIO_ResetBits(S_PULSE6_PORT, S_PULSE6_PIN)
#define S_PULSE7_ON      GPIO_SetBits(S_PULSE7_PORT, S_PULSE7_PIN)
#define S_PULSE7_OFF     GPIO_ResetBits(S_PULSE7_PORT, S_PULSE7_PIN)

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

#define AC_DRIVE1_ASSERT         GPIO_SetBits(AC_DRIVE1_PORT, AC_DRIVE1_PIN)
#define AC_DRIVE1_NEGATE         GPIO_ResetBits(AC_DRIVE1_PORT, AC_DRIVE1_PIN)

#define AC_DRIVE2_ASSERT         GPIO_SetBits(AC_DRIVE2_PORT, AC_DRIVE2_PIN)
#define AC_DRIVE2_NEGATE         GPIO_ResetBits(AC_DRIVE2_PORT, AC_DRIVE2_PIN)

#define AC_DRIVE3_ASSERT         GPIO_SetBits(AC_DRIVE3_PORT, AC_DRIVE3_PIN)
#define AC_DRIVE3_NEGATE         GPIO_ResetBits(AC_DRIVE3_PORT, AC_DRIVE3_PIN)

#define AC_DRIVE4_ASSERT         GPIO_SetBits(AC_DRIVE4_PORT, AC_DRIVE4_PIN)
#define AC_DRIVE4_NEGATE         GPIO_ResetBits(AC_DRIVE4_PORT, AC_DRIVE4_PIN)



void Gpio_Init(void);
void GpioSleep(void);

void OpenValve(uint8_t valveNbr);
void CloseValve(uint8_t valveNbr);

void OpenReliefValve(uint8_t valveNbr);
void CloseReliefValve(uint8_t valveNbr);

#endif


