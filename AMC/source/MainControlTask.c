/** C Source ******************************************************************
*
* NAME      MainControlTask.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded worbench for ARM v8.20.2
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2020, MICROLOGIC
* Calgary, Alberta, Canada, www.micrologic.ab.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include "MainControlTask.h"
#include "PowerManagement.h"
#include "FreeRTOS.h"
#include "timers.h"

#include "wdtHi.h"
#include "gpioHi.h"
#include "dataFlashHi.h"

#include "AdcFd.h"
#include "PressureTdrHi.h"
   
void MainControlTask(void * pvParameters);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: MainControlTaskInit
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MainControlTaskInit( void )
{
    BaseType_t xReturn;
    
    /* spawn the Main Control task */
    xReturn =xTaskCreate( MainControlTask, 
                          "Main Control Task", 
                          1024, //(configMINIMAL_STACK_SIZE+128),
                          NULL, 
                          (tskIDLE_PRIORITY + 2UL), 
                          ( TaskHandle_t * ) NULL);
    
    if(xReturn !=pdPASS)
    {
        //!!!
        return FALSE;
    } 
    else
        return TRUE;    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: MainControlTask
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void MainControlTask(void * pvParameters)
{
    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();

    UINT32 pressure =0;
    
  //  TickType_t delayTime = xTaskGetTickCount();    

    S_VALVE1_OPEN;
    
    for( ;; )
    {      
        KickWdt();         
              
        /* if wake up tasks are complete, power down modules, put micro to sleep */                     
        AdcMeasureReadings();
                 
        PressureTdrRead(0, 24, (UINT8 *)pressure);

        /* this delay allows lower priorty tasks to run */
        //vTaskDelay(1);
   
        /* place this task in the blocked state until it is time to run again */
        vTaskDelayUntil( &xNextWakeTime, 1 );        
    }
}







