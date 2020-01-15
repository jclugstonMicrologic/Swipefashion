/** C Source ******************************************************************
*
* NAME      PLCMachine.c
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
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "ModbusFd.h"
#include "PLCMachine.h"
#include "LogMachine.h"
#include "TloadMachine.h"
#include "ServerPacket.h"
#include "PowerManagement.h"

#include "rtcHi.h"
#include "gpioHi.h"
#include "lcdHi.h"

typedef enum
{
    PLC_INIT_STATE =0,
    PLC_IDLE_STATE,    
    
    PLC_LOADING_STATE,     
    PLC_LOADING_COMPLETE_STATE,
      
    PLC_WAIT_SLAVE_POLL_STATE,
    PLC_WAIT_SLAVE_TICKET_DATA_STATE,  
    
    PLC_LAST_STATE
      
}PlcStatesTypeEnum;

typedef struct
{
    PlcStatesTypeEnum machState;
    PlcStatesTypeEnum prevMachState;
    
    UINT32 timer;
          
}PLC_STRUCT;

PLC_STRUCT PlcData;

PLC_TRUCK_FLOW_DATA PlcTruckFlowData;

INT16 read_holding_regs[40];
float read_holding_regsSP[40];
   
UINT8 MeterTcktDataPending =0;
UINT8 LogBuffer[196];

UINT8 MbMsgReceived;

/** Functions *****************************************************************/
void PlcProcessCommands(int cmd,char *pBuf);

/*
*|----------------------------------------------------------------------------
*|  Routine: PLCMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PlcMachineInit(void)
{ 
    if( !SciBinaryReceiverInit(SCI_PLC_COM, SCI_PLC_BAUD_RATE, NULL_PTR, PlcProcessCommands) )
    {
        //!!!
        return FALSE;
    }         
    
    MbMsgReceived =0;
    
   // LORA_ENABLE;
                
    PlcData.machState =PLC_INIT_STATE;
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PlcStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PlcStateProcess
(
    PLC_STRUCT *structInfo,
    PlcStatesTypeEnum nextState
)
{
    structInfo->prevMachState =structInfo->machState;
    structInfo->machState =nextState;
    
    structInfo->timer =xTaskGetTickCount();
}

#define SIM_LOAD_UNLOAD
#define SIM_LOADING_TIME      3600000
#define SIM_NOT_LOADING_TIME 82800000
/*
*|----------------------------------------------------------------------------
*|  Routine: PlcMachine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PlcMachine(void)
{    
static UINT32 loadingTestTimer =0;

    TickType_t xTicks;    
    
    MODBUS_FIELDS modbusFields;

    xTicks=xTaskGetTickCount();    

//PlcData.machState =PLC_SYSTEM_SLEEP_STATE;
    switch(PlcData.machState)
    {
        case PLC_INIT_STATE:                      
            PlcStateProcess(&PlcData, PLC_IDLE_STATE);
            break;
        case PLC_IDLE_STATE:          
            if( (xTicks-PlcData.timer) >5000 )             
            {               
            #ifdef SIM_LOAD_UNLOAD                
                loadingTestTimer =xTicks;
            #endif
                PlcStateProcess(&PlcData, PLC_LOADING_STATE);                    
            }             
            break;
        case PLC_LOADING_STATE:            
            if( (xTicks-PlcData.timer) >250 ) 
            {
                modbusFields.slaveAddr =2;
                modbusFields.addr =0; // starting address
                        
                modbusFields.nbrRegPoll =40;                  
                Modbus_ReadHoldingRegisters( &modbusFields );
                
                MbMsgReceived =0;
                
                PlcStateProcess(&PlcData, PLC_WAIT_SLAVE_POLL_STATE);                
            }
         
        #ifdef SIM_LOAD_UNLOAD                            
            if( (xTicks-loadingTestTimer) >SIM_LOADING_TIME )
            {
                /* simulate not loading conditions */
                MbMsgReceived  =3;
                PlcTruckFlowData.currentTruckCode =PlcTruckFlowData.ticketTruckCode;  
            }
            else
            {
                /* simulate loading conditions */
                PlcTruckFlowData.currentTruckCode =(PlcTruckFlowData.ticketTruckCode+1);
            }
        #endif

            if( MbMsgReceived &&
                NOT_LOADING               
              )
            {
                /* not loading/finished loading, get meter info */   
                modbusFields.slaveAddr =2;
                modbusFields.addr =30; // starting address
                    
                modbusFields.nbrRegPoll =50;                  
                Modbus_ReadHoldingRegisters( &modbusFields );
                    
                MbMsgReceived =0;               
                PlcStateProcess(&PlcData, PLC_WAIT_SLAVE_TICKET_DATA_STATE);   
            }
            break;
       case PLC_LOADING_COMPLETE_STATE:
            if( (xTicks-PlcData.timer) >250 )             
            {
                modbusFields.slaveAddr =2;
                modbusFields.addr =0; // starting address
                        
                modbusFields.nbrRegPoll =40;                  
                Modbus_ReadHoldingRegisters( &modbusFields );
                
                MbMsgReceived =0;
                
                PlcStateProcess(&PlcData, PLC_WAIT_SLAVE_POLL_STATE);            
            }
      
        #ifdef SIM_LOAD_UNLOAD                
            if( (xTicks-loadingTestTimer) >SIM_NOT_LOADING_TIME )
            {
                /* simulate loading conditions */
                MbMsgReceived =3;
                PlcTruckFlowData.currentTruckCode =(PlcTruckFlowData.ticketTruckCode+1);
            }     
            else
            {
                /* simulate not loading conditions */
                PlcTruckFlowData.currentTruckCode =PlcTruckFlowData.ticketTruckCode;  
            }
        #endif
            if( MbMsgReceived &&
                LOADING               
              )
            {
                PlcStateProcess(&PlcData, PLC_IDLE_STATE);       
            }            
            break;  
        case PLC_WAIT_SLAVE_POLL_STATE:
            if( MbMsgReceived ||
               (xTicks-PlcData.timer) >400
              )
            {                           
                PlcStateProcess(&PlcData, PlcData.prevMachState);  
            }             
            break;            
        case PLC_WAIT_SLAVE_TICKET_DATA_STATE:
            if( MbMsgReceived || 
                (xTicks-PlcData.timer) >2000               
              )
            {
                MeterTcktDataPending =GetNbrPendingMeterTickets();
                
                MeterTcktDataPending ++;
                
                SetNbrPendingMeterTickets(MeterTcktDataPending);
                
                memset(LogBuffer, 0x00, sizeof(LogBuffer) );
                memcpy(LogBuffer, read_holding_regs, sizeof(LogBuffer) );        
                       
                /* invoke log machine (log data to NV memory) */
                LogDataStart(METER_TCKT_LOG);     

                /* get meter ticket data, and que send log message */
                ServerQueInternlLogUpdate();
                    
            #ifdef SIM_LOAD_UNLOAD         
                loadingTestTimer =xTicks;
            #endif

                PlcStateProcess(&PlcData, PLC_LOADING_COMPLETE_STATE);
            }
            break;                   
    }
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PlcProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PlcProcessCommands
(
    int func,
    char *pRxBuffer  /* pointer to the receive buffer */      
)
{
   
//static INT16 values[16];  
UINT8 nbrRegRead =0;

    /* Check for uninitialized pointers, out of bounds inputs, etc */    
    if( pRxBuffer ==NULL_PTR ) return;
    
    switch(func)
    {
        case READ_HOLDING_REGS:
        #ifdef READ_WORD_REG
            nbrRegRead =Modbus_SlaveReceive(func, (UINT8 *)pRxBuffer, read_holding_regs);
             
            PlcTruckFlowData.rate =read_holding_regs[0];
            PlcTruckFlowData.density =read_holding_regs[1];
            PlcTruckFlowData.temp =read_holding_regs[2];                            
        #else    
            nbrRegRead =Modbus_SlaveReceiveSP(func, (UINT8 *)pRxBuffer, read_holding_regsSP);            
            
            PlcTruckFlowData.rate =(UINT16)(read_holding_regsSP[0]*100);
            PlcTruckFlowData.density =(UINT16)(read_holding_regsSP[1]*10);
            PlcTruckFlowData.temp =(UINT16)(read_holding_regsSP[2]*10);
            
            PlcTruckFlowData.loadAccumFlow =(UINT16)(read_holding_regsSP[5]*100);      
            
            PlcTruckFlowData.currentTruckCode =(UINT16)(read_holding_regsSP[6]);            
            PlcTruckFlowData.ticketTruckCode  =(UINT16)(read_holding_regsSP[18]);            
                 
            TLoadSetFlowRate( (float)read_holding_regsSP[0]);
            TLoadSetDensity( (float)read_holding_regsSP[1]);
            TLoadSetFlowTemperature( (float)read_holding_regsSP[2]);             
            TLoadSetLoadAccumFlow((float)read_holding_regsSP[5]);
        #endif
            break;
        default:
            func =0x7fff;
            break;
    }      
    
    MbMsgReceived =func;
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: PlcGetHregData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PlcGetHregData( INT16 *pHreg )
{
    memcpy( pHreg, read_holding_regs, sizeof(read_holding_regs) );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PlcGetData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PlcGetData(PLC_TRUCK_FLOW_DATA *pPlcTruckFlowData)
{
    memcpy( pPlcTruckFlowData, &PlcTruckFlowData, sizeof(PlcTruckFlowData) );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GetNbrPendingMeterTickets
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT8 GetNbrPendingMeterTickets(void)
{
    return RtcReadBackupData(RTC_BACKUP_NBR_MTR_TCKTS_ADDRESS);
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SetNbrPendingMeterTickets
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SetNbrPendingMeterTickets(UINT8 nbrPendingMeterTckts)
{   
    RtcWriteBackupData(RTC_BACKUP_NBR_MTR_TCKTS_ADDRESS, nbrPendingMeterTckts);
}



