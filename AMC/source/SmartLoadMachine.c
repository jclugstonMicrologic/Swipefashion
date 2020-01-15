/** C Source ******************************************************************
*
* NAME      SmartLoadMachine.c
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
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include "GatewayConfig.h"
#include "SmartLoadMachine.h"

#include "AdcFd.h"
#include "gpioHi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

typedef enum
{
    SMARTLOAD_INIT_STATE =0,
    SMARTLOAD_IDLE_STATE,    
    SMARTLOAD_RUN_STATE,
    SMARTLOAD_POTENTIAL_SD_STATE,
    SMARTLOAD_SD_STATE,

    SMARTLOAD_LAST_STATE
      
}SmartLoadStatesTypeEnum;


typedef struct
{
    SmartLoadStatesTypeEnum machState;
    SmartLoadStatesTypeEnum prevMachState;
    
    UINT32 timer;
    UINT32 sleepTimer;
          
}SMARTLOAD_STATE_STRUCT;


SMARTLOAD_STATE_STRUCT SmartLoadState;

SMARTLOAD_TANK_DATA SmartLoadTankData;

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: SmartLoadMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SmartLoadMachineInit(void)
{       
    memset( &SmartLoadTankData, 0x00, sizeof(SmartLoadTankData) );
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SmartLoadStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SmartLoadStateProcess
(
    SMARTLOAD_STATE_STRUCT *pStructInfo,
    SmartLoadStatesTypeEnum prevState,
    SmartLoadStatesTypeEnum nextState
)
{
    pStructInfo->prevMachState =prevState;
    pStructInfo->machState =nextState;
    
    pStructInfo->timer =xTaskGetTickCount();
}


/*
*|----------------------------------------------------------------------------
*|  Routine: SmartLoadMachine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SmartLoadMachine(void)
{   
    GW_SETUP_STRUCT gwSetup;    
           
    GwGetSetup(&gwSetup);   
        
    static UINT32 CalcTimer=0;
    
    TickType_t xTicks=xTaskGetTickCount();
   
    if( (xTicks - CalcTimer) >1000 )
    {
        /* slow this down, CalcFluidVolume math intense */
        SmartLoadTankData.level =GetSloadLevelAverage();
        SmartLoadTankData.measuredRange =Adc4To20Conversion(SmartLoadTankData.level, gwSetup.sensorRange1, gwSetup.sensorRange2);            
        //SmartLoadSensorData.measuredVolume =CalcFluidVolume(SmartLoadSensorData.measuredRange);     
        
        CalcTimer =xTicks;
    }
    
#if 1    
    switch( SmartLoadState.machState )
    {      
        case SMARTLOAD_INIT_STATE:
            
            BLUE_LED_OFF;          
        #if 0
            GwGetSetupFlash(&gwSetup);             
                        
            sdLevel =gwSetup.shutoffLevel;
            sensorRange =gwSetup.sensorRange;
        #endif
            SmartLoadStateProcess( &SmartLoadState, SmartLoadState.machState, SMARTLOAD_IDLE_STATE);
            break;
        case SMARTLOAD_IDLE_STATE:
            SmartLoadStateProcess( &SmartLoadState, SmartLoadState.machState, SMARTLOAD_RUN_STATE);
            break;            
        case SMARTLOAD_RUN_STATE:
            /* eventually we will convert lpgLevel to real value in AdcFd, for now 
                we want to retain the current reading so do conversion here
            */                                     
            if( SmartLoadTankData.measuredRange < gwSetup.shutoffLevel )
            {
                SmartLoadStateProcess( &SmartLoadState, SmartLoadState.machState, SMARTLOAD_POTENTIAL_SD_STATE);                
            }                          
            break;
        case SMARTLOAD_POTENTIAL_SD_STATE:
            if( SmartLoadTankData.measuredRange > gwSetup.shutoffLevel )
            {
                /* false reading */
                SmartLoadStateProcess( &SmartLoadState, SmartLoadState.machState, SMARTLOAD_RUN_STATE);                              
            }
            else if( (xTaskGetTickCount() - SmartLoadState.timer) >500 )
            {
                /* shut down pumps, etc */
                BLUE_LED_ON;
                SmartLoadStateProcess( &SmartLoadState, SmartLoadState.machState, SMARTLOAD_SD_STATE); 
            }             
            break;
        case SMARTLOAD_SD_STATE:          
            /* what now??? */
            if( SmartLoadTankData.measuredRange > gwSetup.shutoffLevel )
            {
                SmartLoadStateProcess( &SmartLoadState, SmartLoadState.machState, SMARTLOAD_INIT_STATE);                
            }                                    
            break;
    }
#endif
    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: SmartLoadGetData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SmartLoadGetData(SMARTLOAD_TANK_DATA *pSloadTankData)
{
    memcpy( pSloadTankData, &SmartLoadTankData, sizeof(SmartLoadTankData) );
}


