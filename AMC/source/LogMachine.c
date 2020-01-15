/** C Source ******************************************************************
*
* NAME      LogMachine.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "types.h"
#include "string.h"

#include "dataFlashFd.h"

#include "AdcFd.h"

#include "rtcHi.h"
#include "gpioHi.h" /* for field test LED control */

#include "GatewayConfig.h"
#include "TLoadMachine.h"
#include "GpsMachine.h"
#include "CellularMachine.h"
#include "PlcMachine.h"
#include "LogMachine.h"

#include "semphr.h"

#define LOG_SEMAPHORE_ENABLE

/** Local Constants and Types *************************************************/

typedef enum
{
    LOG_IDLE_STATE =0,
    LOG_RUN_STATE,
    LOG_CONFIRM_STATE,   
    LOG_WRITE_FAIL_STATE,
    LOG_ERASE_STATE,
    LOG_ERASE_ALL_STATE,
    
    LOG_LAST_STATE
      
}LogStatesEnum;


typedef struct
{
    UINT32 wrapAddr;
    UINT32 address;    /* current log address of the log */ 
    
    UINT8 logReady;   
    UINT8 logErase;
    
    UINT32 logEnd;
    UINT32 logStart;

    UINT16 packetSize; /* sizeof log packet */
    UINT16 sizeofLog;  /* actual log size, may be multiple pages */
    
    GwLogsEnum logType;

    BOOL (*pLogFunction)(UINT8); 

    LogStatesEnum machState;
    
}LOG_DATA;

typedef struct
{
    UINT32 currentAddr;
    UINT32 wrapAddr;
//    UINT16 nbrRecords;
 
}LOG_INFO;


typedef struct
{
    UINT32 epoch;

//    UINT16 adcRawData[8];   
    
//    float iSense;
    float iSenseAvg;
    float range;
//    float gwTemp;

//    float volume;
    
    UINT8 rtu[2]; /* remain to use (spares) */

    UINT16 checksum;
}DIAGNOSTIC_DATA;

typedef struct
{
    UINT32 epoch;
   
    float lat;
    float lon;

    UINT8 cellRssi;
    UINT8 gpsStatus; 
    
    UINT16 checksum;
    
}FIELD_SURVEY_DIAGNOSTIC_DATA;

typedef struct
{
    UINT32 epoch;
    UINT32 sysTime;
       
    UINT16 outage;
    UINT16 flowRate;
    UINT16 range;
    UINT16 reliability;
    UINT16 sensTemp;    
    UINT16 density;
    
    UINT8 rtu[10];
    
    UINT16 checksum;
    
}FILL_DATA;

LOG_INFO LogInfo[LAST_LOG];

DIAGNOSTIC_DATA DiagnosticData;
FIELD_SURVEY_DIAGNOSTIC_DATA FieldSurveyDiagData;
FILL_DATA FillData;

/*** Local Variable Declarations ***********************************************/
UINT32 LogTimer=0;
SemaphoreHandle_t xLogAccessMutex;

LOG_DATA MeterTcktLogData;
LOG_DATA DiagnosticLogData;
LOG_DATA FieldSurveyDiagLogData;
LOG_DATA FillLogData;

/*** Local Function Prototypes *************************************************/
//void LogMachineTask(void * pvParameters);

BOOL LogMeterTcktData(UINT8 logType);
BOOL LogDiagnosticData(UINT8 logType);
BOOL LogFieldSurveyDiagData(UINT8 logType);
BOOL LogFillData(UINT8 logType);

void LogReadMemoryPointers(void);
void LogWriteMemoryPointers(int);

UINT32 CheckNextSector(UINT32 addr);

LOG_DATA *LogGetStructure(GwLogsEnum log);

/*** Functions *****************************************************************/

/*
*|----------------------------------------------------------------------------
*|  Routine: InitLog
*|  Description: Init a specific log
*|  Retval: void
*|----------------------------------------------------------------------------
*/
void InitLog
(
    LOG_DATA *pLogData,
    GwLogsEnum logType
)
{
//    memset( pLogData, 0x0000, sizeof(LOG_DATA) );

    pLogData->logType =logType;            
    pLogData->machState =LOG_IDLE_STATE;
   
    switch( logType )
    {
        case METER_TCKT_LOG:
            if( pLogData->address ==0xffffffff )
                pLogData->address  =METER_TCKT_LOG_ADDR_START; // first log block

            if( pLogData->address <METER_TCKT_LOG_ADDR_START )
                pLogData->wrapAddr =METER_TCKT_LOG_ADDR_END;
            
            if( pLogData->wrapAddr ==0xffffffff )
                pLogData->address  =0; 
                
            pLogData->logStart =METER_TCKT_LOG_ADDR_START;
            pLogData->logEnd   =METER_TCKT_LOG_ADDR_END;

            pLogData->packetSize =METER_TCKT_LOG_PACKET_SIZE;

            pLogData->sizeofLog =8192; // two sectors with current memory map
            pLogData->pLogFunction =LogMeterTcktData;
            break;              
        case DIAGNOSTIC_LOG:
            if( pLogData->address ==0xffffffff )
                pLogData->address  =DIAGNOSTIC_LOG_ADDR_START; // first log block
            
            if( pLogData->address <DIAGNOSTIC_LOG_ADDR_START )
                pLogData->address =DIAGNOSTIC_LOG_ADDR_END;

            if( pLogData->wrapAddr ==0xffffffff )
                pLogData->wrapAddr  =0; 
            
            pLogData->logStart =DIAGNOSTIC_LOG_ADDR_START;
            pLogData->logEnd   =DIAGNOSTIC_LOG_ADDR_END;

            pLogData->packetSize =DIAGNOSTIC_LOG_PACKET_SIZE;

            pLogData->sizeofLog =DIAGNOSTIC_LOG_PACKET_SIZE; 
            pLogData->pLogFunction =LogDiagnosticData;          
            break;              
        case FIELD_SURVEY_DIAG_LOG:
            if( pLogData->address ==0xffffffff )
                pLogData->address  =FIELD_SURVEY_DIAG_START; // first log block
            
            if( pLogData->address <FIELD_SURVEY_DIAG_START )
                pLogData->address =FIELD_SURVEY_DIAG_END;

            if( pLogData->wrapAddr ==0xffffffff )
                pLogData->wrapAddr  =0; 
            
            pLogData->logStart =FIELD_SURVEY_DIAG_START;
            pLogData->logEnd   =FIELD_SURVEY_DIAG_END;

            pLogData->packetSize =FIELD_SURVEY_DIAG_LOG_PACKET_SIZE;

            pLogData->sizeofLog =FIELD_SURVEY_DIAG_LOG_PACKET_SIZE; 
            pLogData->pLogFunction =LogFieldSurveyDiagData;          
            break;      
        case PLC_LOG:
            if( pLogData->address ==0xffffffff )
                pLogData->address  =PLC_LOG_START; // first log block
            
            if( pLogData->address <PLC_LOG_START )
                pLogData->address =PLC_LOG_END;

            if( pLogData->wrapAddr ==0xffffffff )
                pLogData->wrapAddr  =0; 
            
            pLogData->logStart =PLC_LOG_START;
            pLogData->logEnd   =PLC_LOG_END;

            pLogData->packetSize =PLC_LOG_PACKET_SIZE;

            pLogData->sizeofLog =PLC_LOG_PACKET_SIZE; 
            pLogData->pLogFunction =LogFillData;
            break;            
    }
    
    #if(LOG_LAST_LOG_END >LOG_ADDR_END)
       #error "LOGS HAVE EXCEEDED ALLOCATED MEMORY"
    #endif
 
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogMachineInit
*|  Description: Init the system logs
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL LogMachineInit(void)
{   
    BaseType_t xReturn;
    
    LogReadMemoryPointers();

    InitLog(&MeterTcktLogData, METER_TCKT_LOG);
    InitLog(&DiagnosticLogData, DIAGNOSTIC_LOG);
    InitLog(&FieldSurveyDiagLogData, FIELD_SURVEY_DIAG_LOG);
    InitLog(&FillLogData, PLC_LOG);

    /* place here so we do not unnecessarily write the flash memory for every log init */
    LogWriteMemoryPointers(0);

//static UINT8 tempBuf[256];    
//DataFlashRead(0xc00000, sizeof(tempBuf), tempBuf);        
//DataFlashRead(0xc00000, sizeof(tempBuf), tempBuf);        

#ifdef LOG_SEMAPHORE_ENABLE
    xLogAccessMutex =xSemaphoreCreateMutex();
#endif
    
    /* spawn the log machine task */
    xReturn =xTaskCreate( LogMachineTask, 
                          "Log Machine Task", 
                          configMINIMAL_STACK_SIZE, 
                          NULL, 
                          (tskIDLE_PRIORITY + 1UL), 
                          ( TaskHandle_t * )NULL);    
    
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
*|  Module:
*|  Routine: LogStateProcess
*|  Description: move the calling machines state, and reset timeoutTimer
*|----------------------------------------------------------------------------
*/
void LogStateProcess
(
    LOG_DATA *pLogData,
    LogStatesEnum state
)
{
    pLogData->machState =state;

} // end LogStateProcess()

/*
*|----------------------------------------------------------------------------
*|  Module:
*|  Routine: LogManageAddress
*|  Description: manage log address (wrap, end of memory, etc)
*|----------------------------------------------------------------------------
*/
void LogManageAddress
(
    LOG_DATA *pLogData
)
{
    BOOL eraseSector =FALSE;
    static UINT16 eraseSectorNbr =0;
    UINT16 j;

    if( ( (pLogData->address+pLogData->sizeofLog) %DATAFLASH_SIZE_SECTOR_BYTES )== 0 )
    { 
        /* approaching sector boundary, erase sector ahead of us */
        eraseSectorNbr =(pLogData->address+pLogData->sizeofLog) /DATAFLASH_SIZE_SECTOR_BYTES;

        eraseSector =TRUE;
    }

    if( (pLogData->address+pLogData->sizeofLog) >=pLogData->logEnd )
    {
        /* approaching end of memory, erase log start sector */
    	eraseSectorNbr = pLogData->logStart/DATAFLASH_SIZE_SECTOR_BYTES;

    	eraseSector =TRUE;
    }

    if( pLogData->address >=pLogData->logEnd )
    {
        /* end of memory, wrap to beginning */
        pLogData->address  =pLogData->logStart;
        pLogData->wrapAddr =pLogData->logStart+pLogData->sizeofLog;
        
        eraseSectorNbr =(pLogData->address) /DATAFLASH_SIZE_SECTOR_BYTES;

        eraseSector =TRUE;
    }  
              
    if( eraseSector )
    {     
        for(j=0; j<(pLogData->sizeofLog/DATAFLASH_SIZE_SECTOR_BYTES +1); j++)
        {
            DataFlashEraseSector(eraseSectorNbr+j);
        }
    }

} // end LogManageAddress()


/*
*|----------------------------------------------------------------------------
*|  Routine: LogFindEndOfMemory
*|  Description: Find the current memory address of this log
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogFindEndOfMemory
(
    LOG_DATA *pLogData
)
{
#if 0  
    //return;
#else    
    UINT8 aBuf[32];
    UINT8 eraseValue[32];
    UINT8 nbrSeqBytes =0;
    //UINT32 nextSectorAddr =0;
    
    memset(eraseValue, 0xff, sizeof(eraseValue));

    pLogData->address =pLogData->logStart;    

    // number of sequential bytes to determine if page is erased
    nbrSeqBytes =16;//pLogData->packetSize;

    while(1)
    {
    	if( pLogData->address >pLogData->logEnd )
    	{
            /* end of memory was not found, ??? */
            return;
    	}

        DataFlashRead(pLogData->address, nbrSeqBytes, aBuf);
        
        if( memcmp(&aBuf, &eraseValue, nbrSeqBytes) !=0 )    
        {
            /* memory is not erased, check end of the sector */
            pLogData->address +=(DATAFLASH_SIZE_SECTOR_BYTES -pLogData->packetSize);

            DataFlashRead(pLogData->address, nbrSeqBytes, aBuf);

            if( memcmp(&aBuf, &eraseValue, nbrSeqBytes) !=0 )
            {
                /* end of sector is not erased, continue on to next sector */
                pLogData->address +=pLogData->packetSize;
                continue;
            }
            else
            {
                /* end of sector is erased, end of memory must be in this sector */
                break;
            }
        }
        else
        {
            /* start of sector is erased, this is end of memory     
               now check the next block, if it is erased, we have not wrapped yet, 
               if data is present, then memory wrap has occured */
            pLogData->wrapAddr =CheckNextSector( (pLogData->address +DATAFLASH_SIZE_SECTOR_BYTES) );            
            return;
        }
    }

    while(1)
    {
        DataFlashRead(pLogData->address, nbrSeqBytes, aBuf);

        if( memcmp(&aBuf, &eraseValue, nbrSeqBytes) ==0 )
        {
            /* erased, continue reading backwards through sector */
            pLogData->address -=pLogData->packetSize;//DATAFLASH_SIZE_PAGE_BYTES;
        }
        else
        {
            /* not erased, move forward one page, we know that location is erased, and it is the 
               end of log memory */
            pLogData->address +=pLogData->packetSize;//DATAFLASH_SIZE_PAGE_BYTES;
            
            /* now check the next block, if it is erased, we have not wrapped yet, 
               if data is present, then memory wrap has occured */
            pLogData->wrapAddr =CheckNextSector( (pLogData->address +DATAFLASH_SIZE_SECTOR_BYTES) );           
            break;
        }        
    }
#endif        
}

UINT32 CheckNextSector(UINT32 addr)
{           
    UINT8 aBuf[32];
    UINT8 eraseValue[32];
    UINT8 nbrSeqBytes =0;    
    
    nbrSeqBytes =16;
    memset(eraseValue, 0xff, sizeof(eraseValue));
        
    DataFlashRead(addr, nbrSeqBytes, aBuf);
            
    if( memcmp(&aBuf, &eraseValue, nbrSeqBytes) ==0 )
    {              
        /* erased */
        return 0; 
    }
    else
    {
        /* not erased, we have wrapped */
        return addr;
    }  
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogMachine
*|  Description: Log machine state handler
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL LogMachine
(
    LOG_DATA *pLogData
)
{     
    UINT16 endBlock;
    static UINT16 block =0;
   
    if( pLogData ==NULL )
        return FALSE;
#ifdef LOG_SEMAPHORE_ENABLE   
    if( xLogAccessMutex != NULL )
#endif      
    {
#ifdef LOG_SEMAPHORE_ENABLE     
        if( xSemaphoreTake( xLogAccessMutex, ( TickType_t ) 0 ) == pdTRUE ) 
#endif          
        {
            /* We were able to obtain the semaphore and can now access the
              shared resource. */
    
            switch( pLogData->machState )
            {
                case LOG_IDLE_STATE: 
                    /* find end of log memory for this logs allocated range */
                    LogFindEndOfMemory(pLogData);

                    /* go to log run state */
                    LogStateProcess(pLogData, LOG_RUN_STATE);
                    break;
                case LOG_RUN_STATE: 
                    if( pLogData->logReady )
                    {
                        /* we have data to log, log it now */
                        if( pLogData->pLogFunction !=NULL )
                        {
                            LogManageAddress(pLogData);

                            if( pLogData->pLogFunction(pLogData->logType) )
                            {
                                pLogData->logReady =FALSE;

                                /* goto next state */
                                LogStateProcess(pLogData, LOG_CONFIRM_STATE);                        
                            }
                            else
                            {
                                /* fail, page is not erased, try next section */
                                pLogData->address +=pLogData->packetSize;
                                LogStateProcess(pLogData, LOG_WRITE_FAIL_STATE );
                                break;
                            }
                        }
                    }
                    if( pLogData->logErase ==TRUE )
                    {
                        block =(pLogData->logStart/DATAFLASH_SIZE_BLOCK_BYTES);
                        LogStateProcess(pLogData, LOG_ERASE_STATE);                  
                    }
                    break;
                case LOG_CONFIRM_STATE: /* read data back and confirm integrity */
                    /* read back our data and check CRC, if available */
                    // DATAFLASH_DataRead(pLogData->info.address, aBuf, sizeof(aBuf());
                    //pLogData->address +=pLogData->packetSize;                       
                    LogWriteMemoryPointers(0);

                    LogStateProcess(pLogData, LOG_RUN_STATE); 
                    break;
                case LOG_WRITE_FAIL_STATE:
                    break;
                case LOG_ERASE_STATE:            
                    endBlock =(pLogData->logEnd/DATAFLASH_SIZE_BLOCK_BYTES);

                    DataFlashEraseBlock(block);
                    
                    if( ++block >=endBlock)
                    {
                        /* flag letting software know erase is complete */
                        pLogData->logErase =FALSE;
                        pLogData->address =0xffffffff;

                        InitLog(pLogData, pLogData->logType);

                        LogWriteMemoryPointers(0);
                                
                        /* erase complete, go idle */
                        LogStateProcess(pLogData, LOG_IDLE_STATE); 
                    }
                    break;
                case LOG_LAST_STATE:
                    break;
            }

#ifdef LOG_SEMAPHORE_ENABLE                 
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xLogAccessMutex );    
#endif            
    
        }        
#ifdef LOG_SEMAPHORE_ENABLE            
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */          
        }
#endif        
    }
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: 
*|  Description: Populate this log
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
BOOL LogMeterTcktData(UINT8 logType)
{   
    LOG_DATA *pLogData;
    
    pLogData =&MeterTcktLogData;
       
    if( !DataFlashWrite( pLogData->address, sizeof(LogBuffer), LogBuffer ) )     
    {
        return FALSE;
    }

    pLogData->address +=pLogData->packetSize;
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogDiagnosticData
*|  Description: Populate this log
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
BOOL LogDiagnosticData(UINT8 logType)
{   
    UINT16 j;
    char *p;

//#define FULL_LOG   
    
    #ifdef FULL_LOG    
    ADC_READINGS_STRUCT adcData;
    #endif
    TLOAD_TANK_DATA tLoadTankData;
    
    DiagnosticData.epoch =RtcGetEpochTime();  
    
    TLoadGetData(&tLoadTankData);

#ifdef FULL_LOG      
    AdcGetData(&adcData);
                     
    /* eventually we will convert lpgLevel to real value in AdcFd, for now 
       we want to retain the current reading so do conversion here */
    DiagnosticData.iSenseAvg =adcData.slLevel;    
 //   DiagnosticData.iSenseAvg =GetSloadLevelAverage();          
    DiagnosticData.range =adcData.fluidLevel;
#else   
    DiagnosticData.iSenseAvg =tLoadTankData.lvlSensor.current;
    DiagnosticData.range =tLoadTankData.lvlSensor.measuredRange;     
#endif

    p=(char *)&DiagnosticData;

    DiagnosticData.checksum =0;

    for(j=0; j<(sizeof(DiagnosticData)-sizeof(DiagnosticData.checksum)); j++)
    {
        DiagnosticData.checksum += (*p++);
    }

    if( !DataFlashWrite( DiagnosticLogData.address, sizeof(DiagnosticData), (UINT8 *)&DiagnosticData ) ) 
    {
        return FALSE;    
    }

//memset(&DiagnosticData, 0x00, sizeof(DiagnosticData));
//DataFlashRead(DiagnosticLogData.address, sizeof(DiagnosticData), (UINT8 *)&DiagnosticData);
    
    DiagnosticLogData.address +=sizeof(DiagnosticData);

    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogFieldSurveyDiagData
*|  Description: Populate this log
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
BOOL LogFieldSurveyDiagData(UINT8 logType)
{    
    UINT16 j;
    char *p;
    
    FieldSurveyDiagData.epoch =RtcGetEpochTime();  
    FieldSurveyDiagData.cellRssi =CellularGetRssi();
    FieldSurveyDiagData.gpsStatus =GpsGetStatus();
    FieldSurveyDiagData.lat =GpsGetLatitude();
    FieldSurveyDiagData.lon =GpsGetLongitude();
    
    p=(char *)&FieldSurveyDiagData;

    FieldSurveyDiagData.checksum =0;

    for(j=0; j<(sizeof(FieldSurveyDiagData)-sizeof(FieldSurveyDiagData.checksum)); j++)
    {
        FieldSurveyDiagData.checksum += (*p++);
    }    
    
    if( !DataFlashWrite( FieldSurveyDiagLogData.address, sizeof(FieldSurveyDiagData), (UINT8 *)&FieldSurveyDiagData ) ) 
    {
        return FALSE;    
    }
   
    FieldSurveyDiagLogData.address +=sizeof(FieldSurveyDiagData);    
    
/* just put GPS status LED control here for now */  
if( FieldSurveyDiagData.gpsStatus )        
    XBEE_ENABLE;
else
    XBEE_DISABLE;    

    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: LogFillData
*|  Description: Populate this log
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
BOOL LogFillData(UINT8 logType)
{    
    UINT16 j;
    char *p;
   
    TickType_t xTicks;
    xTicks=xTaskGetTickCount();    
    
    //PLC_TRUCK_FLOW_DATA plcTruckFlowData;
    TLOAD_TANK_DATA tloadTankData;

    //PlcGetData(&plcTruckFlowData);
    TLoadGetData(&tloadTankData);
    
    /* clear this struct */
    memset(&FillData, 0x00, sizeof(FillData));
    
    FillData.epoch       =RtcGetEpochTime();  
    FillData.sysTime     =xTicks;
    FillData.outage      =(UINT16)(tloadTankData.lvlSensor.current*1000);     //vega reliability reading
    FillData.flowRate    =(UINT16)(tloadTankData.plcSensor.rate*100); // PLC flow rate reading
    FillData.range       =(UINT16)(tloadTankData.lvlSensor.measuredRange *1000); //vega range reading (mm)
    FillData.reliability =(UINT16)(tloadTankData.lvlSensor.reliability*10);     //vega reliability reading
    FillData.sensTemp    =(UINT16)(tloadTankData.lvlSensor.temperature*10);    
    FillData.density       =(UINT16)(tloadTankData.plcSensor.density*10); // PLC density reading
    
    p=(char *)&FillData;

    FillData.checksum =0;

    for(j=0; j<(sizeof(FillData)-sizeof(FillData.checksum)); j++)
    {
        FillData.checksum += (*p++);
    }    
    
    if( !DataFlashWrite( FillLogData.address, sizeof(FillData), (UINT8 *)&FillData ) ) 
    {
        return FALSE;    
    }
   
    FillLogData.address +=sizeof(FillData);    
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogDataStart
*|  Description: Start this log
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
void LogDataStart(GwLogsEnum log)
{
    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    /* flag log machine to log this specific log */
    pLogData->logReady =TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogDataErase
*|  Description: Erase this log
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
void LogDataErase(GwLogsEnum log)
{
    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    /* flag log machine to erase this specific log */
    pLogData->logErase =TRUE;    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: LogGetStartAddress
*|  Description:
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
UINT32 LogGetStartAddress(GwLogsEnum log)
{
    UINT32 addr =0;

    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    addr =pLogData->logStart;

    return addr;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogGetCurrentAddress
*|  Description:
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
UINT32 LogGetCurrentAddress(GwLogsEnum log)
{
    UINT32 addr =0;

    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    addr =pLogData->address;

    return addr;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogGetEndAddress
*|  Description:
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
UINT32 LogGetEndAddress(GwLogsEnum log)
{
    UINT32 addr =0;

    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    addr =pLogData->logEnd;

    return addr;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogGetWrapAddress
*|  Description:
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
UINT32 LogGetWrapAddress(GwLogsEnum log)
{
    UINT32 addr =0;

    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    addr =pLogData->wrapAddr;

    return addr;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogGetEraseStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT8 LogGetEraseStatus(GwLogsEnum log)
{
    UINT8 logEraseStatus =0;
    LOG_DATA *pLogData;

    pLogData =LogGetStructure(log);

    logEraseStatus =pLogData->logErase;

    return logEraseStatus;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogGetStructure
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
LOG_DATA *LogGetStructure(GwLogsEnum log)
{
    LOG_DATA *pLogData;

    switch(log)
    {
        case METER_TCKT_LOG:
            pLogData =&MeterTcktLogData;
            break;
        case DIAGNOSTIC_LOG:
            pLogData =&DiagnosticLogData;
            break;            
        case FIELD_SURVEY_DIAG_LOG:
            pLogData =&FieldSurveyDiagLogData;
            break;             
        case PLC_LOG:
            pLogData =&FillLogData;
            break;                         
        default:
            /* unknown/uninitialized log */
            pLogData =NULL;
            break;
    }    

    return pLogData;
}



/*
*|----------------------------------------------------------------------------
*|  Routine: LogDataEraseAll
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL LogDataEraseAll(void)
{
    GwLogsEnum logNbr;
    static UINT16 block =0;
    
    LOG_DATA *pLogData;

#ifdef LOG_SEMAPHORE_ENABLE         
    if( xLogAccessMutex != NULL )
#endif      
    {
#ifdef LOG_SEMAPHORE_ENABLE           
        if( xSemaphoreTake( xLogAccessMutex, ( TickType_t ) 10 ) == pdTRUE ) 
#endif          
        {          
            for(block=LOG_ADDR_START/DATAFLASH_SIZE_BLOCK_BYTES; block<LOG_ADDR_END/DATAFLASH_SIZE_BLOCK_BYTES; block++) 
            {
        //memset(tempBuf, 0xaa, sizeof(tempBuf));      
        //DataFlashWrite(block*65536, sizeof(tempBuf), tempBuf);              
                DataFlashEraseBlock(block);
            
        //memset(tempBuf, 0x00, sizeof(tempBuf));              
        //DataFlashRead(block*65536, sizeof(tempBuf), tempBuf);        
            }          

            LogReadMemoryPointers();
               
            for(logNbr =METER_TCKT_LOG; logNbr<LAST_LOG; logNbr++)
            {   
                pLogData =LogGetStructure(logNbr);

                InitLog(pLogData, logNbr);
            }

            LogWriteMemoryPointers(1);
     
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xLogAccessMutex );        
            
            return TRUE;
        }
#ifdef LOG_SEMAPHORE_ENABLE             
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            return FALSE;
        }
#endif        
    }
#ifdef LOG_SEMAPHORE_ENABLE      
    else
        return FALSE;
#endif    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: LogStopAllLogs
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL LogStopAllLogs(void)
{
    BOOL status;
    
#ifdef LOG_SEMAPHORE_ENABLE         
    if( xLogAccessMutex != NULL )
    {
        if( xSemaphoreTake( xLogAccessMutex, ( TickType_t ) 10 ) == pdTRUE ) 
        {
            status =TRUE;
        }
        else
            status =FALSE;
    }
#else
    status =TRUE;
#endif
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogStartAllLogs
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogStartAllLogs(void)
{
#ifdef LOG_SEMAPHORE_ENABLE         
    if( xLogAccessMutex != NULL )
    {
        xSemaphoreGive( xLogAccessMutex );     
    }
#endif    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogWriteMemoryPointers
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogWriteMemoryPointers(int erase)
{
#if 1
    return;
#else        
    GwLogsEnum logNbr;
    static UINT32 address =LOG_ADDR_TABLE;
    LOG_DATA *pLogData;
    
    if( erase )
        address =LOG_ADDR_TABLE;
    
    for(logNbr =METER_TCKT_LOG; logNbr<LAST_LOG; logNbr++)
    {
        pLogData =LogGetStructure(logNbr);

        //LogCurrentAddr[logNbr] =pLogData->address;
        LogInfo[logNbr].currentAddr =pLogData->address;       
        LogInfo[logNbr].wrapAddr =pLogData->wrapAddr;
    }
    
    if( address ==LOG_ADDR_TABLE)
    {
        /* erase this sector */
        DataFlashEraseSector(address/DATAFLASH_SIZE_SECTOR_BYTES);
    }

    //DataFlashWrite( address,  sizeof(LogCurrentAddr), (UINT8 *)&LogCurrentAddr);
    DataFlashWrite( address,  sizeof(LogInfo), (UINT8 *)&LogInfo);
    
    address +=sizeof(LogInfo);
    
    if( address ==(LOG_ADDR_TABLE + LOG_ADDR_TABLE_SIZE) )
    {
        address =LOG_ADDR_TABLE;
    }   
#endif    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogReadMemoryPointers
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogReadMemoryPointers(void)
{
#if 1
    return;
#else    
    GwLogsEnum logNbr;
    UINT32 address =LOG_ADDR_TABLE;
    LOG_DATA *pLogData;
    
    memset(LogInfo, 0x00, sizeof(LogInfo) );
    
    while(LogInfo[0].currentAddr !=0xffffffff )
    {
        DataFlashRead( address,  sizeof(LogInfo), (UINT8 *)&LogInfo );
        
        address +=sizeof(LogInfo);
    }
    
    address -=(2*sizeof(LogInfo));
    DataFlashRead( address,  sizeof(LogInfo), (UINT8 *)&LogInfo );
    
    for(logNbr =METER_TCKT_LOG; logNbr<LAST_LOG; logNbr++)
    {
        pLogData =LogGetStructure(logNbr);

        pLogData->address =LogInfo[logNbr].currentAddr;
        pLogData->wrapAddr =LogInfo[logNbr].wrapAddr;
    }
#endif    
}



/*
*|----------------------------------------------------------------------------
*|  Routine: LogMachineTask
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogMachineTask(void * pvParameters)
{
    GwLogsEnum logNbr;

    LOG_DATA *pLogData;

    for(;;)
    {
        for(logNbr =METER_TCKT_LOG; logNbr<LAST_LOG; logNbr++)
        {
            pLogData =LogGetStructure(logNbr);

            /* run the log machine on a RTOS task */
            LogMachine(pLogData);
        }    
    }
}


/*
*|----------------------------------------------------------------------------
*|  Routine: LogCheckDiagnosticLogTime
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogCheckDiagnosticLogTime(void)
{
    //static UINT32 LogTimer=0;
    
    TickType_t xTicks=xTaskGetTickCount();
   
    if( (xTicks - LogTimer) >1000 )
    {      
        LogTimer =xTicks;
        LogDataStart(DIAGNOSTIC_LOG);      
    }
}


/*
*|----------------------------------------------------------------------------
*|  Routine: LogCheckFieldSurveyDiagLogTime
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogCheckFieldSurveyDiagLogTime(void)
{
    //static UINT32 LogTimer=0;

    TickType_t xTicks=xTaskGetTickCount();
   
    if( (xTicks - LogTimer) >1000 )
    {      
        LogTimer =xTicks;
        LogDataStart(FIELD_SURVEY_DIAG_LOG);      
    }
}


/*
*|----------------------------------------------------------------------------
*|  Routine: LogCheckFieldSurveyDiagLogTime
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogCheckFillLogTime(void)
{
    TickType_t xTicks=xTaskGetTickCount();
   
    if( (xTicks - LogTimer) >1000 )
    {      
        LogTimer =xTicks;
        LogDataStart(PLC_LOG);      
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LogResetLogtimer
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LogResetLogtimer(void)
{
    TickType_t xTicks=xTaskGetTickCount();
    
    LogTimer =xTicks;    
}






