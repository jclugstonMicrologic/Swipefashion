/** C Source ******************************************************************
*
* NAME      spiHi.c
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

#include "spiHi.h"
#include "gpioHi.h"

#include "sysTimers.h"

//void SpiDelay(UINT16);
const uint32_t  SPI_CLK[SPIn]           ={SPI1_CLK, SPI2_CLK};   
//const uint32_t  SPI_CLK_INIT[SPIn]  ={SPI1_CLK_INIT, SPI3_CLK_INIT}; 

//const uint32_t  SPI_NSS_PIN[SPIn]       ={SPI0_NSS_PIN,         SPI3_NSS_PIN}; 
//GPIO_TypeDef*   SPI_NSS_GPIO_PORT[SPIn] ={SPI0_NSS_GPIO_PORT,   SPI3_NSS_GPIO_PORT}; 

const uint32_t  SPI_SCK_PIN[]           ={SPI1_SCK_PIN,         SPI2_SCK_PIN};   
GPIO_TypeDef*   SPI_SCK_GPIO_PORT[]     ={SPI1_SCK_GPIO_PORT,   SPI2_SCK_GPIO_PORT}; 
const uint32_t  SPI_SCK_GPIO_CLK[]      ={SPI1_SCK_GPIO_CLK,    SPI2_SCK_GPIO_CLK}; 
const uint32_t  SPI_SCK_SOURCE[]        ={SPI1_SCK_SOURCE,      SPI2_SCK_SOURCE}; 
const uint32_t  SPI_SCK_AF[]            ={SPI1_SCK_AF,          SPI2_SCK_AF}; 

const uint32_t  SPI_MISO_PIN[]          ={SPI1_MISO_PIN,        SPI2_MISO_PIN};   
GPIO_TypeDef*   SPI_MISO_GPIO_PORT[]    ={SPI1_MISO_GPIO_PORT,  SPI2_MISO_GPIO_PORT}; 
const uint32_t  SPI_MISO_GPIO_CLK[]     ={SPI1_MISO_GPIO_CLK,   SPI2_MISO_GPIO_CLK}; 
const uint32_t  SPI_MISO_SOURCE[]       ={SPI1_MISO_SOURCE,     SPI2_MISO_SOURCE}; 
const uint32_t  SPI_MISO_AF[]           ={SPI1_MISO_AF,         SPI2_MISO_AF}; 

const uint32_t  SPI_MOSI_PIN[]          ={SPI1_MOSI_PIN,        SPI2_MOSI_PIN};   
GPIO_TypeDef*   SPI_MOSI_GPIO_PORT[]    ={SPI1_MOSI_GPIO_PORT,  SPI2_MOSI_GPIO_PORT}; 
const uint32_t  SPI_MOSI_GPIO_CLK[]     ={SPI1_MOSI_GPIO_CLK,   SPI2_MOSI_GPIO_CLK}; 
const uint32_t  SPI_MOSI_SOURCE[]       ={SPI1_MOSI_SOURCE,     SPI2_MOSI_SOURCE}; 
const uint32_t  SPI_MOSI_AF[]           ={SPI1_MOSI_AF,         SPI2_MOSI_AF}; 

               
/*
*|----------------------------------------------------------------------------
*|  Routine: SpiInit
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void SpiInit(SpiPortEnum port)
{
    /* SPI configuration -------------------------------------------------------*/  
    //SPI_InitTypeDef  SPI_InitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;    

    /* Peripheral Clock Enable -------------------------------------------------*/
    /* Enable the SPI clock */
    //SPI1_CLK_INIT(SPI1_CLK, ENABLE);
  
    if( port == SPI_PORT1 )
    {
        /* APB2 is used for USART1 & USART6 */
        RCC_APB2PeriphClockCmd(SPI_CLK[port], ENABLE);        
    }
    else
    {
        /* APB1 is used for USART2, USART3, UART4 & UART5 */
        RCC_APB1PeriphClockCmd(SPI_CLK[port], ENABLE);        
    }
 
    /* Enable GPIO clocks */
    RCC_AHB2PeriphClockCmd(SPI_SCK_GPIO_CLK[port] | SPI_MISO_GPIO_CLK[port] | SPI_MOSI_GPIO_CLK[port], ENABLE );

    /* SPI GPIO Configuration --------------------------------------------------*/
    /* Connect SPI pins to AF5 */  
    GPIO_PinAFConfig(SPI_SCK_GPIO_PORT[port], SPI_SCK_SOURCE[port], SPI_SCK_AF[port]);
    GPIO_PinAFConfig(SPI_MISO_GPIO_PORT[port], SPI_MISO_SOURCE[port], SPI_MISO_AF[port]);
    GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT[port], SPI_MOSI_SOURCE[port], SPI_MOSI_AF[port]);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

    /* SPI SCK pin configuration */
    GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN[port] | SPI_MISO_PIN[port] | SPI_MOSI_PIN[port];
    GPIO_Init(SPI_SCK_GPIO_PORT[port], &GPIO_InitStructure);

#if 0    
    /* Configure the Priority Group to 1 bit */                
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //NVIC_PriorityGroup_2);
  
    /* Configure the SPI interrupt priority */
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: SpiDeviceInit
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void SpiDeviceInit(UINT8 periph)
{
    SPI_InitTypeDef  SPI_InitStructure;

    SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_TIMode  = 0;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_CRCCalculation = 0; /* disabled */
    SPI_InitStructure.SPI_CRCLength =1;
    SPI_InitStructure.SPI_NSSPMode  =0;
  
    /* initializes the SPI communication */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
            
    switch( periph )
    {
        case SPI_PERIPHERAL1:
        case SPI_PERIPHERAL2:
        case SPI_PERIPHERAL3:
        case SPI_PERIPHERAL4:
        case SPI_PERIPHERAL5:
        case SPI_PERIPHERAL6:
        case SPI_PERIPHERAL7:
        case SPI_PERIPHERAL8:
            SPI_I2S_DeInit(SPI2_PORT);
            
            /* SPI Mode 3 */
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
            SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;
            SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
            
            SPI_Init(SPI2_PORT, &SPI_InitStructure);
            
             /* enable the SPI peripheral */
            SPI_Cmd(SPI2_PORT, ENABLE);
            break;
        default:
            SPI_I2S_DeInit(SPI2_PORT);
                            
            /* SPI Mode 3 */
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
            SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
            SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_2;
            
            SPI_Init(SPI2_PORT, &SPI_InitStructure);
                         
            /* enable the SPI peripheral */
            SPI_Cmd(SPI2_PORT, ENABLE);
            break;                   
    }  
}

/*
*|----------------------------------------------------------------------------
*|  Module: spiHi
*|  Routine:  SpiStart
*|  Description:
*|----------------------------------------------------------------------------
*/
void spiStart(UINT8 periph)
{            
    switch(periph)
    {
        case SPI_PERIPHERAL1:
            /* assert cs */
            ASSERT_SPI_PERIPH1_CS;
            break;              
        case SPI_PERIPHERAL2:
            /* assert cs */
            ASSERT_SPI_PERIPH2_CS;
            break;        
        case SPI_PERIPHERAL3:
            /* assert cs */
            /* assert cs */
            ASSERT_SPI_PERIPH3_CS;          
            break;
        case SPI_PERIPHERAL4:
            /* assert cs */
            ASSERT_SPI_PERIPH4_CS;
            break;  
        case SPI_PERIPHERAL5:
            /* assert cs */
            ASSERT_SPI_PERIPH5_CS;
            break;        
        case SPI_PERIPHERAL6:
            /* assert cs */
            ASSERT_SPI_PERIPH6_CS;
            break;
        case SPI_PERIPHERAL7:
            /* assert cs */
            ASSERT_SPI_PERIPH7_CS;
            break;            
        case SPI_PERIPHERAL8:
            /* assert cs */
            ASSERT_SPI_PERIPH8_CS;
            break;             
    }
    
    TimerDelayUs(10);
}


/*
*|----------------------------------------------------------------------------
*|  Module: spiHi
*|  Routine:  spiStop
*|  Description:
*|----------------------------------------------------------------------------
*/
void spiStop(UINT8 periph)

{       
    switch(periph)
    {
        case SPI_PERIPHERAL1:
            /* negate cs */
            NEGATE_SPI_PERIPH1_CS;
            break;                    
        case SPI_PERIPHERAL2:
            /* negate cs */
            NEGATE_SPI_PERIPH2_CS;
            break;        
        case SPI_PERIPHERAL3:
            /* neagte cs */
            NEGATE_SPI_PERIPH3_CS;
            break;
        case SPI_PERIPHERAL4:
            /* neagte cs */
            NEGATE_SPI_PERIPH4_CS;
            break;            
        case SPI_PERIPHERAL5:
            /* negate cs */
            NEGATE_SPI_PERIPH5_CS;
            break;        
        case SPI_PERIPHERAL6:
            /* neagte cs */
            NEGATE_SPI_PERIPH6_CS;
            break;
        case SPI_PERIPHERAL7:
            /* neagte cs */
            NEGATE_SPI_PERIPH7_CS;
            break;                        
        case SPI_PERIPHERAL8:
            /* neagte cs */
            NEGATE_SPI_PERIPH8_CS;
            break;                        
    }
    
    TimerDelayUs(10);  
}

/*
*|----------------------------------------------------------------------------
*|  Module: spiHi
*|  Routine:  SpiTransferSpi1
*|  Description:
*|----------------------------------------------------------------------------
*/
UINT8 SpiTransferSpi1
(
    UINT8 txByte
)
{           
    UINT8 aByte =0;
    
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET ) {}   /* Wait for DR register to be empty */                
    SPI_I2S_SendData( SPI1, txByte );  

    /* if we are expecting data, get it */
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) == RESET ) {}              
    
    /* must always read SPIx->DR */
    aByte =SPI_I2S_ReceiveData(SPI1);
        
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_BSY ) == SET ) {}    
    
    return aByte;
}

/*
*|----------------------------------------------------------------------------
*|  Module: spiHi
*|  Routine:  SpiTransferSpi2
*|  Description:
*|----------------------------------------------------------------------------
*/
UINT8 SpiTransferSpi2
(
    UINT8 txByte
)
{   
    UINT8 aByte =0;
    
    while ( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) == RESET ) {}   /* Wait for DR register to be empty */                
    SPI_I2S_SendData( SPI2, txByte );  

    /* if we are expecting data, get it */
    //if( pRxBuf !=NULL_PTR )        
    while ( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) == RESET ) {}              
    
    /* must always read SPIx->DR */
    aByte =SPI_I2S_ReceiveData(SPI2);
        
    while ( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_BSY ) == SET ) {}    
    
    return aByte;
}
  

