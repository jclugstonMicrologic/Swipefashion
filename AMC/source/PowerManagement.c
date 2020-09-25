/** C Source ******************************************************************
*
* NAME      PowerManagement.c
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

#include "PowerManagement.h"
#include "AmcConfig.h"

#include "gpioHi.h"
#include "rtcHi.h"

#include "FreeRTOS.h"
#include "timers.h"

#define REPORT_TIME     10800 /* report into server every 6hrs (3600sec/hr/2sec/cnt*6hrs) =10800cnts */

typedef struct
{
    PwrManagementStatesTypeEnum machState;
    PwrManagementStatesTypeEnum prevMachState;
    
    UINT32 timer;
          
}pwr_mgmt_t;

pwr_mgmt_t PowerManagement;

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: PwrMgmt_Init
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PwrMgmt_Init(void)
{
    memset( &PowerManagement, 0x00, sizeof(PowerManagement) );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PowerManagementShutdown
*|  Description: shutdown all peripherals, clocks, etc
                 config I/O for power saving
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PwrMgmt_Shutdown(void)
{
//    POWER_5V_OFF;    
}

#define STAY_AWAKE_TIME 5000 // 30 seconds???


/*
*|----------------------------------------------------------------------------
*|  Routine: PowerManagementStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PwrMgmt_StateProcess
(
    pwr_mgmt_t *structInfo,
    PwrManagementStatesTypeEnum nextState
)
{
    structInfo->prevMachState =structInfo->machState;
    structInfo->machState =nextState;
    
    structInfo->timer =xTaskGetTickCount();
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: PowerManagementMachine
*|  Description:
*|  Arguments: none
*|  Retval:
*|----------------------------------------------------------------------------
*/
PwrManagementStatesTypeEnum PwrMgmt_Machine(void)
{             
    TickType_t xTicks;
    
    xTicks=xTaskGetTickCount();

    switch( PowerManagement.machState )
    {
        case PWR_MANAGE_INIT_STATE: 
            /* init state, do some stuff */
            PowerManagement.sleepTimer =xTaskGetTickCount();
            
            PowerManagement.powerDownCounter =0;
            
            PowerManagement.machState =PWR_MANAGE_IDLE_STATE;
            break;
        case PWR_MANAGE_IDLE_STATE:            
            if( (xTicks-PowerManagement.sleepTimer) >STAY_AWAKE_TIME )             
            {   
                if( !SOL_PGOOD )
                {
                    /* get meter ticket data, and que send log message */
                }
                else
                {
                    /* we are powered up, so stay awake and
                       send data periodically
                    */
                    PowerManagementStateProcess(&PowerManagement, PWR_MANAGE_DISABLE_STATE);
                }
            }   
            break;
        case PWR_MANAGE_SLEEP_STATE:         
            /* wake up every second, check if time to stay awake, else sleep
               done this way because wdt timer needs to be serviced
            */
            RtcStartWakeupTimer(1); //GwGetReportInterval());          
            
            PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
            //PWR_EnterSTANDBYMode();
                
            /* 
               wake from sleep uses HSI, need to reinit our clock 
               to use HSE 
            */                           
            SystemInit();
                        
            /* for testing */
            GREEN_LED_TOGGLE; 
            
            /* wake up in(sleep for) time, seconds */      
            if( PowerManagement.powerDownCounter ++ >GwGetReportInterval() )
            {                  
                /* for testing */
                GREEN_LED_OFF;                
                POWER_5V_ON;
                
                while(1){}
                
                /* stay awake */
                PowerManagement.machState =PWR_MANAGE_INIT_STATE;
            }
                
            if( !GwGetSleepEnabled() )
            {
                /* may have been disabled since we entered this state, 
                  so keep checking */
                PowerManagement.machState =PWR_MANAGE_DISABLE_STATE;
            }                
            break;
        case PWR_MANAGE_PWR_DOWN_STATE:                   
            break;
        case PWR_MANAGE_DISABLE_STATE:
            /* do nothing */
            if( GwGetSleepEnabled())
                PowerManagement.machState =PWR_MANAGE_INIT_STATE;
            break;
        default:
            break;          
    }   
    
    return PowerManagement.machState;
}
#endif


/*
*|----------------------------------------------------------------------------
*|  Routine: CheckPowerManagementSleep
*|  Description:
*|  Arguments: none
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL CheckPowerManagementSleep(void)
{
    if( PowerManagement.machState ==PWR_MANAGE_SLEEP_STATE )
        return TRUE;
    else
        return FALSE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PowerUpPeripheral
*|  Description:
*|  Arguments: none
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PowerUpPeripheral( PwrDownModuleTyepEnum periph )
{
    switch( periph)
    {
        case PWR_CELL_MODULE:            
            break;
        case PWR_GPS_MODULE:            
            break;  
        case PWR_IRIDIUM_MODULE:
            break;      
        case PWR_MLINK_MODULE:
            break;            
        case PWR_5V_REGULATOR:
//            POWER_5V_ON;
            break;
        case PWR_ALL_PERIPH:            
            /* typically each device is turned on in its own machine, so no
               need for a global all on
            */
            break;
    }      
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PowerDownPeripheral
*|  Description:
*|  Arguments: none
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PowerDownPeripheral( PwrDownModuleTyepEnum periph )
{
    switch( periph)
    {
        case PWR_CELL_MODULE:
            break;
        case PWR_GPS_MODULE:
            break;  
        case PWR_IRIDIUM_MODULE:
            break;      
        case PWR_MLINK_MODULE:
            break;
        case PWR_5V_REGULATOR:
            //POWER_5V_OFF;
            break;            
        case PWR_ALL_PERIPH:
            /* disable all peripherals, any device on the 3V/5V regulators
               needs to be turned off individually
            */
            break;            
    }
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PowerCheckChargerStatus
*|  Description: Check if battery charger is on/charging
*|  Arguments: none
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PowerCheckChargerStatus(void)
{    
    return TRUE;
}



