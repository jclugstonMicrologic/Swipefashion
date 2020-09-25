/** H Header  ******************************************************************
*
* NAME      GatewayConfig.h
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

#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include "types.h"

#define MAX_APN_LENGTH 28

typedef struct
{
    UINT32 gatewayId;
    UINT16 serverPort;
    UINT16 spare;
       
    char apn[MAX_APN_LENGTH]; 
    
    UINT8 ipAddr[4];
    
    UINT8 rtu[24]; /* spares for future use */
                
}NETWORK_CONFIG_STRUCT;

typedef struct
{
    UINT32 report; /* report every t seconds */ 
    
    BOOL sleepEnabled;
    UINT16 rtu;
   
}SCHEDULE_CONFIG_STRUCT;

typedef struct
{
    UINT8 facilityId;
    UINT8 spotId;
    UINT8 deviceType;  // sensor type TLOAD, XLOAD, etc
}SENSOR_CONFIG_STRUCT;

typedef struct
{
    NETWORK_CONFIG_STRUCT network;
    SCHEDULE_CONFIG_STRUCT shedule; 
    SENSOR_CONFIG_STRUCT sensor;
      
    UINT16 rtu;
    UINT16 crc;

}AMC_CONFIG_STRUCT;


typedef struct
{           
    UINT32 railCarNbr;
    
    UINT16 highLevel;
    UINT16 vOffset;
    UINT16 sensorRange1;
    UINT16 sensorRange2;
    
    UINT16 remoteUpdateRate;
    UINT16 logRate;
                    
    UINT16 spare2;
    UINT16 crc;
        
}AMC_SETUP_STRUCT;


BOOL AmcConfigInit(void);

void AmcGetConfig(AMC_CONFIG_STRUCT *pGwConfig);
void AmcGetSetup(AMC_SETUP_STRUCT *pGwSetup);

UINT32 GwGetReportInterval(void);
UINT16 GwGetSensorRange(void);
UINT16 GwGetShutoffLevel(void);

void GwSetIsenseValue(float value);
float GwGetIsenseValue(void);

BOOL GwGetSleepEnabled(void);


#endif




