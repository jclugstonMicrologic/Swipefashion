/** H header  ******************************************************************
*
* NAME      sciHi.h
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



#ifndef __NFCOMMON_SCI_HI_H__
#define __NFCOMMON_SCI_HI_H__

#include "types.h"
#include "stm32f4xx.h"
//#include "string.h"

#define UART_MAX_RX_QUE_SIZE (256)

#define COMn    2

#define NBR_PORTS COMn

typedef enum 
{
  USART_1 =0,
  UART_4 =1,  
    
  USART_2,
  USART_3,
  UART_5,
  USART_6,

  LPUART_1

} COMTypeDef;

/* typedef for tx and rx queue structures */
typedef struct
{
   UINT16 tail; /* queue input/write pointer */
   UINT16 full; /* queue full flag */
   UINT16 head; /* queue output/read pointer */
   UINT8 q[UART_MAX_RX_QUE_SIZE]; /* queue array */
} QUE_STRUCT;


/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define SERIAL_COM1                        USART1
#define SERIAL_COM1_CLK                    RCC_APB2Periph_USART1
#define SERIAL_COM1_TX_PIN                 GPIO_Pin_6
#define SERIAL_COM1_TX_GPIO_PORT           GPIOB
#define SERIAL_COM1_TX_GPIO_CLK            RCC_AHB2Periph_GPIOB
#define SERIAL_COM1_TX_SOURCE              GPIO_PinSource6
#define SERIAL_COM1_TX_AF                  GPIO_AF_USART1
#define SERIAL_COM1_RX_PIN                 GPIO_Pin_7
#define SERIAL_COM1_RX_GPIO_PORT           GPIOB
#define SERIAL_COM1_RX_GPIO_CLK            RCC_AHB2Periph_GPIOB
#define SERIAL_COM1_RX_SOURCE              GPIO_PinSource7
#define SERIAL_COM1_RX_AF                  GPIO_AF_USART1
#define SERIAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to UART4
 */ 
#define SERIAL_COM2                        UART4
#define SERIAL_COM2_CLK                    RCC_APB1Periph_UART4
#define SERIAL_COM2_TX_PIN                 GPIO_Pin_10
#define SERIAL_COM2_TX_GPIO_PORT           GPIOC
#define SERIAL_COM2_TX_GPIO_CLK            RCC_AHB2Periph_GPIOC
#define SERIAL_COM2_TX_SOURCE              GPIO_PinSource10
#define SERIAL_COM2_TX_AF                  GPIO_AF_UART4
#define SERIAL_COM2_RX_PIN                 GPIO_Pin_11
#define SERIAL_COM2_RX_GPIO_PORT           GPIOC
#define SERIAL_COM2_RX_GPIO_CLK            RCC_AHB2Periph_GPIOC
#define SERIAL_COM2_RX_SOURCE              GPIO_PinSource11
#define SERIAL_COM2_RX_AF                  GPIO_AF_UART4
#define SERIAL_COM2_IRQn                   UART4_IRQn

#if 0
/**
 * @brief Definition for COM port3, connected to USART2
 */ 
#define SERIAL_COM3                        USART2
#define SERIAL_COM3_CLK                    RCC_APB1Periph_USART2
#define SERIAL_COM3_TX_PIN                 GPIO_Pin_2
#define SERIAL_COM3_TX_GPIO_PORT           GPIOA
#define SERIAL_COM3_TX_GPIO_CLK            RCC_AHB2Periph_GPIOA
#define SERIAL_COM3_TX_SOURCE              GPIO_PinSource2
#define SERIAL_COM3_TX_AF                  GPIO_AF_USART2
#define SERIAL_COM3_RX_PIN                 GPIO_Pin_3
#define SERIAL_COM3_RX_GPIO_PORT           GPIOA
#define SERIAL_COM3_RX_GPIO_CLK            RCC_AHB2Periph_GPIOA
#define SERIAL_COM3_RX_SOURCE              GPIO_PinSource3
#define SERIAL_COM3_RX_AF                  GPIO_AF_USART2
#define SERIAL_COM3_IRQn                   USART2_IRQn

/**
 * @brief Definition for COM port4, connected to USART1
 */ 
#define SERIAL_COM4                        USART1
#define SERIAL_COM4_CLK                    RCC_APB2Periph_USART1
#define SERIAL_COM4_TX_PIN                 GPIO_Pin_6
#define SERIAL_COM4_TX_GPIO_PORT           GPIOB
#define SERIAL_COM4_TX_GPIO_CLK            RCC_AHB2Periph_GPIOB
#define SERIAL_COM4_TX_SOURCE              GPIO_PinSource6
#define SERIAL_COM4_TX_AF                  GPIO_AF_USART1
#define SERIAL_COM4_RX_PIN                 GPIO_Pin_7
#define SERIAL_COM4_RX_GPIO_PORT           GPIOB
#define SERIAL_COM4_RX_GPIO_CLK            RCC_AHB2Periph_GPIOB
#define SERIAL_COM4_RX_SOURCE              GPIO_PinSource7
#define SERIAL_COM4_RX_AF                  GPIO_AF_USART1
#define SERIAL_COM4_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port5, connected to USART4
 */ 
#define SERIAL_COM5                        UART4
#define SERIAL_COM5_CLK                    RCC_APB1Periph_UART4
#define SERIAL_COM5_TX_PIN                 GPIO_Pin_10
#define SERIAL_COM5_TX_GPIO_PORT           GPIOC
#define SERIAL_COM5_TX_GPIO_CLK            RCC_AHB2Periph_GPIOC
#define SERIAL_COM5_TX_SOURCE              GPIO_PinSource10
#define SERIAL_COM5_TX_AF                  GPIO_AF_UART4
#define SERIAL_COM5_RX_PIN                 GPIO_Pin_11
#define SERIAL_COM5_RX_GPIO_PORT           GPIOC
#define SERIAL_COM5_RX_GPIO_CLK            RCC_AHB2Periph_GPIOC
#define SERIAL_COM5_RX_SOURCE              GPIO_PinSource11
#define SERIAL_COM5_RX_AF                  GPIO_AF_UART4
#define SERIAL_COM5_IRQn                   UART4_IRQn

/**
 * @brief Definition for COM port6, connected to LPUART1
 */ 
#define SERIAL_COM6                        LPUART1
#define SERIAL_COM6_CLK                    RCC_APB1ENR2Periph_LPUART1
#define SERIAL_COM6_TX_PIN                 GPIO_Pin_11
#define SERIAL_COM6_TX_GPIO_PORT           GPIOB
#define SERIAL_COM6_TX_GPIO_CLK            RCC_AHB2Periph_GPIOB
#define SERIAL_COM6_TX_SOURCE              GPIO_PinSource11
#define SERIAL_COM6_TX_AF                  GPIO_AF_LPUART1   
#define SERIAL_COM6_RX_PIN                 GPIO_Pin_10
#define SERIAL_COM6_RX_GPIO_PORT           GPIOB
#define SERIAL_COM6_RX_GPIO_CLK            RCC_AHB2Periph_GPIOB
#define SERIAL_COM6_RX_SOURCE              GPIO_PinSource10
#define SERIAL_COM6_RX_AF                  GPIO_AF_LPUART1
#define SERIAL_COM6_IRQn                   LPUART1_IRQn

// Nucleo -F429ZI eval board
/**
 * @brief Definition for COM port1, connected to USART3
 */ 
#define SERIAL_COM1                        USART3
#define SERIAL_COM1_CLK                    RCC_APB1Periph_USART3
#define SERIAL_COM1_TX_PIN                 GPIO_Pin_10
#define SERIAL_COM1_TX_GPIO_PORT           GPIOC
#define SERIAL_COM1_TX_GPIO_CLK            RCC_AHB2Periph_GPIOC
#define SERIAL_COM1_TX_SOURCE              GPIO_PinSource10
#define SERIAL_COM1_TX_AF                  GPIO_AF_USART3
#define SERIAL_COM1_RX_PIN                 GPIO_Pin_11
#define SERIAL_COM1_RX_GPIO_PORT           GPIOC
#define SERIAL_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define SERIAL_COM1_RX_SOURCE              GPIO_PinSource11
#define SERIAL_COM1_RX_AF                  GPIO_AF_USART3
#define SERIAL_COM1_IRQn                   USART3_IRQn

/**
 * @brief Definition for COM port2, connected to USART6
 */ 
#define SERIAL_COM2                        USART6
#define SERIAL_COM2_CLK                    RCC_APB2Periph_USART6
#define SERIAL_COM2_TX_PIN                 GPIO_Pin_6
#define SERIAL_COM2_TX_GPIO_PORT           GPIOC
#define SERIAL_COM2_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define SERIAL_COM2_TX_SOURCE              GPIO_PinSource6
#define SERIAL_COM2_TX_AF                  GPIO_AF_USART6
#define SERIAL_COM2_RX_PIN                 GPIO_Pin_7
#define SERIAL_COM2_RX_GPIO_PORT           GPIOC
#define SERIAL_COM2_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define SERIAL_COM2_RX_SOURCE              GPIO_PinSource7
#define SERIAL_COM2_RX_AF                  GPIO_AF_USART6
#define SERIAL_COM2_IRQn                   USART6_IRQn

/**
 * @brief Definition for COM port3, connected to USART2
 */ 
#define SERIAL_COM3                        USART2
#define SERIAL_COM3_CLK                    RCC_APB1Periph_USART2
#define SERIAL_COM3_TX_PIN                 GPIO_Pin_5
#define SERIAL_COM3_TX_GPIO_PORT           GPIOD
#define SERIAL_COM3_TX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define SERIAL_COM3_TX_SOURCE              GPIO_PinSource5
#define SERIAL_COM3_TX_AF                  GPIO_AF_USART2
#define SERIAL_COM3_RX_PIN                 GPIO_Pin_6
#define SERIAL_COM3_RX_GPIO_PORT           GPIOD
#define SERIAL_COM3_RX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define SERIAL_COM3_RX_SOURCE              GPIO_PinSource6
#define SERIAL_COM3_RX_AF                  GPIO_AF_USART2
#define SERIAL_COM3_IRQn                   USART2_IRQn

/**
 * @brief Definition for COM port4, connected to USART1
 */ 
#define SERIAL_COM4                        USART1
#define SERIAL_COM4_CLK                    RCC_APB2Periph_USART1
#define SERIAL_COM4_TX_PIN                 GPIO_Pin_9
#define SERIAL_COM4_TX_GPIO_PORT           GPIOA
#define SERIAL_COM4_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define SERIAL_COM4_TX_SOURCE              GPIO_PinSource9
#define SERIAL_COM4_TX_AF                  GPIO_AF_USART1
#define SERIAL_COM4_RX_PIN                 GPIO_Pin_10
#define SERIAL_COM4_RX_GPIO_PORT           GPIOA
#define SERIAL_COM4_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define SERIAL_COM4_RX_SOURCE              GPIO_PinSource10
#define SERIAL_COM4_RX_AF                  GPIO_AF_USART1
#define SERIAL_COM4_IRQn                   USART1_IRQn
#endif


void SciSerialPortInit(COMTypeDef port, UINT32 baudRate,UINT8 parity);
void SciSetPort(UINT16 port);
UINT16 SciGetByte(char uart, char *rxBytePtr);
extern void SciSendByte(char txByte,char sciPort);
UINT8 SciGetString(char sciPort,char *pString);

void SciSocketInit(void);
int SciSocketCheckQue(void);
int SciSocketRecv( unsigned char *buf, int count );

#endif


