/** C Source ******************************************************************
*
* NAME      TLoadMachine.c
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "MainControlTask.h"

#include "AdcFd.h"
#include "gpioHi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "lcdHi.h"

#include "HARTFd.h"

#include "LogMachine.h"
#include "GatewayConfig.h"

#include "PlcMachine.h"
#include "TLoadMachine.h"

#include "ServerPacket.h"

#include "CellularMachine.h"
#include "GpsMachine.h"
#include <stdio.h>

#define LOG_FTP_TIMER   43200000 //msec ==12hrs
           
/* use 4-20mA value for high level alarm */
#define TLOAD_HIGH_LVL_DETECTED  (TLoadTankData.lvlSensor.current <highLevelSetting && \
                                  TLoadTankData.lvlSensor.current >=4.0) 
                                   

typedef enum
{
    TLOAD_INIT_STATE =0,
    TLOAD_IDLE_STATE,    
    TLOAD_POLL_DEVICE_STATE,
    TLOAD_WAIT_RESPONSE_STATE,
    
    TLOAD_NOT_LOADING_STATE,
      
    TLOAD_PROCESS_STATE,
    TLOAD_POTENTIAL_SD_STATE,
    TLOAD_SD_STATE,

    TLOAD_LAST_STATE
      
}TLoadStatesTypeEnum;


typedef struct
{
    TLoadStatesTypeEnum machState;
    TLoadStatesTypeEnum prevMachState;
    
    UINT32 timer;
    UINT32 sleepTimer;
          
}TLOAD_STATE_STRUCT;


TLOAD_STATE_STRUCT TLoadState;

TLOAD_TANK_DATA TLoadTankData;

INT8 HartMsgReceived = -1;


/** Functions *****************************************************************/
void TloadProcessCommands(int func,char *pRxBuffer);
void TLoadSetUnits(TLOAD_TANK_DATA *pTloadTankData, bool metric);
/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL TLoadMachineInit(void)
{       
    memset( &TLoadTankData, 0x00, sizeof(TLoadTankData) );
    
    if( !SciBinaryReceiverInit(SCI_HART_COM, SCI_HART_BAUD_RATE, NULL_PTR, TloadProcessCommands) )
    {
        //!!!
        return FALSE;
    }         
         
    TLoadState.machState =TLOAD_INIT_STATE;
    
    return TRUE;    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadStateProcess
(
    TLOAD_STATE_STRUCT *pStructInfo,
    TLoadStatesTypeEnum nextState
)
{
    pStructInfo->prevMachState =pStructInfo->machState;
    pStructInfo->machState =nextState;
    
    pStructInfo->timer =xTaskGetTickCount();
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadMachine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadMachine(void)
{   
    static UINT32 FileTransferTimer =0;
    TLOAD_TANK_DATA tempTLoadTankData;
    GW_SETUP_STRUCT gwSetup;    
           
    GwGetSetup(&gwSetup);   
        
    static UINT32 CalcTimer=0;
    float highLevelSetting =0.0;
    
    char lcdStr[20];
    
    TickType_t xTicks=xTaskGetTickCount();
    
    if( (xTicks - CalcTimer) >1000 )
    {
        /* slow this down, CalcFluidVolume math intense */
        //TLoadTankData.level =GetSloadLevelAverage();
        //TLoadTankData.measuredRange =Adc4To20Conversion(TLoadTankData.current, gwSetup.sensorRange1, gwSetup.sensorRange2);            
        //TLoadTankData.measuredVolume =CalcFluidVolume(TLoadTankData.measuredRange);     
        
        CalcTimer =xTicks;           
    }
       
    if( (xTicks -FileTransferTimer) >LOG_FTP_TIMER ||
         ServerGetForceLogState(PLC_LOG)
      )
    {
        FileTransferTimer =xTicks;
        
        ServerForceLogTransfer(PLC_LOG, FALSE);
          
        /* que send log message via FTP */
        ServerQueInternlLogUpdate();
    }
    
    switch( TLoadState.machState )
    {      
        case TLOAD_INIT_STATE:
            Hart_SendCommand(HART_READ_UNIQUE_IDENT); //HART_READ_PRIMARY_VALUE); //HART_READ_UNIQUE_IDENT
                              
            FileTransferTimer =xTicks;
            
            TLoadStateProcess(&TLoadState, TLOAD_WAIT_RESPONSE_STATE);           
            break;
        case TLOAD_IDLE_STATE:
            break;      
        case TLOAD_POLL_DEVICE_STATE:
            if( (xTicks-TLoadState.timer) >300 ) 
            {
                /* HART_READ_DYN_VAR_AND_LOOP_CURRENT message (tx/rx) takes
                   about 500msec @ 1200bps
                */
                HartMsgReceived = -1;
              
                Hart_SendCommand(HART_READ_DYN_VAR_AND_LOOP_CURRENT);
                TLoadStateProcess(&TLoadState, TLOAD_WAIT_RESPONSE_STATE);
            }
            break;
        case TLOAD_NOT_LOADING_STATE:
            break;
        case TLOAD_WAIT_RESPONSE_STATE:
            if( HartMsgReceived != -1 )                         
            {               
                if( TLoadTankData.lvlSensor.current <4.0)
                {
                    TLoadSetRange(-1);                
                    TLoadSetPercent(-1);                  
                }
              
                float currVer =0.0;   
                bool metric =FALSE;
                currVer =atof(FW_VERSION); 
                UINT8 cellRssi =CellularGetRssi();
                int row =1;
                                 
                /* do not change original data, change temp struct for display only */
                memcpy( &tempTLoadTankData, &TLoadTankData, sizeof(tempTLoadTankData) );
                TLoadSetUnits(&tempTLoadTankData, metric);
                                 
                if(!metric)
                {
                    sprintf(lcdStr, "Range:  %2.3fin", tempTLoadTankData.lvlSensor.measuredRange);
                    Lcd_SendString(row++, lcdStr);
                    
                    sprintf(lcdStr, "Rel:    %2.1fdB", tempTLoadTankData.lvlSensor.reliability);
                    Lcd_SendString(row++, lcdStr);            
                    
                    //sprintf(lcdStr, "F Rate: %2.1fm^3/min", TLoadTankData.plcSensor.rate);
                    //Lcd_SendString(row++, lcdStr);                            
                    sprintf(lcdStr, "Temp:   %2.1fdegF", tempTLoadTankData.lvlSensor.temperature);
                    Lcd_SendString(row++, lcdStr);               
                }
                else
                {
                    sprintf(lcdStr, "Range:  %2.3fm", tempTLoadTankData.lvlSensor.measuredRange);
                    Lcd_SendString(row++, lcdStr);
                    
                    sprintf(lcdStr, "Rel:    %2.1fdB", tempTLoadTankData.lvlSensor.reliability);
                    Lcd_SendString(row++, lcdStr);            
                    
                    //sprintf(lcdStr, "F Rate: %2.1fm^3/min", tempTLoadTankData.plcSensor.rate);
                    //Lcd_SendString(row++, lcdStr);                            
                    sprintf(lcdStr, "Temp:   %2.1fdegC", tempTLoadTankData.lvlSensor.temperature);
                    Lcd_SendString(row++, lcdStr);                                 
                }
                
                sprintf(lcdStr, "RSSI: %d Ver: %1.2f", cellRssi, currVer);
                Lcd_SendString(row++, lcdStr);               
                
                //sprintf(lcdStr, "Temp:  %2.2fdegC", TLoadTankData.lvlSensor.temperature);
                //Lcd_SendString(row++, lcdStr);               
                               
                TLoadStateProcess(&TLoadState, TLOAD_PROCESS_STATE); 
            }
            else if( (xTicks-TLoadState.timer) >750 )
            {
                sprintf(lcdStr, "TRIG Gateway");
                Lcd_SendString(1, lcdStr);
                
                sprintf(lcdStr, "Sensor Error");
                Lcd_SendString(3, lcdStr);
                 
                TLoadSetRange(-1.0);     
                TLoadSetCurrent(-1.0);
                TLoadStateProcess(&TLoadState, TLoadState.prevMachState);
            }            
            break;     
        case TLOAD_PROCESS_STATE:
            /* log data */
            LogDataStart(PLC_LOG);
                      
            /* TLoad PRO, high level only */
            highLevelSetting =GwGetShutoffLevel();
            
            if( TLOAD_HIGH_LVL_DETECTED )
            {
                RELAY_ENABLE;
                
                //sprintf(lcdStr, "High Level Alarm");
                //Lcd_SendString(4, lcdStr);                  
            }
            else
            {
                RELAY_DISABLE;
                
                //sprintf(lcdStr, "                ");        
                //Lcd_SendString(4, lcdStr);
            }
            
            TLoadStateProcess(&TLoadState, TLOAD_POLL_DEVICE_STATE);
            break;
    }    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetUnits
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetUnits(TLOAD_TANK_DATA *pTloadTankData, bool metric)
{    
    if(!metric)    
    {
        /* quick conversion ,eters to inches*/
        //pTloadTankData->lvlSensor.measuredRange *=39.37;
        pTloadTankData->lvlSensor.temperature   =(pTloadTankData->lvlSensor.temperature*9/5)+32;
    }    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadGetData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadGetData(TLOAD_TANK_DATA *ptloadTankData)
{
    memcpy( ptloadTankData, &TLoadTankData, sizeof(TLoadTankData) );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetRange
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetRange(float range)
{
    TLoadTankData.lvlSensor.measuredRange =range;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetPercent
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetPercent(float percent)
{
    TLoadTankData.lvlSensor.percent =percent;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetCurrent
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetCurrent(float current)
{
    TLoadTankData.lvlSensor.current =current;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetReliability
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetReliability(float rel)
{
    TLoadTankData.lvlSensor.reliability =rel;  
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetTemperature
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetTemperature(float temp)
{
    TLoadTankData.lvlSensor.temperature =temp;  
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetFlowRate
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetFlowRate(float frate)
{
    TLoadTankData.plcSensor.rate =frate;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetDensity
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetDensity(float density)
{
    TLoadTankData.plcSensor.density =density;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetTemperature
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetFlowTemperature(float temp)
{
    TLoadTankData.plcSensor.flowTemp =temp;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: TLoadSetFlowRate
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TLoadSetLoadAccumFlow(float loadAccumFlow)
{
    TLoadTankData.plcSensor.loadAccumFlow =loadAccumFlow;
}
 
    
/*
*|----------------------------------------------------------------------------
*|  Routine: TloadProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void TloadProcessCommands
(
    int func,
    char *pRxBuffer  /* pointer to the receive buffer */      
)
{  
    UINT8 hartCmd =func;
    //UINT8 statusByte = pRxBuffer[13];
    
    static float pv =0.0;
    static float sv =0.0;
    static float tv =0.0;
    static float qv =0.0;
    static float current =0.0;
       
#if 0    
    if(statusByte !=0x00 )
    {
        /* ERROR */
        return;
    }
#endif

    switch(hartCmd)
    {
        case HART_READ_UNIQUE_IDENT:
            HartAddr[0] =pRxBuffer[13];
            HartAddr[1] =pRxBuffer[20];
            HartAddr[2] =pRxBuffer[21];
            HartAddr[3] =pRxBuffer[22];
            break;
        case HART_READ_PRIMARY_VALUE:
            pv =Hart_Receive(&pRxBuffer[16]);
                        
            TLoadSetRange(pv);
            break;
        case HART_READ_LOOP_CURRENT_AND_PRANGE:
            current =Hart_Receive(&pRxBuffer[15]);
            
            sv =Hart_Receive(&pRxBuffer[19]);          
            
            TLoadSetCurrent(current);
            TLoadSetPercent(sv);
            break;
        case HART_READ_DYN_VAR_AND_LOOP_CURRENT:          
            pv =Hart_Receive(&pRxBuffer[20]);              
            current =Hart_Receive(&pRxBuffer[15]);  
            sv =Hart_Receive(&pRxBuffer[25]);              
            tv =Hart_Receive(&pRxBuffer[30]);         
            qv =Hart_Receive(&pRxBuffer[35]);
            
            TLoadSetRange(sv);
            TLoadSetCurrent(current);
            TLoadSetPercent(pv);            
            TLoadSetReliability(tv);
            TLoadSetTemperature(qv);
            break;            
    }
    
    HartMsgReceived =hartCmd;
} 



