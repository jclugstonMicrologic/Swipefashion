/** C Source ******************************************************************
*
* NAME      main.c
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
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


/* hardware includes. */
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "sysTimers.h"

/* HI layer, to be moved when FD complete, here for testing */
#include "gpioHi.h"
#include "rtcHi.h"
#include "dataFlashHi.h"
#include "accelHi.h"
#include "wdtHi.h"
#include "lcdHi.h"

#include "AdcFd.h"
#include "crc.h"
#include "PressureTdrHi.h"
#include "PCMachine.h"

#include "GatewayConfig.h"
#include "MainControlTask.h"
#include "PowerManagement.h"

//#include "vector.h"

#define PC_ENABLED              1

BOOL TargetHardwareInit(void);
BOOL AmcTasksInit(void);
void vTask(void * pvParameters);

/*-----------------------------------------------------------*/


/*
*|----------------------------------------------------------------------------
*|  Routine: vTimerCallback
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void vTimerCallback( TimerHandle_t xTimer )
{
   
#define BLUE_LED_TEST    
#ifdef RED_LED_TEST  
    RED_LED_TOGGLE;   
#else        
    GREEN_LED_TOGGLE;   
#endif    

    TickType_t xTicks=xTaskGetTickCount();
   
#if 0   
    UINT8 rssi;
    
    rssi =IridiumGetRssi();

    RED_LED_OFF;
    BLUE_LED_OFF;            
    GREEN_LED_OFF;
    
    switch(rssi)
    {
        case 0:
            break;
        case 1:
            RED_LED_ON;
            break;            
        case 2:
            BLUE_LED_ON;
            break;   
        case 3:
            RED_LED_ON;
            BLUE_LED_ON;
            break;   
        case 4:
            GREEN_LED_ON;
            break;   
        case 5:
            RED_LED_ON;
            GREEN_LED_ON;            
            break;                      
    }
#endif    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: main
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int main(void)
{       
    /* initialize the gateway hardware */
    if( !TargetHardwareInit() )
    {
        //!!!
        return FALSE;
    }

#if 0    
    /* initialize a periodic timer for testing */    
    if( TimerCreate(250, vTimerCallback) ==NULL_PTR )
    {
        //!!!
        return FALSE;
    }
#endif
    
    /* initialize the gateway tasks, modules, state machines, etc */
    if( !AmcTasksInit() )
    {
        //!!!
        return FALSE;      
    }

      
    /* start the scheduler */
    vTaskStartScheduler();

 
    /* if all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TargetHardwareInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL TargetHardwareInit(void)
{
    /* setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();

    /* ensure all priority bits are assigned as preemption priority bits */
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    /* adjust interrupt vector table by vector offset (0x4000) */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, (0x08000000 | 0x4000) );
    
    /* check if the system has resumed from IWDG reset */
    if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET )
    {
        /* IWDGRST flag set */
        /* Clear reset flags */
        RCC_ClearFlag();
    }

    /* initialize uSec timer */
    TimerSetupUs();
    
    /* initialize all GPIO */
    GpioInit();
             
    /* initialize ADC */
    AdcInit();
  
    /* initialize internal RTC */
    RtcInit();
    
    RtcInit();       

    PressureTdrInit();

    /* initialize hardware crc */
    //CrcInit32();
      
    /* initialize watch dog timer, serviced in MainControlTask
       do not init if debugging, watchdog is a nuisance
    */ 
    
    /* disable IWDG if core is halted */
    DBGMCU->APB1FZR1 |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
     
    /*!!! need to find out how to disable during sleep(don't think it's possible)
     OR allow reset, then go to sleep out of reset without enabling wdt    
    */
//    WdtInit();  
   
    return TRUE;    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: AmcTasksInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AmcTasksInit(void)
{   
#ifdef PC_ENABLED      
    /* initialize PC serial comm port */    
    if( !PCMachineInit() )
    {
        //!!!
        return FALSE;      
    }
#endif
             
    
#ifdef TEST_FLASH    
UINT8 aBuf[16];
DataFlashRead(0x0000, 16, aBuf);
DataFlashEraseSector(0);
memset( aBuf, 0x00, sizeof(aBuf) );
DataFlashRead(0x0000, 16, aBuf);
DataFlashRead(0x0010, 16, aBuf);
DataFlashRead(0x0000, 16, aBuf);
memset( aBuf, 0x55, sizeof(aBuf) );
DataFlashWrite(0x0000, 16, aBuf);
memset( aBuf, 0x00, sizeof(aBuf) );
DataFlashRead(0x0000, 16, aBuf);
#endif
   
    /* start serial receiver task for proprietary serial comms (Zigbee, PC GUI, MLINK) */
    SciBinaryStartReceiver();
            
    /* initialize the main control task */
    MainControlTaskInit();  

    /* Otherwise enters DFU mode to allow user programming his application */
    /* Init Device Library */
    //USBD_Init(&USBD_Device, &DFU_Desc, 0);
      
    return TRUE;
}



/*-----------------------------------------------------------*/

