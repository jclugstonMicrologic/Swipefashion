/** C Source ******************************************************************
*
* NAME      wdtHi.c
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

#include "wdtHi.h"


/*
*|----------------------------------------------------------------------------
*|  Routine: WdtInit
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void WdtInit(void)
{
    UINT32 lsiFreq;
    
    /* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
    lsiFreq =32000;// GetLSIFrequency();
   
    /* IWDG timeout equal to t ms (the timeout may varies due to LSI frequency
        dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);


    /* IWDG counter clock: LSI/256 */
    IWDG_SetPrescaler(IWDG_Prescaler_256);   //IWDG_Prescaler_32
     
    /* Set counter reload value to obtain 250ms IWDG TimeOut.
        Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/32)
                            = 0.25s / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128
    */
        
    IWDG_SetReload(lsiFreq/64);//128);
    
    // we now have a 2second watchdog timeout
    
    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();         
} 


static u8 WWDG_CNT=0x7f;     /*Save the WWDG counter settings, the default is the largest */


/**
 * Window watchdog interrupt service routine
 */
void WWDG_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;    /*WWDG interrupt*/
    /* In 2, sub priority 3 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_InitStructure);/*    NVIC initialization*/
}

void WWDG_Init(u8 tr,u8 wr,u32 fprer)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);    /*WWDG clock enable*/

     WWDG->CFR = 0x3DF;                    //Set WWDG_CFR register
     WWDG->SR = 0x00;                      //Clear interrupt flag
     WWDG->CR = 0xFF;  
#if 0
    WWDG_SetPrescaler(fprer); /*Set the IWDG pre frequency value*/
    WWDG_SetWindowValue(wr);    /*Settings window value*/

   // WWDG_CNT=tr&WWDG_CNT; /* The initialization of WWDG_CNT */
    WWDG_Enable(WWDG_CNT);    /*Enable the watchdog, the         set the counter */
    WWDG_ClearFlag();    /*Remove pre wakeup interrupt flag*/
#endif
    
    WWDG_NVIC_Init();/* Initial window watchdog NVIC */
    WWDG_EnableIT(); /* Open the window watchdog interrupt */
}


/**
 * Set the WWDG value of the counter weight
 */
void WWDG_Set_Counter(u8 cnt)
{
    WWDG_Enable(cnt); /*Enable the watchdog, the         set the counter    */
}

/**
 * The watchdog interrupt service routine
 */
void WWDG_IRQHandler(void)
{
    WWDG_Set_Counter(WWDG_CNT);
    WWDG_ClearFlag();    /*Remove pre wakeup interrupt flag*/
}



/*
*|----------------------------------------------------------------------------
*|  Routine: KickWdt
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void KickWdt(void)
{
    // reload/kick IWDG counter
    IWDG_ReloadCounter();
    
   // WWDG_Set_Counter(WWDG_CNT);
//    WWDG_ClearFlag();    /*Remove pre wakeup interrupt flag*/
      
}

