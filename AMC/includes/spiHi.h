/** H header  ******************************************************************
*
* NAME      spiHi.h
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

#ifndef SPI_HI
#define SPI_HI

#include "types.h"
#include "stm32f4xx.h"

#define SPIn    2

/* SPIx Communication boards Interface */

#define SPI1_PORT                      SPI1
#define SPI1_CLK                       RCC_APB2Periph_SPI1
#define SPI1_CLK_INIT                  RCC_APB2PeriphClockCmd //RCC_APB1PeriphClockCmd
#define SPI1_IRQn                      SPI1_IRQn
#define SPI1_IRQHANDLER                SPI1_IRQHandler

/*
#define SPI0_NSS_PIN                   GPIO_Pin_11
#define SPI0_NSS_GPIO_PORT             GPIOA
#define SPI0_NSS_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define SPI0_NSS_SOURCE                GPIO_PinSource4
#define SPI0_NSS_AF                    GPIO_AF_SPI4
*/

#define SPI1_SCK_PIN                   GPIO_Pin_13
#define SPI1_SCK_GPIO_PORT             GPIOE
#define SPI1_SCK_GPIO_CLK              RCC_AHB2Periph_GPIOE
#define SPI1_SCK_SOURCE                GPIO_PinSource13
#define SPI1_SCK_AF                    GPIO_AF_SPI1

#define SPI1_MISO_PIN                  GPIO_Pin_14
#define SPI1_MISO_GPIO_PORT            GPIOE
#define SPI1_MISO_GPIO_CLK             RCC_AHB2Periph_GPIOE
#define SPI1_MISO_SOURCE               GPIO_PinSource14
#define SPI1_MISO_AF                   GPIO_AF_SPI1

#define SPI1_MOSI_PIN                  GPIO_Pin_15
#define SPI1_MOSI_GPIO_PORT            GPIOE
#define SPI1_MOSI_GPIO_CLK             RCC_AHB2Periph_GPIOE
#define SPI1_MOSI_SOURCE               GPIO_PinSource15
#define SPI1_MOSI_AF                   GPIO_AF_SPI1


#define SPI2_PORT                      SPI2
#define SPI2_CLK                       RCC_APB1Periph_SPI2
#define SPI2_CLK_INIT                  RCC_APB1PeriphClockCmd //RCC_APB1PeriphClockCmd
#define SPI2_IRQn                      SPI2_IRQn
#define SPI2_IRQHANDLER                SPI2_IRQHandler

/*
#define SPI3_NSS_PIN                   GPIO_Pin_4
#define SPI3_NSS_GPIO_PORT             GPIOA
#define SPI3_NSS_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define SPI3_NSS_SOURCE                GPIO_PinSource4
#define SPI3_NSS_AF                    GPIO_AF_SPI1
*/

#define SPI2_SCK_PIN                   GPIO_Pin_13
#define SPI2_SCK_GPIO_PORT             GPIOB
#define SPI2_SCK_GPIO_CLK              RCC_AHB2Periph_GPIOB
#define SPI2_SCK_SOURCE                GPIO_PinSource13
#define SPI2_SCK_AF                    GPIO_AF_SPI2

#define SPI2_MISO_PIN                  GPIO_Pin_14
#define SPI2_MISO_GPIO_PORT            GPIOB
#define SPI2_MISO_GPIO_CLK             RCC_AHB2Periph_GPIOB
#define SPI2_MISO_SOURCE               GPIO_PinSource14
#define SPI2_MISO_AF                   GPIO_AF_SPI2

#define SPI2_MOSI_PIN                  GPIO_Pin_15
#define SPI2_MOSI_GPIO_PORT            GPIOB
#define SPI2_MOSI_GPIO_CLK             RCC_AHB2Periph_GPIOB
#define SPI2_MOSI_SOURCE               GPIO_PinSource15
#define SPI2_MOSI_AF                   GPIO_AF_SPI2

#define SPI_DUMMY_BYTE                 (0x00)

#define ASSERT_SPI_PERIPH1_CS   LORA_ASSERT_CS
#define NEGATE_SPI_PERIPH1_CS   LORA_NEGATE_CS

#define ASSERT_SPI_PERIPH2_CS   FLASH_ASSERT_M1_CS
#define NEGATE_SPI_PERIPH2_CS   FLASH_NEGATE_M1_CS

#define ASSERT_SPI_PERIPH3_CS   FLASH_ASSERT_M2_CS
#define NEGATE_SPI_PERIPH3_CS   FLASH_NEGATE_M2_CS

#define ASSERT_SPI_PERIPH4_CS   ACCEL_ASSERT_CS
#define NEGATE_SPI_PERIPH4_CS   ACCEL_NEGATE_CS

typedef enum
{
    SPI_PORT1  =0,
    SPI_PORT2,
    SPI_PORT3,
    SPI_PORT4,
    
    SPI_PORT_END
}SpiPortEnum;


typedef enum
{
    SPI_PERIPHERAL1 =0,
    SPI_PERIPHERAL2,
    SPI_PERIPHERAL3,    
    SPI_PERIPHERAL4,
    
    SPI_PERIPHERAL_END
      
}SpiPeripheralEnum;

typedef struct
{
    UINT8 txBuf[10];
//    Int8U rxBuf[10];
    SpiPeripheralEnum peripheral;
    UINT16 nbrBytes;    
    UINT8 port;
      
}SPI_DATA_STRUCT;


void SpiInit(SpiPortEnum port);
void SpiDeviceInit(SpiPeripheralEnum periph);

void spiStart(SpiPeripheralEnum periph);
void spiStop(SpiPeripheralEnum periph);

UINT8 SpiTransferSpi1(UINT8 txByte);
UINT8 SpiTransferSpi2(UINT8 txByte);
  
#endif








