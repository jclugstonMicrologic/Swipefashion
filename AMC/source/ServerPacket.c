/** C Source *******************************************************************
*
* NAME      ServerPacket.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded worbench for ARM v7.4
*
* REVISION LOG
*
********************************************************************************
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include <stdlib.h>

#include "ServerPacket.h"
#include "AdcFd.h"
#include "GpsMachine.h"
#include "GatewayConfig.h"

#include "LogMachine.h"
#include "MeterTicketMachine.h"
#include "TLoadMachine.h"
#include "PlcMachine.h"
#include "PowerManagement.h"

#include "dataFlashHi.h"
#include "accelHi.h"

#include "FreeRTOS.h"
#include "task.h"

#include "rtcHi.h"
#include "MainControlTask.h"
#include "CellularMachine.h"

#include <stdio.h>
#include <stdarg.h>
#include <pb_encode.h>
#include <pb_decode.h>

/* a periodic message to the server, GPS, etc */ 
//#define SENSOR_PERIODIC_MESSAGE_TIMER         60000
//#define GPS_PERIODIC_MESSAGE_TIMER      60000
#define ACCEL_PERIODIC_MESSAGE_TIMER        3600000
#define FW_UDPATE_PERIODIC_MESSAGE_TIMER     300000 

#define ENCODE_BUFFER_SIZE 192  //128

typedef struct
{
    BOOL systemStartup;
    BOOL fwUpdate;
    BOOL internalLogUpdate;
    BOOL forceLogTransfer;
    BOOL status;
    BOOL gwStatus;
    
    UINT32 sensorTxTimer;
    UINT32 gpsTxTimer;
    UINT32 accelTxTimer;
    UINT32 checkFwUpdateTimer;
       
}CLIENT_SERVER_MESSAGING;

CLIENT_SERVER_MESSAGING ClientServerMessaging;

typedef struct
{
    UINT32 current;
    UINT32 start;
    UINT32 end;
}LOG_MEMORY_ADDRESS;


/*** Functions ****************************************************************/
UINT32 GetMeterTicketFromLog(void);


/*
*|----------------------------------------------------------------------------
*|  Routine: ClientServerMessagingInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void ClientServerMessagingInit()
{
    ClientServerMessaging.systemStartup   =TRUE;
    ClientServerMessaging.gwStatus =TRUE;
    ClientServerMessaging.fwUpdate   =FALSE;
    ClientServerMessaging.internalLogUpdate =FALSE;
    ClientServerMessaging.forceLogTransfer =FALSE;
    ClientServerMessaging.status =FALSE;
    
    ClientServerMessaging.sensorTxTimer =xTaskGetTickCount();      
    ClientServerMessaging.gpsTxTimer =xTaskGetTickCount();      
}

/*
*|----------------------------------------------------------------------------
*|  Routine: BuildIridiumServerPacket
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL BuildIridiumServerPacket
(	
    IRIDIUM_PACKET_STRUCT *pIridiumPacket
)
{    
    ADC_READINGS_STRUCT adcData;
    RTC_TimeTypeDef RtcTimeStruct;  
    RTC_DateTypeDef RtcDateStruct; 

    AdcGetData(&adcData);
    
    RtcGetTime(RTC_Format_BIN, &RtcTimeStruct);
          
    RtcGetDate(RTC_Format_BIN, &RtcDateStruct);
      
    pIridiumPacket->std.gwId =1;            /* read from (setup) flash memory */
    pIridiumPacket->std.timeStamp =0x170203; /* compress RTC date/time */
    pIridiumPacket->lpgLevel =adcData.slLevel;
        
    pIridiumPacket->spare=0x1234;
    pIridiumPacket->crc  =0x5aa5;
       
    return TRUE;
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: BuildSensorServerPacket
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL BuildSensorServerPacket
(	
    SENSOR_PACKET_STRUCT *pSensorPacket
)
{    
    static UINT32 lat=0, lon=0;

    ADC_READINGS_STRUCT adcData;
    
    RTC_TimeTypeDef RtcTimeStruct;  
    RTC_DateTypeDef RtcDateStruct; 
   
    RtcGetTime(RTC_Format_BIN, &RtcTimeStruct);          
    RtcGetDate(RTC_Format_BIN, &RtcDateStruct);
    
    GetAdcData(&adcData);
    
    lat =GpsGetLatitude();
    lon =GpsGetLongitude();
    
    pSensorPacket->std.gwId =1234;           /* read from (setup) flash memory */
    pSensorPacket->std.timeStamp =0x170203;  /* compress RTC date/time */
    
    pSensorPacket->outage  =110;
    pSensorPacket->vBatt   =adcData.batteryVolts;
    pSensorPacket->temp    =adcData.temp;
    pSensorPacket->sensor1 =9876;
       
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetXloadSensorData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetXloadSensorData
(	
    XLOAD_SENSOR_DATA *pXloadSensorPacket
)
{
    GW_CONFIG_STRUCT gwConfig;    
    GwGetConfig(&gwConfig);
    
    pXloadSensorPacket->gwId =gwConfig.network.gatewayId;
    pXloadSensorPacket->outage =75.5;
    pXloadSensorPacket->vbat =12.4;
    pXloadSensorPacket->temp =22.5;
    pXloadSensorPacket->sensor1 =7.9;
    
    return true;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetSmartloadSensorData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetSmartloadSensorData
(	
    SENSOR_PACKET_STRUCT *pSloadSensorPacket
)
{
    GW_CONFIG_STRUCT gwConfig;
    TLOAD_TANK_DATA tLoadTankData;
    ADC_READINGS_STRUCT adcData;
    
    GwGetConfig(&gwConfig);    
    AdcGetData(&adcData);
        
    TLoadGetData(&tLoadTankData);   
                 
    pSloadSensorPacket->std.gwId =gwConfig.network.gatewayId;
    pSloadSensorPacket->std.timeStamp =RtcGetEpochTime();
    pSloadSensorPacket->std.pcktId =1;
    pSloadSensorPacket->std.spare =2;
    
    pSloadSensorPacket->outage =tLoadTankData.lvlSensor.measuredRange;
    pSloadSensorPacket->volume =6;
    pSloadSensorPacket->vBatt =123;
    pSloadSensorPacket->temp =adcData.temp;
    /* right now this is the 4-20mA value, good for initial development */
    pSloadSensorPacket->sensor1 =tLoadTankData.lvlSensor.current; 
    
    return true;
}
#endif

/*
*|----------------------------------------------------------------------------
*|  Routine: GetSensorDataFields
*|  Description: get the fields that are valid for this application
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GetSensorDataFields
( 
    SENSOR_DATA *pTloadSensorPacket
)
{
    /* initialize to 0 so all optional fields are set to false */
    memset(pTloadSensorPacket, 0x00, sizeof(SENSOR_DATA));
            
    /* now equate the fields for this application */    
    pTloadSensorPacket->has_senRange =true;
    pTloadSensorPacket->has_senReliability =true;
    pTloadSensorPacket->has_sensTemperature =true;
    pTloadSensorPacket->has_sensVbat =true;
    pTloadSensorPacket->has_spotId =true;
    pTloadSensorPacket->has_deviceType =true;
    
    /* car id is an array so we set the number of elements in the array to tx */
    pTloadSensorPacket->carId_count =0; 
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetTloadRegisterCmd
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetTloadRegisterCmd
(	
    REGISTER_GTW_CMD *pTloadRegisterPacket
)
{       
    GW_CONFIG_STRUCT gwConfig;
    
    GwGetConfig(&gwConfig);   
    
    pTloadRegisterPacket->std.type =MESSAGE_REGISTER_GTW_POST;
    pTloadRegisterPacket->std.gwId =gwConfig.network.gatewayId;
    pTloadRegisterPacket->std.timeStamp =RtcGetEpochTime();
    pTloadRegisterPacket->std.messageId =MESSAGE_REGISTER_GTW_POST;

    pTloadRegisterPacket->has_fwVersion =true;    
    pTloadRegisterPacket->fwVersion =atof(FW_VERSION);

    return true;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GetTloadSensorData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetTloadSensorData
(	
    SENSOR_DATA *pTloadSensorPacket
)
{
    BOOL pwrStatus =FALSE;
     
    GW_CONFIG_STRUCT gwConfig;
    TLOAD_TANK_DATA tLoadTankData;
    //PLC_TRUCK_FLOW_DATA plcTruckFlowData;
    ADC_READINGS_STRUCT adcData;
    
    GwGetConfig(&gwConfig);    
    //PlcGetData(&plcTruckFlowData);
    AdcGetData(&adcData);
        
    TLoadGetData(&tLoadTankData);   
    
    pwrStatus =PowerCheckChargerStatus();
       
    /* get the optional fields to send for this application */   
    GetSensorDataFields(pTloadSensorPacket);      
    
    pTloadSensorPacket->std.type =MESSAGE_SENSOR_DATA_POST;
    pTloadSensorPacket->std.gwId =gwConfig.network.gatewayId;
    pTloadSensorPacket->std.timeStamp =RtcGetEpochTime();
    pTloadSensorPacket->std.messageId =MESSAGE_SENSOR_DATA_POST;
    
    pTloadSensorPacket->facilityId =gwConfig.sensor.facilityId;
    pTloadSensorPacket->outage =tLoadTankData.lvlSensor.current;
    pTloadSensorPacket->flowRate =3; //tLoadTankData.plcSensor.rate; 
    pTloadSensorPacket->senRange =tLoadTankData.lvlSensor.measuredRange;
    pTloadSensorPacket->senReliability =tLoadTankData.lvlSensor.reliability;
    pTloadSensorPacket->sensTemperature =tLoadTankData.lvlSensor.temperature;
                        
    pTloadSensorPacket->sensVbat =pwrStatus; //adcData.batteryVolts;
    pTloadSensorPacket->density =8;// tLoadTankData.plcSensor.density; 
    
    pTloadSensorPacket->sensVol =9; //tLoadTankData.lvlSensor.temperature; //123.4;
    pTloadSensorPacket->loadId =23;
    
    pTloadSensorPacket->spotId =gwConfig.sensor.spotId;
    pTloadSensorPacket->deviceType =gwConfig.sensor.deviceType;
    
    /* if rail car id is <=16 characters then it is fine to 
       copy all data into index 0, in this example size is 10, we will add 
       a check later
    */    
    memcpy( &pTloadSensorPacket->carId[0],"TRIG123456", 10);
        
    return true;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetGpsData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetGpsData
(	
    GPS_DATA *pGpsData
)
{  
    GW_CONFIG_STRUCT gwConfig;    
    GwGetConfig(&gwConfig);
    
    /* initialize to 1 so all optional fields are set to true */
    memset(pGpsData, 0x01, sizeof(GPS_DATA));
    
    pGpsData->std.type =MESSAGE_GPS_DATA_POST;
    pGpsData->std.gwId =gwConfig.network.gatewayId;
    pGpsData->std.timeStamp =RtcGetEpochTime();
    pGpsData->std.messageId =MESSAGE_GPS_DATA_POST;
    
    pGpsData->lat =GpsGetLatitude();
    pGpsData->lon =GpsGetLongitude();
    pGpsData->alt =GpsGetAltitude();
    pGpsData->speed =GpsGetSpeed();
    
    pGpsData->status =GpsGetStatus();
           
    return true;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GetMeterTicketData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetMeterTicketData
(	
    METER_TICKET_DATA *pMeterTicketData
)
{     
    UINT32 meterTcktAddr =0;
    
    GW_CONFIG_STRUCT gwConfig;
     
    GwGetConfig(&gwConfig);   
           
    meterTcktAddr =GetMeterTicketFromLog();
    DataFlashRead(meterTcktAddr, sizeof(METER_TICKET_DATA), (UINT8 *)pMeterTicketData);
    
/* initialize to 1 so all optional fields are set to true */
memset(pMeterTicketData, 0x01, sizeof(METER_TICKET_DATA));
    
    pMeterTicketData->std.type =MESSAGE_METER_TCKT_DATA_POST;
    pMeterTicketData->std.gwId =gwConfig.network.gatewayId;
    pMeterTicketData->std.timeStamp =RtcGetEpochTime();
    pMeterTicketData->std.messageId =MESSAGE_METER_TCKT_DATA_POST;

    pMeterTicketData->reqSeqnbrTarget =1.0;
    pMeterTicketData->yymmdd =190321;
    pMeterTicketData->hhmm =1513;
    pMeterTicketData->ticketTruckCode =1000;
    pMeterTicketData->ticketWellCode =2000;
    
    pMeterTicketData->weighBill =1.1;
    pMeterTicketData->gccOil =2.2;
    pMeterTicketData->gccWater =3.3;
    pMeterTicketData->flowWeightedAvgDegC =4.4;
    pMeterTicketData->perWaterCut =5.5;
    pMeterTicketData->loadOilM3 =6.6;
    pMeterTicketData->loadWaterM3 =7.7;
    pMeterTicketData->normLoadM3 =8.8;
    pMeterTicketData->flowWeightedAvgkGM3 =9.9;
    pMeterTicketData->autoCompOil =10.1;
    pMeterTicketData->autoCompWater =11.2;
    pMeterTicketData->autoCompPer =12.3;
    pMeterTicketData->flowMins =13.4;
    pMeterTicketData->shrinkage =14.5;    
    pMeterTicketData->dryTankM3 =15.6;    
    pMeterTicketData->wetTankM3 =16.7;
    pMeterTicketData->loadKg =17.8;
    pMeterTicketData->futureTicketPar1 =18.9;
    pMeterTicketData->futureTicketPar2 =19.0;    
    pMeterTicketData->futureTicketPar3 =20.1;        
       
    return true;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GetTloadAccelData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetTloadAccelData
(	
    ACCELEROMETER_DATA *pTloadAccelData
)
{          
    GW_CONFIG_STRUCT gwConfig;
    AccelDataStruct accelData;    
     
    GwGetConfig(&gwConfig);   
    
    AccelGetAccelData(&accelData);
      
    /* initialize to 1 so all optional fields are set to true */
    memset(pTloadAccelData, 0x01, sizeof(ACCELEROMETER_DATA));
    
    pTloadAccelData->std.type =MESSAGE_ACCELEROMETER_DATA_POST;
    pTloadAccelData->std.gwId =gwConfig.network.gatewayId;
    pTloadAccelData->std.timeStamp =RtcGetEpochTime();
    pTloadAccelData->std.messageId =MESSAGE_ACCELEROMETER_DATA_POST;
    
    pTloadAccelData->accelX =accelData.iwAccelX;
    pTloadAccelData->accelY =accelData.iwAccelY;
    pTloadAccelData->accelZ =accelData.iwAccelZ;
    
    return true;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GetGwStatusData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetGwStatusData
(	
    GW_STATUS *pGwStatusData
)
{          
    GW_CONFIG_STRUCT gwConfig;    
     
    GwGetConfig(&gwConfig);   
         
    /* initialize to 1 so all optional fields are set to true */
    memset(pGwStatusData, 0x01, sizeof(GW_STATUS));
    
    pGwStatusData->std.type =MESSAGE_GW_STATUS_POST;
    pGwStatusData->std.gwId =gwConfig.network.gatewayId;
    pGwStatusData->std.timeStamp =RtcGetEpochTime();
    pGwStatusData->std.messageId =MESSAGE_GW_STATUS_POST;
    
    pGwStatusData->cellrssi  =CellularGetRssi();
    pGwStatusData->radiorssi = -3;
          
    return true;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetTloadLoadPlan
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetTloadLoadPlan
(	
     LOAD_PLAN *pTloadLoadPlan
)
{       
    GW_CONFIG_STRUCT gwConfig;
    
    GwGetConfig(&gwConfig);   
    
    /* initialize to 1 so all optional fields are set to true */
    memset(pTloadLoadPlan, 0x01, sizeof(LOAD_PLAN));
    
    pTloadLoadPlan->std.type =MESSAGE_LOAD_PLAN_GET;
    pTloadLoadPlan->std.gwId =gwConfig.network.gatewayId;
    pTloadLoadPlan->std.timeStamp =RtcGetEpochTime();
    pTloadLoadPlan->std.messageId =MESSAGE_LOAD_PLAN_GET;
    
    return true;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetFwUpdateVersion
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetFwUpdateVersion
(	
    FW_UPDATE_VERSION *pFwUpdateVer
)
{       
    GW_CONFIG_STRUCT gwConfig;
    
    GwGetConfig(&gwConfig);   
    
    /* initialize to 1 so all optional fields are set to true */
    memset(pFwUpdateVer, 0x01, sizeof(FW_UPDATE_VERSION));
    
    pFwUpdateVer->std.type =MESSAGE_FW_UPDATE_VERSION_GET;
    pFwUpdateVer->std.gwId =gwConfig.network.gatewayId;
    pFwUpdateVer->std.timeStamp =RtcGetEpochTime();
    pFwUpdateVer->std.messageId =MESSAGE_FW_UPDATE_VERSION_GET;
    
    pFwUpdateVer->fwVersion  =atof(FW_VERSION);
    
    return true;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SetGwStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SetGwStatus
(	
    STATUS_PACKET *pStatus
)
{
    GW_CONFIG_STRUCT gwConfig;
    
    GwGetConfig(&gwConfig);   
    
    pStatus->std.type =MESSAGE_STATUS;
    pStatus->std.gwId =gwConfig.network.gatewayId;
    pStatus->std.timeStamp =RtcGetEpochTime();
    pStatus->std.messageId =1;
      
    pStatus->status =STATUS_CODE_ACK;
    
    return true;
}


#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: GetMeterTicketData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetMeterTicketData
(	
    METER_TCKT_DATA *pMeterTicketData
)
{  
    GW_CONFIG_STRUCT gwConfig;    
    GwGetConfig(&gwConfig);
    
    pMeterTicketData->gwId =gwConfig.network.gatewayId;   
           
    return true;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetStartupData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GetStartupData
(	
    void
)
{  
    GW_CONFIG_STRUCT gwConfig;    
    GwGetConfig(&gwConfig);
    
    pMeterTicketData->gwId =gwConfig.network.gatewayId;   
           
    return true;
}
#endif



/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeTloadRegisterCmd
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeTloadRegisterCmd
(
    uint8_t *pDataBuf, 
    REGISTER_GTW_CMD *pTloadRegisterCmd
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, REGISTER_GTW_CMD_fields, pTloadRegisterCmd) )          
        return ostream.bytes_written;
    else 
        return 0;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeTloadRegisterCmd
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeTloadRegisterCmd
(
    uint8_t *pDataBuf, 
    REGISTER_GTW_CMD *pTloadRegisterCmd
)
{                
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);//sizeof(XLOAD_SENSOR_DATA));
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, REGISTER_GTW_CMD_fields, pTloadRegisterCmd);      
}


/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeTloadSensorData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeTloadSensorData
(
    uint8_t *pDataBuf, 
    SENSOR_DATA *pTloadSensorData
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, SENSOR_DATA_fields, pTloadSensorData) )    
        return ostream.bytes_written;
    else 
        return 0;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeXloadSensorData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeTloadSensorData
(
    uint8_t *pDataBuf, 
    SENSOR_DATA *pTloadSensorData
)
{                
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);//sizeof(XLOAD_SENSOR_DATA));
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, SENSOR_DATA_fields, pTloadSensorData);      
}

/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeGpsData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeGpsData
(
    uint8_t *pDataBuf, 
    GPS_DATA *pGpsData    
)
{               
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf,ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, GPS_DATA_fields, pGpsData) )    
        return ostream.bytes_written;
    else
        return 0;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeGpsData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeGpsData
(
    uint8_t *pDataBuf, 
    GPS_DATA *pGpsData
)
{          
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, GPS_DATA_fields, pGpsData);       
}

/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeMeterTicketData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeMeterTicketData
(
    uint8_t *pDataBuf, 
    METER_TICKET_DATA *pMeterTicketData
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, METER_TICKET_DATA_fields, pMeterTicketData) )        
        return ostream.bytes_written;
    else 
        return 0;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeMeterTicketData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeMeterTicketData
(
    uint8_t *pDataBuf, 
    METER_TICKET_DATA *pMeterTicketData
)
{                
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);//sizeof(XLOAD_SENSOR_DATA));
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, METER_TICKET_DATA_fields, pMeterTicketData);      
}


/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeTloadAccelData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeTloadAccelData
(
    uint8_t *pDataBuf, 
    ACCELEROMETER_DATA *pTloadAccelData
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, ACCELEROMETER_DATA_fields, pTloadAccelData) )        
        return ostream.bytes_written;
    else 
        return 0;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeTloadAccelData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeTloadAccelData
(
    uint8_t *pDataBuf, 
    ACCELEROMETER_DATA *pTloadAccelData
)
{                
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);//sizeof(XLOAD_SENSOR_DATA));
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, ACCELEROMETER_DATA_fields, pTloadAccelData);      
}

/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeTloadLoadPlan
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeTloadLoadPlan
(
    uint8_t *pDataBuf, 
    LOAD_PLAN *pTloadLoadPlan
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, LOAD_PLAN_fields, pTloadLoadPlan) )     
        return ostream.bytes_written;
    else 
        return 0;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeGwStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeGwStatus
(
    uint8_t *pDataBuf, 
    GW_STATUS *pGwStatus
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, GW_STATUS_fields, pGwStatus) )     
        return ostream.bytes_written;
    else 
        return 0;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeFwVersionGet
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
int EncodeFwVersionGet
(
    uint8_t *pDataBuf, 
    FW_UPDATE_VERSION *pFwUpdateVer
)
{             
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
    
    if( pb_encode(&ostream, FW_UPDATE_VERSION_fields, pFwUpdateVer) )     
        return ostream.bytes_written;
    else 
        return 0;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeTloadParameter
*|  Description:
*|  Retval: bool
*|----------------------------------------------------------------------------
*/
bool DecodeTloadParameter
(       
    uint8_t *pDataBuf,
    GW_PARAMETERS *pGwParameters
)
{
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, GW_PARAMETERS_fields, pGwParameters);
}
    
/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeTloadLoadPlan
*|  Description:
*|  Retval: bool
*|----------------------------------------------------------------------------
*/
bool DecodeTloadLoadPlan
(
    uint8_t *pDataBuf, 
    LOAD_PLAN *pTloadLoadPlan
)
{                
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, LOAD_PLAN_fields, pTloadLoadPlan);      
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeFwVersionAvailable
*|  Description: Check with server for latest version of fw available
*|  Retval: bool
*|----------------------------------------------------------------------------
*/
bool DecodeFwVersionAvailable
(
    uint8_t *pDataBuf, 
    FW_UPDATE_VERSION *pFwUpdateVer
)
{
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, FW_UPDATE_VERSION_fields, pFwUpdateVer);        
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeTloadStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeTloadStatus
(
    uint8_t *pDataBuf, 
    STATUS_PACKET *pTloadStatus
)
{                
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, ENCODE_BUFFER_SIZE);//sizeof(XLOAD_SENSOR_DATA));
            
    /* now we are ready to decode the message. */
    return pb_decode(&istream, STATUS_PACKET_fields, pTloadStatus);      
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: EncodeMeterTcktData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool EncodeMeterTcktData
(
    uint8_t *pDataBuf, 
    METER_TCKT_DATA *pMeterTcktData,
    uint16_t nbrBytes    
)
{
    pb_ostream_t ostream = pb_ostream_from_buffer(pDataBuf, nbrBytes);       
    return pb_encode(&ostream, METER_TCKT_DATA_fields, pMeterTcktData);            
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DecodeMeterTcktData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
bool DecodeMeterTcktData
(
    uint8_t *pDataBuf, 
    METER_TCKT_DATA *pMeterTcktData,
    uint16_t nbrBytes    
)
{
    /* create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(pDataBuf, nbrBytes);
    
    /* now we are ready to decode the message. */
    return pb_decode(&istream, METER_TCKT_DATA_fields, pMeterTcktData);       
}
#endif


#define GOOGLE_PROTOC_ENCODE_DECODE
/*
*|----------------------------------------------------------------------------
*|  Routine: ServerBuildMessage
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
int ServerBuildMessage
(
    char *pTxBuf,
    ServerMsgsTypeEnum serverMsg
)
{
    //static METER_TCKT_DATA meterTcktData;
    static REGISTER_GTW_CMD tloadRegisterCmd;
    static SENSOR_DATA tloadSensorData;       
    static METER_TICKET_DATA meterTcktData;
    static GPS_DATA gpsData;
    static ACCELEROMETER_DATA accelData;
    static LOAD_PLAN loadPlan;
    static FW_UPDATE_VERSION fwUpdateVer;
    static STATUS_PACKET status;    
    static GW_STATUS gwStatus;    
    
    /* static to take this off the stack */
    static uint8_t tempBuf[ENCODE_BUFFER_SIZE];
    
    static UINT16 MsgCounter[SERVER_MSG_LAST] ={0,0,0,0,0,0,0};
   
    int nbrBytes =0;

    memset( tempBuf, 0x00, sizeof(tempBuf));

    //UINT8  nbrPendingMeterTickets =0;    
    //UINT32 flashMemoryAddr =0;    
           
//serverMsg =SERVER_MSG_SENSOR;//SERVER_MSG_GPS;

    MsgCounter[serverMsg] ++;

    switch( serverMsg )
    {
        case SERVER_MSG_STARTUP:
            /* build our data packet */                                        
            GetTloadRegisterCmd(&tloadRegisterCmd);
            
            //sprintf( pTxBuf, "{\"temperature\":%d,\"outage\":%d,\"active\":%s}", (UINT32)smartloadSensorData.temp/10, (UINT32)(smartloadSensorData.outage*10), LedFlashTest? "true" : "false");
            //sprintf( pTxBuf, "{\"clientKeys\":\"attribute1,attribute2\",\"sharedkeys\":\"shared1,shared2\"}");
            //sprintf( pTxBuf, "{\"messages\":\"hello\"}");
            //sprintf( pTxBuf, "Hello MQTT Server");
            
        #ifdef GOOGLE_PROTOC_ENCODE_DECODE
            /* encode the structure */
            nbrBytes =EncodeTloadRegisterCmd( tempBuf, &tloadRegisterCmd );
                         
            /* test decoder */
            //DecodeTloadRegisterCmd( tempBuf, &tloadRegisterCmd );
        #else            
            /* for testing, we will actually send encoded startupData that is in pTxBuf from EncodeStartupData() */
            sprintf( pTxBuf, "Hello Server, GW id: %d msg#: %d\r\n", tloadRegisterCmd.std.gwId, MsgCounter[serverMsg]);
        #endif
            break;      
        case SERVER_MSG_SENSOR:                                  
             /* create a json packet */
            //sprintf( pTxBuf, "{\"temperature\":%d,\"outage\":%d,\"active\":%s}", (UINT32)smartloadSensorData.temp/10, (UINT32)(smartloadSensorData.outage*10), LedFlashTest? "true" : "false");            
            
            /* build our data packet */      
            GetTloadSensorData(&tloadSensorData);

        #ifdef GOOGLE_PROTOC_ENCODE_DECODE            
            /* encode the structure */
            nbrBytes =EncodeTloadSensorData( tempBuf, &tloadSensorData );
                        
            /* test decoder */
            //DecodeTloadSensorData( tempBuf, &tloadSensorData );            
        #else            
            /* for testing, we will actually send encoded smartloadSensorData that is in tempBuf from Encode???() */
            sprintf( pTxBuf, "\r\nSENSOR DATA: GW id: %d, time: %d, msg#: %d\r\ntemp: %d, outage: %d, I: %d(uA), volume: NA(l)\r\n\r\n", 
                                                                                                                 tloadSensorData.std.gwId, 
                                                                                                                 (UINT32)tloadSensorData.std.timeStamp,
                                                                                                                 MsgCounter[serverMsg], 
                                                                                                                 (UINT32)RtcGetEpochTime(), 
                                                                                                                 (UINT32)(tloadSensorData.outage*10),
                                                                                                                 (UINT32)(tloadSensorData.sensor1*1000) );//            
        #endif            
            break;
        case SERVER_MSG_METER_TCKT:                        
        #if 0          
            GetMeterTicketData(&meterTcktData);
            
            nbrPendingMeterTickets =GetNbrPendingMeterTickets();
            flashMemoryAddr = LogGetCurrentAddress(METER_TCKT_LOG) -(nbrPendingMeterTickets*0x100);
            DataFlashRead( flashMemoryAddr,  sizeof(meterTcktData.dataBuf), (uint8_t *)&meterTcktData.dataBuf );                      
            //DataFlashRead( flashMemoryAddr,  256, (uint8_t *)&meterTcktData.dataBuf );                      
            
            //DataFlashRead( flashMemoryAddr,  sizeof(tempBuf), (uint8_t *)&tempBuf );                      
            //memcpy( &meterTcktData.dataBuf, tempBuf, 100);
            
            /* test encoder */
            nbrBytes =EncodeMeterTcktData( tempBuf, &meterTcktData, sizeof(tempBuf) );

            /* test decoder (first clear dataBuf, then check if decoder works */
            memset( &meterTcktData.dataBuf, 0x00, sizeof(meterTcktData.dataBuf) );
            
            /* decode */
            DecodeMeterTcktData( tempBuf, &meterTcktData, sizeof(tempBuf) );            
               
            /* for testing, we will actually send encoded meterTcktData that is in tempBuf from EncodeMeterTcktData(), number of bytes to send 
              is in byteswritten field appears to be sizeof(STRUCT) +2
            */
     
            //sprintf( pTxBuf, "Meter Ticket Data, GW id: %d msg#: %d %s\r\n", meterTcktData.gwId, MsgCounter[serverMsg], meterTcktData.dataBuf);
            sprintf( pTxBuf, "GW id: %d msg#: %d\n%s\r\n", meterTcktData.gwId, MsgCounter[serverMsg], meterTcktData.dataBuf);
        #else
            GetMeterTicketData(&meterTcktData);   
         
            /* encode the structure */
            nbrBytes =EncodeMeterTicketData( tempBuf, &meterTcktData );
        #endif             
            break;
        case SERVER_MSG_GPS:
            /* build our data packet */          
            GetGpsData(&gpsData);

        #ifdef GOOGLE_PROTOC_ENCODE_DECODE
            /* encode the structure */
            nbrBytes =EncodeGpsData( tempBuf, &gpsData );
                         
            /* test decoder */
           // DecodeGpsData( tempBuf, &gpsData );
        #else    
            /* this is for testing, we will actually send encoded gpsData that is in tempBuf from EncodeGpsData() */
            sprintf( pTxBuf, "GPS Data, GW id: %d msg#: %d lat: %d lon: %d alt: %d status: %d\r\n", gpsData.std.gwId, MsgCounter[serverMsg], (int)gpsData.lat, (int)gpsData.lon,(int)gpsData.alt, gpsData.status);
        #endif
            break;        
        case SERVER_MSG_ACCEL:
            /* build our data packet */                                  
            GetTloadAccelData(&accelData);
            
        #ifdef GOOGLE_PROTOC_ENCODE_DECODE
            /* encode the structure */
            nbrBytes =EncodeTloadAccelData( tempBuf, &accelData );
            
            //ostream = pb_ostream_from_buffer(tempBuf, ENCODE_BUFFER_SIZE);    
            //pb_encode(&ostream, ACCELEROMETER_DATA_fields, &accelData);
            //nbrBytes =ostream.bytes_written;
                      
            /* test decoder */
           //DecodeTloadAccelData( tempBuf, &accelData );
        #endif            
            break;
        case SERVER_MSG_LOAD_PLAN:
            /* build our data packet */                                  
            GetTloadLoadPlan(&loadPlan);
            
        #ifdef GOOGLE_PROTOC_ENCODE_DECODE
            /* encode the structure */
            nbrBytes =EncodeTloadLoadPlan( tempBuf, &loadPlan );
                         
            /* test decoder */
            //DecodeTloadLoadPlan( tempBuf, &loadPlan );
        #endif            
            break;            
        case SERVER_MSG_FW_UPDATE:
            /* build our data packet */                                  
            GetFwUpdateVersion(&fwUpdateVer);
            
        #ifdef GOOGLE_PROTOC_ENCODE_DECODE
            /* encode the structure */
            nbrBytes =EncodeFwVersionGet( tempBuf, &fwUpdateVer );
                         
            /* test decoder */
            //DecodeTloadLoadPlan( tempBuf, &loadPlan );
        #endif                        
            break;    
        case SERVER_MSG_GW_STATUS:
            /* build our data packet */                                  
            GetGwStatusData(&gwStatus);  
            
        #ifdef GOOGLE_PROTOC_ENCODE_DECODE
            /* encode the structure */
            nbrBytes =EncodeGwStatus( tempBuf, &gwStatus );                         
        #endif             
            break;
        case SERVER_MSG_STATUS:
            /* build our data packet */                                  
            SetGwStatus(&status);
            break;
    }    

#ifdef GOOGLE_PROTOC_ENCODE_DECODE
    memcpy( pTxBuf, tempBuf, nbrBytes);
#endif
    
    return nbrBytes;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerGetGpsTxInterval
*|  Description: Modify server packet interval based on speed
*|  Retval: GPS server packet interval
*|----------------------------------------------------------------------------
*/
UINT32 ServerGetGpsTxInterval(void)
{
    float speed =GpsGetSpeed();    
    
         if(speed >100.0) return    15000;    
    else if(speed >60.0)  return    30000;    
    else if(speed >30.0)  return    60000;    
    else if(speed >5.0)   return   300000;    
    else                  return  3600000; //43200000;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: ServerGetSensorTxInterval
*|  Description: Modify server packet interval
*|----------------------------------------------------------------------------
*/
UINT32 ServerGetSensorTxInterval(void)
{
    GW_SETUP_STRUCT gwSetup;    
    
#if 1
    GwGetSetup( &gwSetup );
    
    if( gwSetup.remoteUpdateRate ==0 )
        gwSetup.remoteUpdateRate =10000;
    
    return gwSetup.remoteUpdateRate;
#else  
    if( LOADING )
        return 60000;
    else 
        return 3600000;
#endif    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerForceLogTransfer
*|  Description: Modify server packet interval
*|----------------------------------------------------------------------------
*/
void ServerForceLogTransfer(UINT8 logType, BOOL state)
{
    ClientServerMessaging.forceLogTransfer =state;        
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerForceLogTransfer
*|  Description: Modify server packet interval
*|----------------------------------------------------------------------------
*/
bool ServerGetForceLogState(UINT8 logType)
{
    return ClientServerMessaging.forceLogTransfer;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerSetStatus
*|  Description: 
*|----------------------------------------------------------------------------
*/
void ServerSetStatus(BOOL ack)
{
    ClientServerMessaging.status =ack;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: ServerCheckMessageToSend
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
ServerMsgsTypeEnum ServerCheckMessageToSend()
{
    ServerMsgsTypeEnum msg =SERVER_MSG_NONE;    

    if( ClientServerMessaging.systemStartup )
    {
        /* gateway startup message */
        msg =SERVER_MSG_STARTUP;
    }
        
    else if( ClientServerMessaging.status )
    {
        /* gateway ACK/NACK message */
        msg =SERVER_MSG_STATUS;      
    }
    
    else if( ClientServerMessaging.gwStatus )
    {
        /* gateway status information */
        msg =SERVER_MSG_GW_STATUS;            
    }

#if 1
    else if( (xTaskGetTickCount() -ClientServerMessaging.sensorTxTimer) > ServerGetSensorTxInterval() )
    {
        /* time for periodic message */
        msg =SERVER_MSG_SENSOR;          
    }                          
#endif     
    
#if 1    
    else if( (xTaskGetTickCount() -ClientServerMessaging.gpsTxTimer) > ServerGetGpsTxInterval() )
    {
        /* time for periodic message */
        msg =SERVER_MSG_GPS;         
    }
#endif
    
#if 0
    else if( (xTaskGetTickCount() -ClientServerMessaging.accelTxTimer) > ACCEL_PERIODIC_MESSAGE_TIMER )
    {
        /* time for periodic message */
        msg =SERVER_MSG_ACCEL;
    }       
#endif    

#if 0
    else if( GetNbrPendingMeterTickets() >0 )
    {
        /* meter ticket data pending */
        msg =SERVER_MSG_METER_TCKT;                
    }
#endif
    
#if 1    
    else if( (xTaskGetTickCount() -ClientServerMessaging.checkFwUpdateTimer) > FW_UDPATE_PERIODIC_MESSAGE_TIMER )
    {
        /* time for periodic message */
        msg =SERVER_MSG_FW_UPDATE;
    }
#endif
             
    return msg;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerCheckFtpMessageToSend
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
ServerMsgsTypeEnum ServerCheckFtpMessageToSend(void)
{
    ServerMsgsTypeEnum msg =SERVER_MSG_NONE;   
    
#if 0    
    if( GetNbrPendingMeterTickets() >0 )
    {
        /* meter ticket data pending */
        msg =SERVER_MSG_METER_TCKT;                
    }
#endif
    
#if 1    
    if( ClientServerMessaging.internalLogUpdate )
    {
        /* time for message */
        msg =SERVER_MSG_INT_LOG_UPDATE;
    }      
#endif  

#if 1    
    else if(ClientServerMessaging.fwUpdate )
    {
        msg =SERVER_MSG_FW_DOWNLOAD;      
    }
#endif    
    
    return msg;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerClearMessageToSend
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
void ServerClearMessageToSend(ServerMsgsTypeEnum serverMsg)
{
    UINT8 nbrPendingMeterTickets =0;
    
    switch(serverMsg)      
    {     
        case SERVER_MSG_STARTUP:
            ClientServerMessaging.systemStartup =FALSE;
            break;           
        case SERVER_MSG_STATUS:
            ClientServerMessaging.status =FALSE;
            break;  
        case SERVER_MSG_GW_STATUS:
            ClientServerMessaging.gwStatus =FALSE;
            break;            
        case SERVER_MSG_METER_TCKT:
            /* we just sent a meter ticket, and it was acknowledged by server */
            nbrPendingMeterTickets =GetNbrPendingMeterTickets();
                    
            /* remove */
            nbrPendingMeterTickets --;                    
            SetNbrPendingMeterTickets(nbrPendingMeterTickets);
            break;
        case SERVER_MSG_SENSOR:            
            ClientServerMessaging.sensorTxTimer =xTaskGetTickCount();                              
            break;            
        case SERVER_MSG_GPS: 
            ClientServerMessaging.gpsTxTimer =xTaskGetTickCount();                              
            break;               
        case SERVER_MSG_ACCEL:
            ClientServerMessaging.accelTxTimer =xTaskGetTickCount(); 
            break;             
        case SERVER_MSG_FW_UPDATE:
            ClientServerMessaging.checkFwUpdateTimer =xTaskGetTickCount(); 
            //ClientServerMessaging.checkFwUpdate =FALSE;
            break;                        
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: ServerClearFtpMessageToSend
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
void ServerClearFtpMessageToSend(ServerMsgsTypeEnum serverMsg)
{
    //UINT8 nbrPendingMeterTickets =0;
    
    switch(serverMsg)      
    {
#if 0      
        case SERVER_MSG_METER_TCKT:
            /* we just sent a meter ticket, and it was acknowledged by server */
            nbrPendingMeterTickets =GetNbrPendingMeterTickets();
                    
            /* remove */
            nbrPendingMeterTickets --;                    
            SetNbrPendingMeterTickets(nbrPendingMeterTickets);
            break;      
#endif            
        case SERVER_MSG_INT_LOG_UPDATE:
            ClientServerMessaging.internalLogUpdate =FALSE;
            break;       
        case SERVER_MSG_FW_DOWNLOAD:
            ClientServerMessaging.fwUpdate =FALSE;
            break;             
    }    
}

    
void ServerQueInternlLogUpdate(void)
{
    ClientServerMessaging.internalLogUpdate =TRUE;                      
}

void ServerQueFwUpdate(void)
{
    ClientServerMessaging.fwUpdate =TRUE;                      
}

    
/*
*|----------------------------------------------------------------------------
*|  Routine: GetMeterTicketLog
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/    
UINT32 GetMeterTicketFromLog(void)
{
    int nbrPendingMeterTickets;
    int nbrTckstEndOfMemory =0;
    
    LOG_MEMORY_ADDRESS meterTcktLogMemAddr;
    
    nbrPendingMeterTickets =GetNbrPendingMeterTickets();
        
    meterTcktLogMemAddr.start =( LogGetCurrentAddress(METER_TCKT_LOG) -(nbrPendingMeterTickets*0x100) );                      
        
    if( meterTcktLogMemAddr.start <LogGetStartAddress(METER_TCKT_LOG) )
    {
        /* need to get meter data at end of log */
        nbrTckstEndOfMemory =(LogGetStartAddress(METER_TCKT_LOG) -meterTcktLogMemAddr.start)/256;
            
        meterTcktLogMemAddr.start =LogGetEndAddress(METER_TCKT_LOG) -(nbrTckstEndOfMemory*0x100);
    }
        
    meterTcktLogMemAddr.end =meterTcktLogMemAddr.start +0x100;  
        
    return meterTcktLogMemAddr.start;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: BuildJsonPacket
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
void BuildJsonPacket( char *pBuf, int n, ...)
{    
#if 0 
  int i;
  static double val =0; 
  
  //printf ("Printing floats:");
  strcpy(pBuf, "");
  
  va_list vl;
  va_start(vl,n);
  for (i=0;i<n;i++)
  {
    val=va_arg(vl,double);
//    printf (" [%.2f]",val);
    strcat( pBuf, "{\"temperature\":35, \"humidity\":65.0, \"active\": false,\"isCool\": true}");
  }
  va_end(vl);

  strcpy( pBuf, "{\"temperature\":35, \"humidity\":65.0, \"active\": false,\"isCool\": true}");
#endif  
}



