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

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

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
UINT32 loopCnt =0;
char aStr[32];
UINT8 valveState =0;

    //UINT8 chipId;
    
  //  TickType_t delayTime = xTaskGetTickCount();    

    /* !!! test !!! */
    strcpy(aStr, "AMC v");
    strcat(aStr, FW_VERSION);
    strcat(aStr, "\r\n\r\n");
     
    SendString(aStr);
    
    for( ;; )
    {      
        KickWdt();         
              
   //     AdcMeasureReadings();
                 
        if( (loopCnt++ %1000) ==0 )
        {
            PressureTdr_ReadPT(0, &PressureValue[0], &TemperatureValue[0]);
            PressureTdr_ReadPT(7, &PressureValue[7], &TemperatureValue[7]);                  
            
            memset(&aStr, 0x00,sizeof(aStr));
            sprintf(aStr, "P1 %2.2fkPa T1 %2.1fdegC\n\r", PressureValue[0], TemperatureValue[0]);
            SendString(aStr);
            
            memset(&aStr, 0x00,sizeof(aStr));
            sprintf(aStr, "P7 %2.2fkPa T7 %2.1fdegC\r\n\r\n", PressureValue[7], TemperatureValue[7]);
            SendString(aStr);
        }
   
        if( valveState == 0)
        {
            if( !GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                OpenValve(1);      
                valveState =1;
            }
        }
        else if( valveState == 1)
        {
            if( GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                valveState =2;
            }
        }
        else if( valveState == 2)
        {
            if( !GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                CloseValve(1); 
                valveState =3;
            }
        }                
        else if( valveState ==3)
        {
            if( GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                valveState =0;
            }
        }               

        /* this delay allows lower priorty tasks to run */
        //vTaskDelay(1);

       // BluetoothMachine();

        /* place this task in the blocked state until it is time to run again */
        vTaskDelayUntil( &xNextWakeTime, 1 );        
    }
}







