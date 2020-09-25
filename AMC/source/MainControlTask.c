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

#include "AdcFd.h"
#include "PressureTdrHi.h"
#include "BluetoothMachine.h"
   
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

    //UINT8 chipId;
    
  //  TickType_t delayTime = xTaskGetTickCount();    

    /* !!! test !!! */
    S_VALVE1_OPEN;
    S_VALVE4_OPEN;
    S_VALVE7_OPEN;
    
    for( ;; )
    {      
        KickWdt();         
              
        AdcMeasureReadings();
         
        PressureTdr_Read(BMP3_PRESS_DATA_ADDR, 0, 3, (UINT8 *)PressureValue[0]);
        PressureTdr_Read(BMP3_PRESS_DATA_ADDR, 8, 3, (UINT8 *)PressureValue[7]);

        SciSendByte(SCI_PC_COM, 'H');
        
        /* this delay allows lower priorty tasks to run */
        //vTaskDelay(1);

        BluetoothMachine();

        /* place this task in the blocked state until it is time to run again */
        vTaskDelayUntil( &xNextWakeTime, 1 );        
    }
}







