/** C Source ******************************************************************
*
* NAME      timer.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded worbench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "sysTimers.h"

//#include "FreeRTOS.h"
//#include "timers.h"


/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: TimerSetupUs
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL TimerSetupUs( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Timer 9 is a free running timer user to generate microsecond delays. */

    TIM_DeInit( TIM2 );
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

    /* Timer clock enable */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 0xffffffffUL;    /* free-run over full 32-bit range */
    TIM_TimeBaseStructure.TIM_Prescaler = 79;           /* divide 80MHz clock by 80 */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

    TIM_SetCounter( TIM2, 0 );

    TIM_Cmd( TIM2, ENABLE );        /* Enable timer */

    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TimerDelayUs
*|  Description:
*|  Arguments: time delay in micro seconds
*|  Retval: none
*|----------------------------------------------------------------------------
*/
void TimerDelayUs( UINT32 delayUL )
{
    UINT32 startTimeUL;
    
    startTimeUL = TIM2->CNT;
    delayUL += 1; // make sure wait at least delayUL, even if timer ticks right away
    while ((TIM2->CNT - startTimeUL) < delayUL)
    {}
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TimerCreate
*|  Description:
*|  Arguments: timeout -timer timeout value in milli seconds
*|             pCallBack -call back function to be invoked on timeout
*|  Retval:    true or false
*|----------------------------------------------------------------------------
*/
TimerHandle_t TimerCreate( int32_t timeout, void (*pCallBack)() )
{
    TimerHandle_t xTimers;
    
    xTimers =xTimerCreate("Timer", timeout, pdTRUE,( void * )0, pCallBack);
    
    if( xTimers != NULL )
    {
        /* Start the timer.  No block time is specified, and
          even if one was it would be ignored because the RTOS
          scheduler has not yet been started. */
        if( xTimerStart( xTimers, 0 ) != pdPASS )
        {
            /* The timer could not be set into the Active
               state. */
        }       
    }
    
    return xTimers;     
}



