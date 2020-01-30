/** H Header  ******************************************************************
*
* NAME      PowerManagement.h
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

#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include "types.h"

#define POWER_DOWN_CELL_MODULE  POWER_5V_OFF
#define POWER_DOWN_GPS_MODULE   GPS_DISABLE

                        
typedef enum
{
    PWR_MANAGE_INIT_STATE =0,
    
    PWR_MANAGE_IDLE_STATE,
    PWR_MANAGE_SLEEP_STATE,
    PWR_MANAGE_PWR_DOWN_STATE,
    PWR_MANAGE_DISABLE_STATE,
    
    PWR_MANAGE_LAST_STATE
      
}PwrManagementStatesTypeEnum;

typedef enum
{
    PWR_CELL_MODULE =0,
    PWR_GPS_MODULE,
    PWR_IRIDIUM_MODULE,
    PWR_MLINK_MODULE,

    PWR_5V_REGULATOR,
    PWR_ALL_PERIPH,
    
    PWR_LAST_MODULE
   
}PwrDownModuleTyepEnum;
  

PwrManagementStatesTypeEnum PowerManagementMachine(void);

void PowerUpPeripheral( PwrDownModuleTyepEnum periph );
void PowerDownPeripheral( PwrDownModuleTyepEnum periph );
BOOL PowerCheckChargerStatus(void);
BOOL CheckPowerManagementSleep(void);


#endif




