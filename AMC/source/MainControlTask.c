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

#include "sysTimers.h"

//#include "AdcFd.h"
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
    
#ifdef TERMINAL_ENABLED
UINT32 loopCnt =0;
char aStr[32];
UINT8 valveState =0;
UINT8 sensorPresent =0;
UINT8 debounceCnt =0;
press_sensor_data_t PSensorData[8];
    
  //  TickType_t delayTime = xTaskGetTickCount();    

    /* !!! test !!! */
    strcpy(aStr, "AMC v");
    strcat(aStr, FW_VERSION);
    strcat(aStr, __DATE__);
    strcat(aStr, "\r\n\r\n");
     
    SciAsciiSendString(SCI_PC_COM, aStr);
    
    sensorPresent =PressureTdr_GetTdrs();
    
    for(int j=0; j<8; j++)
    {
        if( sensorPresent & (0x01<<j) )
        {
            sprintf(aStr, "Pressure Sensor %d detected\r\n", j+1);
        }
        else
        {
            sprintf(aStr, "Pressure Sensor %d NOT detected\r\n", j+1);            
        }
        
        SciAsciiSendString(SCI_PC_COM, aStr);
    }
#endif    
    
    for( ;; )
    {      
        KickWdt();         
              
   //     AdcMeasureReadings();

#ifdef TERMINAL_ENABLED        
        if( (++loopCnt %100) ==0 )
        {
            for(int sensor =0; sensor<8; sensor++)
            {
                PressureTdr_ReadPT(sensor, &PSensorData[sensor].press, &PSensorData[sensor].temp);
            }            
            
            if( (loopCnt %1000) ==0 )
            {
                memset(&aStr, 0x00,sizeof(aStr));
                sprintf(aStr, "P1: %2.3fkPa T1: %2.1fdegC\r\n", PSensorData[0].press, PSensorData[0].temp);
                SciAsciiSendString(SCI_PC_COM, aStr);
                
                memset(&aStr, 0x00,sizeof(aStr));
                sprintf(aStr, "P8: %2.3fkPa T8: %2.1fdegC\r\n\r\n", PSensorData[7].press, PSensorData[7].temp);
                SciAsciiSendString(SCI_PC_COM, aStr);
            }
        }
        
        if( valveState == 0)
        {
            debounceCnt =0;
            
            while( !GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                if( debounceCnt ++ >50 )
                {                
                    OpenValve(1);      
                    valveState =1;
                    break;
                }                
                TimerDelayUs(1000);
            }                    
        }
        else if( valveState == 1)
        {
            debounceCnt =0;
            
            while( GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                if( debounceCnt ++ >50 )
                {                
                    valveState =2;
                    break;
                }                
                TimerDelayUs(1000);
            }
        }
        else if( valveState == 2)
        {
            debounceCnt =0;
           
            while( !GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                if( debounceCnt ++ >50 )
                {
                    CloseValve(1); 
                    valveState =3;
                    break;
                }
                TimerDelayUs(1000);
            }
        }                
        else if( valveState ==3)
        {
            debounceCnt =0;
            
            while( GPIO_ReadInputDataBit(USER_BTN_PORT, USER_BTN_PIN) )
            {
                if( debounceCnt ++ >50 )
                {
                    valveState =0;
                    break;
                }
                TimerDelayUs(1000);
            }
        }               
#endif
        
        /* this delay allows lower priorty tasks to run */
        //vTaskDelay(1);

        Ble_Machine();
          
        PressureTdr_CheckOverPres();
        
        /* place this task in the blocked state until it is time to run again */
        vTaskDelayUntil( &xNextWakeTime, 1 );        
    }
}







