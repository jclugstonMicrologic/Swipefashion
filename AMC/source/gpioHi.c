/** C Source ******************************************************************
*
* NAME      gpioHi.c
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
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "gpioHi.h"
#include "FreeRTOS.h"
#include "task.h"

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: GpioInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpioInit
(
    void
)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
 
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
  
    /* enable the GPIO Clock */
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_GPIOA, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_GPIOB, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_GPIOC, ENABLE); 
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_GPIOD, ENABLE); 
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_GPIOE, ENABLE); 

    /* set output states before setting pin as an output */
    POWER_5V_OFF;
      
    /* configure the following GPIO pins as outputs */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   
    GPIO_InitStructure.GPIO_Pin =GREEN_LED_PIN;
    GPIO_Init(GREEN_LED_PORT, &GPIO_InitStructure);          
    
    GPIO_InitStructure.GPIO_Pin = RED_LED_PIN;
    GPIO_Init(RED_LED_PORT, &GPIO_InitStructure);              
                     
    GPIO_InitStructure.GPIO_Pin = POWER_5V_ENABLE_PIN;
    GPIO_Init(POWER_5V_ENABLE_PORT, &GPIO_InitStructure);    
        
    GPIO_InitStructure.GPIO_Pin = FLASH_M1_CS_PIN;
    GPIO_Init(FLASH_M1_CS_PORT, &GPIO_InitStructure);              
    
    GPIO_InitStructure.GPIO_Pin = FLASH_M2_CS_PIN;
    GPIO_Init(FLASH_M2_CS_PORT, &GPIO_InitStructure);                  
       
          
    /* configure the following GPIO pins as inputs */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;  
    
      
    GPIO_InitStructure.GPIO_Pin = WAKEUP_BTN_PIN;
    GPIO_Init(WAKEUP_BTN_PORT, &GPIO_InitStructure);  
        
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);       
   
    /* enable and set EXTI line interrupt */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource6);
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode =EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);    
   
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                
      
    /* configure ADC Channels as analog input */    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AN_ADC_CNTRL;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    
    GPIO_InitStructure.GPIO_Pin =(ADC_1_PIN | ADC_2_PIN | 
                                  ADC_VSENSE1_PIN);
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);		   
   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
        
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;                                 
    GPIO_Init(GPIOB, &GPIO_InitStructure);		           
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpioSleep
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpioSleep
(
    void
)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    FLASH_NEGATE_M1_CS;
    FLASH_NEGATE_M2_CS;
    
    /* configure the following GPIO pins as inputs */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz; 
   
    GPIO_InitStructure.GPIO_Pin =GREEN_LED_PIN;
    GPIO_Init(GREEN_LED_PORT, &GPIO_InitStructure);                   

    GPIO_InitStructure.GPIO_Pin = RED_LED_PIN;
    GPIO_Init(RED_LED_PORT, &GPIO_InitStructure);              
       
    GPIO_InitStructure.GPIO_Pin =(ADC_1_PIN | ADC_2_PIN | 
                                  ADC_VSENSE1_PIN);
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);		   

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;                                 
    GPIO_Init(GPIOA, &GPIO_InitStructure);         
    
    /* USART TX/RX pins */
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;                                 
    GPIO_Init(GPIOD, &GPIO_InitStructure);                 
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;                                 
    GPIO_Init(GPIOD, &GPIO_InitStructure);      
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;                                 
    GPIO_Init(GPIOC, &GPIO_InitStructure);                 
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;                                 
    GPIO_Init(GPIOD, &GPIO_InitStructure);          
        
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;                                 
    GPIO_Init(GPIOA, &GPIO_InitStructure);          
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;                                 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;                                 
    GPIO_Init(GPIOC, &GPIO_InitStructure);     
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);      
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;                                 
    GPIO_Init(GPIOB, &GPIO_InitStructure);                     
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;                                 
    GPIO_Init(GPIOB, &GPIO_InitStructure);     
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;                                 
    GPIO_Init(GPIOE, &GPIO_InitStructure);     
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;                                 
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
    /* SPI pins */
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;                                 
    GPIO_Init(GPIOE, &GPIO_InitStructure);      
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
    GPIO_Init(GPIOE, &GPIO_InitStructure);          
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;                                 
    GPIO_Init(GPIOE, &GPIO_InitStructure);      
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);      
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5;                                 
    GPIO_Init(GPIOB, &GPIO_InitStructure);      
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);          
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;                                 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    

    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AN; //GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_All;
    
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
        
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_All;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    
    /* ensure JTAG pins are not changed, can be changed for production use */
    GPIO_InitStructure.GPIO_Pin =0xffe7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);     
    
    GPIO_InitStructure.GPIO_Pin =0x1fff;
    GPIO_Init(GPIOA, &GPIO_InitStructure);         
    
    ADC_Cmd( ADC1, DISABLE );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: EXTI9_5_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetFlagStatus(EXTI_Line6)==SET)
    {
        EXTI_ClearFlag(EXTI_Line6);            
    }    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: EXTI15_10_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetFlagStatus(EXTI_Line13)==SET)
    {
        EXTI_ClearFlag(EXTI_Line13);            
    }  
}


      

    