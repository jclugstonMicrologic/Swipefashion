/** C Source ******************************************************************
*
* NAME      adcHi.c
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

#include "sysTimers.h"
#include "adcHi.h"

UINT16 AdcRawData[NBR_ADC_CHANNELS]; // DMA destination

void adc_dma_init(void);

/*
*|----------------------------------------------------------------------------
*|  Routine: ADC_Init
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void adc_init(void)
{  
    /*
    ***********************
     L O C A L   D A T A
    ***********************
    */

    /*
    *************************
     E R R O R   C H E C K S
    *************************
    */


    /*
    *************************
     C O D E
    *************************
    */
    ADC_InitTypeDef       ADC_InitStructure;;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
  
    /* Enable ADC clock */
    RCC_AHB2PeriphClockCmd( RCC_AHB2Periph_ADC1, ENABLE );
    RCC->CCIPR |=0x30000000;
    
    ADC_CommonStructInit( &ADC_CommonInitStruct );
    
    ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;       /* ADCs operate independently */
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div6;    /* Can be 2, 4, 6, 8 */
    ADC_CommonInitStruct.ADC_DMAAccessMode =ADC_DMAAccessMode_Disabled;  
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  

    ADC_CommonInit( &ADC_CommonInitStruct );
   
//#define USE_DMA    
#ifndef USE_DMA
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */    
    /* ADC1 Initialization */
    ADC_StructInit( &ADC_InitStructure );
    
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;          
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;        
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv =1;//ADC_ExternalTrigConv_T1_CC1;    
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;           
    ADC_InitStructure.ADC_NbrOfConversion =NBR_ADC_CHANNELS;
    
    ADC_Init( ADC1, &ADC_InitStructure );

    TimerDelayUs(100);
    /* Start ADC calibration */
    ADC1->CR |=ADC_CR_ADCAL;
    
    /* Set the channel and sample time */
    ADC_RegularChannelConfig( ADC1, ADC_Channel_1, 1, ADC_SampleTime_480Cycles );
    ADC_RegularChannelConfig( ADC1, ADC_Channel_2, 2, ADC_SampleTime_480Cycles);     
    ADC_RegularChannelConfig( ADC1, ADC_Channel_3, 3, ADC_SampleTime_480Cycles);     
    ADC_RegularChannelConfig( ADC1, ADC_Channel_4, 4, ADC_SampleTime_480Cycles);     
    ADC_RegularChannelConfig( ADC1, ADC_Channel_TempSensor, 5, ADC_SampleTime_480Cycles);    
    
    /* enable VBAT channel: channel18 */
    //ADC_VBATCmd(ENABLE);
    //ADC_VREFCmd(ENABLE);
  
    /* enable TempSensor channel17 */
    ADC_TempSensorCmd(ENABLE);
    
    ADC_Cmd( ADC1, ENABLE );
    
    ADC_DiscModeCmd(ADC1, ENABLE);          
#else   
/*********** DMA ******************************************************/
    adc_dma_init();
	
    /* ADC1 DeInit */  
    ADC_DeInit(); 
	
    /* reset ADC init structure parameters values */
    /* initialize the ADC_Resolution member */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;

    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    /* Initialize the ADC_ContinuousConvMode member */
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    /* Initialize the ADC_ExternalTrigConvEdge member */
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    /* Initialize the ADC_ExternalTrigConv member */
    ADC_InitStructure.ADC_ExternalTrigConv = 0;
    /* Initialize the ADC_DataAlign member */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 3;

    ADC_Init(ADC1, &ADC_InitStructure); 

    ADC_DMACmd(ADC1, ENABLE);  
           
    /* Set the channel and sample time */
    ADC_RegularChannelConfig( ADC1, ADC_Channel_3, 1, ADC_SampleTime_480Cycles );
    ADC_RegularChannelConfig( ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_480Cycles);
    ADC_RegularChannelConfig( ADC1, ADC_Channel_Vbat, 3, ADC_SampleTime_480Cycles);
    
    /* enable VBAT channel: channel18 */
    ADC_VBATCmd(ENABLE);
  
    /* enable TempSensor and Vrefint channels: channel16 and channel17 */
    ADC_TempSensorVrefintCmd(ENABLE);
  
    /* enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
            
#ifdef ENABLE_ADC_INTERRUPT    
    /* NVIC configuration *******************************************************/    
    
    /* enable ADC1 interrupts */
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
#endif
    ADC_Cmd(ADC1, ENABLE);           
           
    ADC_SoftwareStartConv(ADC1);    
#endif   
}


/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine:  adc_dma_init
*|  Description:
*|----------------------------------------------------------------------------
*/
void adc_dma_init(void)
{ 
    DMA_InitTypeDef   DMA_InitStructure;
      
    /* DMA1 clock enable */ 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
      
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;     
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; //((uint32_t)0x4001204C);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&AdcRawData;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
      
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);     
    DMA_Cmd(DMA2_Stream0, ENABLE);

#define ENABLE_DMA_INTERRUPT    
#ifdef ENABLE_DMA_INTERRUPT  
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE); //Enable the DMA1 - Channel1
    NVIC_InitTypeDef NVIC_InitStructure;
    //Enable DMA1 channel IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
#endif  
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: adc_getRawData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT16 adc_getRawData(void)
{
    UINT16 adcRawData;
    
    /* Start the ADC conversion and wait for completion */
    ADC_SoftwareStartConv( ADC1 );
    while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET ){}
    
    adcRawData = ADC_GetConversionValue( ADC1 );
    
    return adcRawData;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ADC_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void ADC_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DMA2_Stream0_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void DMA2_Stream0_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
    {
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    }     
}



