/** C Source ******************************************************************
*
* NAME      MlinkMachine.c
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

#include "MlinkMachine.h"

#include "flash_if.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "gpioHi.h"
#include "sysTimers.h"

#include "TloadMachine.h"

#include "PCMachine.h"

//#define MLINK_BASE
//#define MLINK_ENDPOINT

#define SERIAL_TIMEOUT_500MSEC (500)  //500msec
#define SERIAL_TIMEOUT_100MSEC (100)  //100msec
#define SERIAL_TIMEOUT_1SEC    (1000) //500msec

typedef enum
{
    MLINK_INIT_STATE =0,
    
    MLINK_IDLE_STATE,
    MLINK_WAIT_CNF,
    MLINK_WAIT_START_IND,
    
    MLINK_WAIT_ACTIVATE_START,
    MLINK_WAIT_ACTIVATE_SUCCESS,
   
    MLINK_PAIR,
    
    /* MLINK transfer states */
    MLINK_MSG_INIT,
    MLINK_POLL_START,
    MLINK_DATA_REQUEST,
    //MLINK_WAIT_NEXT_CNF,
    MLINK_WAIT_STATUS_IND,
    MLINK_WAIT_RADIO_IND,
    
    /* MLINK transfer types */
    //MLINK_SCHEDULED_POLLING,
    MLINK_MSG_TRANSFER_STATE,
    MLINK_EP_MESSAGING,
        
    MLINK_LAST_STATE
      
}MlinkStatesTypeEnum;



typedef struct
{
    MlinkStatesTypeEnum machState;
    MlinkStatesTypeEnum prevMachState;
        
    UINT32 timer;
    UINT32 sleepTimer;    
          
}MLINK_STATE_STRUCT;


typedef struct
{
    UINT16 som;
    UINT16 nbrBytes;
    UINT8 sigId;
    
}MLINK_COMMON;

typedef struct
{
    UINT8 epAddr;  
    UINT8 rssi;         // dBm
    
    UINT16 temperature;
    UINT16 pressure;
    UINT16 battery;
       
    UINT16 crc;

}MLINK_EP_DATA_STRUCT;


typedef union
{
  struct
  {
    uint8_t deviceType:1;
    uint8_t assertInfo:1;
    uint8_t extnederSupport:1;
    uint8_t spare:5;
  } b;         
  
}FLAGS;

typedef struct
{   
    FLAGS flags;  
    UINT8 hwRev;
    
    UINT16 hwFault;
    
    UINT16 fwMajorRev;
    UINT16 fwMinorRev;
    UINT16 fwFlags;
            
    UINT8 region;
       
    UINT16 crc;

}MLINK_INFO_STRUCT;

typedef struct
{   
    UINT32 sn;
    UINT8 outcome;
    INT8 rssi;          // dBm
    //UINT8 networkPath;
    UINT8 epAddr;
    
}MLINK_ACTVITY_STRUCT;

typedef struct
{    
    UINT8 outcome;
    UINT32 sn; //baseId/epId
    
}MLINK_START_IND_STRUCT;

typedef struct
{   
    UINT8 handle;    
    UINT8 status;
    
}MLINK_POLL_STATUS;

typedef struct
{   
    UINT8 epAddr;       // base only
    INT8 rssi;          // dBm
    UINT8 msg[256];
}MLINK_DATA_IND; // EP does not have epAddr in structure

typedef struct
{   
    UINT8 handle;
    UINT8 outcome;
    INT8 rssi;          // dBm

}MLINK_TX_STATUS_IND;

typedef struct
{
    UINT8 responseId;
    INT8 rssi;
    BOOL dataReady;
    BOOL pollReady;
    BOOL connect;
    BOOL pcktReceived;
    BOOL pollStatus;
    UINT8 epAddr;
    
}MLINK_COMM_STATUS;



MLINK_STATE_STRUCT MlinkState;
MLINK_STATE_STRUCT MlinkDataState;
MLINK_STATE_STRUCT MlinkPollingState;

MLINK_COMMON MlinkCommon;

//MLINK_EP_DATA_STRUCT    MlinkEpData;
MLINK_INFO_STRUCT          MlinkInfo;
MLINK_ACTVITY_STRUCT       MlinkActivity;
MLINK_START_IND_STRUCT     MlinkStartInd;
MLINK_POLL_STATUS          MlinkPollStatus;
MLINK_DATA_IND             MlinkDataInd;
//MLINK_EP_DATA_IND          MlinkEpDataInd;
MLINK_TX_STATUS_IND        MlinkTxStatusInd;


UINT8 CnfId;
UINT8 Reason;

MLINK_COMM_STATUS MlinkCommStatus;

//UINT8 MlinkResponseId;
//BOOL MlinkDataReady;
//BOOL MlinkConnect;

UINT16 EpmsgNbrBytes;

UINT16 NackCnt =0;
UINT16 AckCnt =0;
char TxBuffer[256]; /* transmit buffer */   

UINT32 ActionStartTimer =0;
UINT32 ActionEndTimer =0;

SCI_DATA_STRUCT MlinkDataCom;

XLOAD_TANK_DATA XloadTankData;

void MlinkProcessServerCmd(int cmd,char *pBuf);
void MlinkTxAck(UINT8 ackId);
//void MlinkProcessCmd(char *pBuf);
void MlinkProcessPolledResponse(char *pBuf);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkMachineInit(void)
{
    if( !SciBinaryReceiverInit(SCI_MLINK_COM, SCI_MLINK_BAUD_RATE, &MlinkDataCom, MlinkProcessServerCmd) )
    {
        //!!!
        return FALSE;
    }         
   
    memset( &MlinkCommStatus, 0x00, sizeof(MlinkCommStatus) );
    memset( &MlinkState, 0x00, sizeof(MlinkState) );
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkTransferMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkTransferMachineInit(void)
{
    memset( &MlinkDataState, 0x00, sizeof(MlinkDataState) );  
    memset( &MlinkPollingState, 0x00, sizeof(MlinkPollingState) );  

    MlinkDataState.machState =MLINK_MSG_INIT;
    MlinkPollingState.machState =MLINK_MSG_INIT;
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkCheckTimeout
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkCheckTimeout
(
    MLINK_STATE_STRUCT *pStateStruct,
    UINT32 timeout  //msec
)
{  
    if( (xTaskGetTickCount() - pStateStruct->timer) >timeout )
    {
        return TRUE;
    }
   
    return FALSE;   
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void MlinkStateProcess
(
    MLINK_STATE_STRUCT *pStructInfo,
    MlinkStatesTypeEnum nextState
)
{
    pStructInfo->prevMachState =pStructInfo->machState;
    pStructInfo->machState =nextState;
    
    pStructInfo->timer =xTaskGetTickCount();
}

/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkEnterTestMode
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkEnterTestMode(void)
{
    static UINT8 testState =0;

    char txBuffer[32];
    GPIO_InitTypeDef  GPIO_InitStructure;

    TickType_t xTicks=xTaskGetTickCount();
    
    /*!!! TODO ensure CTS is not stopping us from transmitting */
    
    switch(testState)
    {
        case 0:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //GPIO_Speed_50MHz;

            GPIO_InitStructure.GPIO_Pin = SERIAL_COM2_TX_PIN;
            GPIO_Init(SERIAL_COM2_TX_GPIO_PORT, &GPIO_InitStructure);          
    
            GPIO_SetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            //GPIO_ResetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            
            ML_RTS_ASSERT;        
            TimerDelayUs(50000);
            ML_RESET_NEGATE;
            //TimerDelayUs(50000);    
            ML_ENABLE;    
            //TimerDelayUs(50000);
            ML_RESET_ASSERT;   
   
            GPIO_SetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            
            testState =1;
           // break;
        case 1:
            TimerDelayUs(1200);
            
            GPIO_ResetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            TimerDelayUs(12000);
            
            GPIO_SetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            TimerDelayUs(2500);
            
            GPIO_ResetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            TimerDelayUs(12000);            
            
            GPIO_SetBits(SERIAL_COM2_TX_GPIO_PORT, SERIAL_COM2_TX_PIN);
            TimerDelayUs(1200);
            
            if( MlinkCommStatus.responseId ==0xaa )
            {
                MlinkCommStatus.responseId =0;
                
                /* re init serial port */
                if( !SciBinaryReceiverInit(SCI_MLINK_COM, SCI_MLINK_BAUD_RATE, &MlinkDataCom, MlinkProcessServerCmd) )
                {                
                    return FALSE;
                }          

                testState =2;
            }
            break;
        case 2:          
            strcpy( txBuffer, "\r\nw316,16,8,32\n");         
            //strcpy( txBuffer, "\r\nw315,512,16\n");         
            //strcpy( txBuffer, "r315\n");
            SendMlinkAsciiPacket(SCI_MLINK_COM, strlen(txBuffer), txBuffer); 
            testState =3;
            break;
        case 3:
            if( MlinkCommStatus.responseId ==0xaa )
            {             
                MlinkCommStatus.responseId =0;
                
                strcpy( txBuffer, "\r\nw315,2048,16\n");
                //strcpy( txBuffer, "\r\nw299\n");            
                SendMlinkAsciiPacket(SCI_MLINK_COM, strlen(txBuffer), txBuffer); 
                testState =4;
            }
            break;
        case 4:
            if( MlinkCommStatus.responseId ==0xaa )
            {   
                MlinkCommStatus.responseId =0;
                
                strcpy( txBuffer, "\r\nw299\n");            
                SendMlinkAsciiPacket(SCI_MLINK_COM, strlen(txBuffer), txBuffer); 
                testState =5;
            }          
            break;
    }   
    
    return TRUE;
    
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkEpMessaging
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkEpMessaging(void)
{
    //char txBuffer[256]; /* transmit buffer */   
    int j;
    
    UINT16 nbrTxBytes =0;
          
    static UINT32 testPollData[128];    
    
    TickType_t xTicks=xTaskGetTickCount();
    
    switch( MlinkDataState.machState )
    {
        case MLINK_MSG_INIT:
            /* load */
            MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);            
            memset( &testPollData, 0x00, sizeof(testPollData) );
            break;
        case MLINK_DATA_REQUEST:
            TimerDelayUs(500000);         
          
            TxBuffer[nbrTxBytes++] =0x00;
            TxBuffer[nbrTxBytes++] =0x00; 
            TxBuffer[nbrTxBytes++] =0x00; 
            TxBuffer[nbrTxBytes++] =0x00; 
            TxBuffer[nbrTxBytes++] =0x00; 
                 
            for(j=0; j<32; j++)            
            {
                testPollData[j] =testPollData[j] +j;
                
                TxBuffer[nbrTxBytes++] =((testPollData[j] &0x000000ff)); 
                TxBuffer[nbrTxBytes++] =((testPollData[j] &0x0000ff00)>>8); 
                TxBuffer[nbrTxBytes++] =((testPollData[j] &0x00ff0000)>>16); 
                TxBuffer[nbrTxBytes++] =((testPollData[j] &0xff000000)>>24);                
            }                    
            
            SendMlinkPacket(SCI_MLINK_COM, CMD_DATA_REQUEST, nbrTxBytes,TxBuffer);

            MlinkStateProcess(&MlinkDataState,MLINK_WAIT_CNF);          
            break;            
        case MLINK_WAIT_CNF:
            if(MlinkCommStatus.responseId ==CMD_MLINK_ACK )
            {
                /* confirmation (cnf) */
                MlinkCommStatus.responseId =0;
        
                MlinkStateProcess(&MlinkDataState,MLINK_WAIT_RADIO_IND);
            }          
            else if(MlinkCommStatus.responseId ==CMD_MLINK_NACK )
            {
                /* what to do ??? */
                /* confirmation (cnf) */
                MlinkCommStatus.responseId =0;        
                MlinkStateProcess(&MlinkDataState,MLINK_WAIT_CNF);
            }
            break;            
        case MLINK_WAIT_RADIO_IND:
            if( MlinkCommStatus.responseId ==CMD_TX_STATUS_IND )
            {
                /* EP */
                MlinkCommStatus.responseId =0;
                
                if( MlinkTxStatusInd.outcome !=OUTCOME_SUCCESS)
                {
                    /* what to do ??? */
                    MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);   
                }
                else
                {
                    /* que more data */
                    MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);              
                }
            }            
            if( MlinkCommStatus.responseId ==CMD_DATA_IND )
            {
                /* EP: received data from base, 
                   process received message
                   continue listening
                */
                MlinkCommStatus.responseId =0;
    
                //MlinkProcessCmd( (char *)MlinkDataInd.msg );

ActionEndTimer =xTicks -ActionStartTimer;
ActionStartTimer =xTicks;              

                MlinkStateProcess(&MlinkDataState,MLINK_WAIT_RADIO_IND);
            }   
            break;     
        case MLINK_LAST_STATE:                      
        default:
            /* what to do? */
            break;  
    }
    
    return TRUE;  
} /* MlinkEpMessaging() */


/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkDataTransfer
*|  Description: Downlink message transfer (base to endpoint)
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkDataTransfer(void)
{
  
    BOOL status =TRUE;
            
    TickType_t xTicks=xTaskGetTickCount();
    
    switch( MlinkDataState.machState )
    {
        case MLINK_MSG_INIT:
            //MlinkCommStatus.dataReady =FALSE;
            
            MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);
            break;        
        case MLINK_DATA_REQUEST:
        //#define TEST_PACKET                    
        #ifdef TEST_PACKET          
            MlinkCommStatus.dataReady =TRUE;
        #endif
            
            if(MlinkCommStatus.dataReady)
            { 
ActionStartTimer =xTicks;        
                
                MlinkCommStatus.dataReady =FALSE;
                MlinkCommStatus.pcktReceived =FALSE;
                             
                //TxBuffer[nbrTxBytes++] =MlinkActivity.epAddr;
                //TxBuffer[nbrTxBytes++] =0x00; /* flags */ 
                  
            #ifdef TEST_PACKET
                for(j=0; j<55; j++)            
                {
                    MlinkDataInd.msg[j] =MlinkDataInd.msg[j] +j;
                                      
                    TxBuffer[nbrTxBytes++] =((MlinkDataInd.msg[j] &0x000000ff)); 
                    TxBuffer[nbrTxBytes++] =((MlinkDataInd.msg[j] &0x0000ff00)>>8); 
                    TxBuffer[nbrTxBytes++] =((MlinkDataInd.msg[j] &0x00ff0000)>>16); 
                    TxBuffer[nbrTxBytes++] =((MlinkDataInd.msg[j] &0xff000000)>>24);                        
                }                                    
                SendMlinkPacket(SCI_MLINK_COM,CMD_DATA_REQUEST,nbrTxBytes,TxBuffer);
            #else           
                //memcpy(&TxBuffer[nbrTxBytes], MlinkDataInd.msg, EpmsgNbrBytes);                
                SendMlinkPacket(SCI_MLINK_COM,CMD_DATA_REQUEST,(EpmsgNbrBytes+2),TxBuffer);    
            #endif
                MlinkStateProcess(&MlinkDataState,MLINK_WAIT_CNF);
            }            
            break;
        case MLINK_WAIT_CNF:
            if(MlinkCommStatus.responseId ==CMD_MLINK_ACK )
            {
            AckCnt ++;
                /* confirmation (cnf) */
                MlinkCommStatus.responseId =0;
        
                MlinkStateProcess(&MlinkDataState,MLINK_WAIT_STATUS_IND);
            }          
            else if(MlinkCommStatus.responseId ==CMD_MLINK_NACK )
            {
                /* what to do ??? */
                /* confirmation (cnf) */
                MlinkCommStatus.dataReady =TRUE;
              
                MlinkCommStatus.responseId =0;        
                
                if( Reason ==MLS_REASON_TIMEOUT )
                {
                     MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);
                }
                else                
                {
                    //MlinkStateProcess(&MlinkTransferState,MLINK_WAIT_CNF);
                    MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);                    
                }
            }
            else if( (xTicks-MlinkDataState.timer) >5000 )            
            {
                MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);                                  
            }
            break;    
        case MLINK_WAIT_STATUS_IND:
            if(MlinkCommStatus.responseId ==CMD_TX_STATUS_IND /* ||
               MlinkCheckTimeout(&MlinkTransferState, 250)    */
              )
            {
                MlinkCommStatus.responseId =0;
     
ActionEndTimer =xTicks -ActionStartTimer;

                if( MlinkTxStatusInd.outcome ==OUTCOME_SUCCESS )
                {
                    MlinkCommStatus.pcktReceived =TRUE;
                    
                    /* wait for more data to send */        
                    MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);              
                }
                else if( MlinkTxStatusInd.outcome ==OUTCOME_MAX_TRIES )
                {
                    /* ??? */
                    MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);              
                }
            }            
            else if(MlinkCommStatus.responseId != 0 )
            {
                MlinkStateProcess(&MlinkDataState,MLINK_WAIT_STATUS_IND);              
            }
#if 0
            if( MlinkCheckTimeout(&MlinkDataState, 500) )
            {
ActionEndTimer =xTicks -ActionStartTimer;              
                /* no need to wait, just keep going 
                   clear the flag and wait for more data to send       
                */
                ActionEndTimer =xTicks -ActionStartTimer;
                MlinkCommStatus.dataReady =FALSE;
                MlinkCommStatus.pcktReceived =TRUE;            
                MlinkStateProcess(&MlinkDataState,MLINK_DATA_REQUEST);    
            }
#endif            
            break;  
    }

#if 0    
    int cmd =0;
    if( MlinkCommStatus.pcktReceived )
    {        
        EpmsgNbrBytes =MlinkGetStatus(TxBuffer); 
        /* use NACK to indicate command received, but request failed */ 
        cmd =(CMD_GW_GET_MLINK_STATUS | ACK);
        /* send a packet */
        SciSendPacket(SCI_PC_COM, cmd, EpmsgNbrBytes, TxBuffer);     
    }
#endif
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkScheduledPolling
*|  Description: Uplink message transfer (endpoint to base)
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkScheduledPolling(void)
{   
    UINT16 nbrTxBytes =0;
          
    //static UINT32 testPollData[128];    
    
    TickType_t xTicks=xTaskGetTickCount();
    
    switch( MlinkPollingState.machState )
    {
        case MLINK_MSG_INIT:
            MlinkCommStatus.pollReady =FALSE;
            MlinkStateProcess(&MlinkPollingState,MLINK_POLL_START);           
            break;
        case  MLINK_POLL_START: 
            if(MlinkCommStatus.connect )
                MlinkCommStatus.pollReady =TRUE;
            
            if( MlinkCommStatus.pollReady )
            {
                TxBuffer[nbrTxBytes++] =MlinkActivity.epAddr;
                TxBuffer[nbrTxBytes++] =0x00; /* message (what to poll for) */
                TxBuffer[nbrTxBytes++] =0x00; /* priority */
                        
                /* poll interval (UINT32 in seconds)
                   if 0 then one-shot polling, else scheduled polling
                */
                TxBuffer[nbrTxBytes++] =0x00; 
                TxBuffer[nbrTxBytes++] =0x00; 
                TxBuffer[nbrTxBytes++] =0x00; 
                TxBuffer[nbrTxBytes++] =30;
                //TxBuffer[nbrTxBytes++] =0x00; //MlinkDataInd.msg[0];
                
                MlinkCommStatus.pollReady =FALSE;
                MlinkCommStatus.pcktReceived =FALSE;
                                
                SendMlinkPacket(SCI_MLINK_COM, CMD_POLL_START_REQ, nbrTxBytes,TxBuffer);
                       
                MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_CNF);
            }
            break;           
        case MLINK_WAIT_CNF:
            if(MlinkCommStatus.responseId ==CMD_MLINK_ACK )
            {
                /* confirmation (cnf) */
                MlinkCommStatus.responseId =0;
        
                MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_STATUS_IND);
            }
            else if(MlinkCommStatus.responseId ==CMD_POLL_STATUS_IND )
            {
                MlinkCommStatus.responseId =0;
        
                MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_RADIO_IND);              
            }            
            else if(MlinkCommStatus.responseId ==CMD_MLINK_NACK )
            {
                /* what to do ??? */
                /* confirmation (cnf) */
                MlinkCommStatus.responseId =0;        
                MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_CNF);
            }
            break;            
        case MLINK_WAIT_STATUS_IND:
            if(MlinkCommStatus.responseId ==CMD_POLL_STATUS_IND )
            {
                /* base */
                MlinkCommStatus.responseId =0;
                    
                if(MlinkPollStatus.status ==POLL_STATUS_STARTED)
                {            
                    /* get data from EP */
                    MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_RADIO_IND);
                }
                else
                {
                    MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_STATUS_IND);
                }
            }
            break;
        case MLINK_WAIT_RADIO_IND:
            if(MlinkCommStatus.responseId ==CMD_DATA_IND ) 
            {
                /* received data from EP */
                MlinkCommStatus.responseId =0;
                        
                MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_RADIO_IND);
            }            
            else if(MlinkCommStatus.responseId ==CMD_POLL_STATUS_IND )
            {
                MlinkCommStatus.responseId =0;
                
                MlinkCommStatus.pcktReceived =TRUE;
                MlinkCommStatus.pollStatus =MlinkPollStatus.status;
                
                if(MlinkPollStatus.status ==POLL_STATUS_COMPLETE)
                {
                    /* stay here for now */
                    MlinkProcessPolledResponse((char *)MlinkDataInd.msg );
                    MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_RADIO_IND);
                }      
                else if(MlinkPollStatus.status ==POLL_STATUS_NO_DATA)
                {
                    MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_RADIO_IND);
                    //MlinkCommStatus.pollReady =TRUE;
                }
                else
                {
                    MlinkStateProcess(&MlinkPollingState,MLINK_WAIT_RADIO_IND);
                }                
            }                        
            else if( MlinkCommStatus.pollReady )
            {
                MlinkStateProcess(&MlinkPollingState,MLINK_POLL_START);              
            }
            break;  
        case 100:   
            if( MlinkCheckTimeout(&MlinkPollingState, 5000) )              
            {
                MlinkCommStatus.pollReady =TRUE;
                MlinkStateProcess(&MlinkPollingState,MLINK_POLL_START);   
            }
            break;
        case MLINK_LAST_STATE:                      
        default:
            /* what to do? */
            break;  
    }
    
    return TRUE;
}


//#define ENTER_TEST_MODE     
/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkMachine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MlinkMachine(void)
{
    //char txBuffer[64]; /* transmit buffer */   
    
    UINT16 nbrTxBytes =0;
             
    TickType_t xTicks=xTaskGetTickCount();
    
    MlinkBinaryRxMachine(&MlinkDataCom, SCI_MLINK_COM);
    
    switch( MlinkState.machState )
    {
        case MLINK_INIT_STATE:
            /* startup successful */            
            memset( TxBuffer, 0x00, sizeof(TxBuffer) );
            
        #ifdef ENTER_TEST_MODE            
            MlinkEnterTestMode();
        #else            
            MlinkCommStatus.responseId =0;
                                   
            ML_RTS_ASSERT;        
            TimerDelayUs(50000);
            ML_RESET_NEGATE;
            TimerDelayUs(50000);    
            ML_ENABLE;    
            TimerDelayUs(50000);
            ML_RESET_ASSERT;          
             
            MlinkStateProcess(&MlinkState,MLINK_IDLE_STATE);
        #endif
            break;
        case MLINK_IDLE_STATE:           
            if(MlinkCommStatus.responseId ==CMD_START_RESET)
            {
                MlinkCommStatus.responseId =0;
                
                TxBuffer[nbrTxBytes++] =0x00;
                TxBuffer[nbrTxBytes++] =0x00;
               
                if(MlinkInfo.flags.b.deviceType ==MLINK_DEVICE_BASE )                   
                {
                    SendMlinkPacket(SCI_MLINK_COM, CMD_START_REQ, nbrTxBytes, TxBuffer);                    
                    MlinkStateProcess(&MlinkState,MLINK_WAIT_CNF);
                }
                else
                {
                    TxBuffer[nbrTxBytes++] =0x00;
                    TxBuffer[nbrTxBytes++] =0x00;
                                    
                    SendMlinkPacket(SCI_MLINK_COM, CMD_START_REQ, nbrTxBytes, TxBuffer);
                    MlinkStateProcess(&MlinkState,MLINK_WAIT_CNF);
                }                               
            }
            break;       
        case MLINK_WAIT_CNF: 
            if(MlinkCommStatus.responseId ==CMD_MLINK_ACK )
            {
                /* confirmation(cnf) */
                MlinkCommStatus.responseId =0;
        
                MlinkStateProcess(&MlinkState,MLINK_WAIT_START_IND);
            }
            if( MlinkCheckTimeout(&MlinkState, 2000) )              
            {
                MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);                            
            }
            break;
        case MLINK_WAIT_START_IND:
            if(MlinkCommStatus.responseId ==CMD_START_IND )
            {
                MlinkCommStatus.responseId =0;
        
                if(MlinkInfo.flags.b.deviceType ==MLINK_DEVICE_BASE )       
                    MlinkStateProcess(&MlinkState,MLINK_WAIT_ACTIVATE_START);
                else
                    MlinkStateProcess(&MlinkState,MLINK_WAIT_ACTIVATE_SUCCESS);
            }          
            break;                   
        case MLINK_WAIT_ACTIVATE_START:                    
            /* MLINK base started, wait for EP data */
            if(MlinkCommStatus.responseId ==CMD_ACTIVATE_INDICATION &&
               MlinkActivity.outcome == OUTCOME_STARTED
              )
            {
                /* indication(ind) */
                MlinkCommStatus.responseId =0;
                MlinkStateProcess(&MlinkState,MLINK_WAIT_ACTIVATE_SUCCESS);
            }                             
            break;
        case MLINK_WAIT_ACTIVATE_SUCCESS:
            if(MlinkCommStatus.responseId ==CMD_ACTIVATE_INDICATION &&
               MlinkActivity.outcome == OUTCOME_SUCCESS
              )
            {
                /* Startup: Normal flow complete */
                MlinkCommStatus.responseId =0;
               
                MlinkCommStatus.connect =TRUE;
                 
                MlinkTransferMachineInit();
                
                if(MlinkInfo.flags.b.deviceType ==MLINK_DEVICE_BASE )
                {
                    MlinkStateProcess(&MlinkState,MLINK_WAIT_ACTIVATE_START); //MLINK_MSG_TRANSFER_STATE);
                }
                else
                {
                    MlinkStateProcess(&MlinkState,MLINK_EP_MESSAGING);
                }
            } 
            if( MlinkCheckTimeout(&MlinkState, 30000) )              
            {
                MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);                            
            }
            break;
#if 0            
        case MLINK_MSG_TRANSFER_STATE:
            /* downlink message transfer: data */
            if( !MlinkDataTransfer() )
            {
                MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);              
            }
            /* uplink message transfer: polling */
            if( !MlinkScheduledPolling() )
            {     
                MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);              
            }
            break;
        case MLINK_EP_MESSAGING:
            /* endpoint messaging state */
            if( !MlinkEpMessaging() )
            {
                MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);                            
            }
            break;
#endif
    }
    
    if(MlinkInfo.flags.b.deviceType ==MLINK_DEVICE_BASE )
    {
        /* downlink message transfer: data */
        if( !MlinkDataTransfer() )
        {
            MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);              
        }
        /* uplink message transfer: polling */
        if( !MlinkScheduledPolling() )
        {     
            MlinkStateProcess(&MlinkState,MLINK_INIT_STATE);              
        }
    }
    
    return TRUE;
}

/*
*|------------------------------------------------------------------------------
*|  Routine: MlinkProcessServerCmd
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
void MlinkProcessServerCmd
(
    int cmd,
    char *pRxBuf  /* pointer to the receive buffer */   
)
{         
    BOOL mlinkAck =TRUE;
       
    MlinkCommStatus.responseId =cmd;

    memcpy(&MlinkCommon, pRxBuf, sizeof(MlinkCommon)); 
    
    pRxBuf +=MLINK_SIZEOF_HEAD;
    
    switch(cmd)
    {
        case CMD_MLINK_ACK:  /* confirmation(cnf) */
            CnfId = *pRxBuf;
            mlinkAck =FALSE;
            break;
        case CMD_START_IND:
            memcpy(&MlinkStartInd, pRxBuf, sizeof(MlinkStartInd));                 
            break;
        case CMD_ACTIVATE_INDICATION:     
            if(MlinkInfo.flags.b.deviceType ==MLINK_DEVICE_BASE )   
                memcpy(&MlinkActivity, pRxBuf, sizeof(MlinkActivity));                       
            else
                MlinkActivity.outcome =OUTCOME_SUCCESS;        
            
            MlinkCommStatus.rssi =MlinkActivity.rssi;
            break;
        case CMD_START_RESET:                                                           
            memcpy(&MlinkInfo, pRxBuf, sizeof(MlinkInfo));                                              
            break;
        case CMD_POLL_STATUS_IND:            
            memcpy(&MlinkPollStatus, pRxBuf, sizeof(MlinkPollStatus)); 
            break;
        case CMD_DATA_IND:
            if(MlinkInfo.flags.b.deviceType ==MLINK_DEVICE_BASE )   
                memcpy(&MlinkDataInd, pRxBuf, sizeof(MlinkDataInd));           
            else
            {
                /* skip epAddr as it is not in the response */
                memcpy(&MlinkDataInd.rssi, pRxBuf, sizeof(MlinkDataInd)-sizeof(UINT8));           
            }
            
            MlinkCommStatus.rssi =MlinkDataInd.rssi;
            break;
        case CMD_TX_STATUS_IND:
            memcpy(&MlinkTxStatusInd, pRxBuf, sizeof(MlinkTxStatusInd));
            
            MlinkCommStatus.rssi =MlinkTxStatusInd.rssi;
            break;
        case CMD_PAIR_IND:
            break;
        case CMD_LINK_ADAPT_IND:
        case CMD_LINK_CHANGE_IND:
            break;
        case 0xaa: /* UUT */            
            break;            
        case 0xfe: /* checksum error */            
            break;                        
        case CMD_MLINK_NACK:  /* confirmation(cnf) */
            CnfId  = *pRxBuf ++;
            Reason = *pRxBuf;
            mlinkAck =FALSE; 
            
            NackCnt ++;
            break;            
        case 0xd9: /* not in manual ??? */
            break;
        default:
            mlinkAck =FALSE;
            break;
    }
                
    if( mlinkAck )
    {
        /* need to ACK the MLINK hardware
           response (rsp), a message issued by a client in response to an ind
        */
        MlinkTxAck(MlinkCommStatus.responseId);
    }    
} 


/*
*|------------------------------------------------------------------------------
*|  Routine: MlinkTxAck
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
void MlinkTxAck(UINT8 ackId)
{        
    /* send a packet */
    SendMlinkPacket(SCI_MLINK_COM, CMD_MLINK_ACK, 1, (char *)&ackId);    
}


/*
*|------------------------------------------------------------------------------
*|  Routine: MlinkGetStatus
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
UINT8 MlinkGetStatus(char *pBuf)
{
    memcpy( pBuf, &MlinkCommStatus, sizeof(MlinkCommStatus));
    
    MlinkCommStatus.pcktReceived =FALSE;
      
    return sizeof(MlinkCommStatus);
}
    
/*
*|------------------------------------------------------------------------------
*|  Routine: MlinkDataQue
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
void MlinkDataQue(char *pBuf)
{
    MlinkCommStatus.dataReady =TRUE;

    EpmsgNbrBytes = ( (pBuf[1]<<8) | pBuf[2] );

    TxBuffer[0] =MlinkActivity.epAddr;
    TxBuffer[1] =0x00; /* flags */ 
    
    memcpy( &TxBuffer[2],pBuf, EpmsgNbrBytes);
}

/*
*|------------------------------------------------------------------------------
*|  Routine: MlinkPollQue
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
void MlinkPollQue(char *pBuf)
{
    MlinkCommStatus.pollReady =TRUE;

    EpmsgNbrBytes = ( (pBuf[1]<<8) | pBuf[2] );
}

/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkGetDataMsg
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT16 MlinkGetDataMsg(UINT8 *pDataBuf)
{
    memcpy(pDataBuf, MlinkDataInd.msg, 64);
    
    return sizeof(MlinkDataInd.msg);
}

/*
*|----------------------------------------------------------------------------
*|  Routine: MlinkGetXloadData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT16 MlinkGetXloadData(UINT8 *pDataBuf)
{
    memcpy(pDataBuf, &XloadTankData, sizeof(XloadTankData));
    
    return sizeof(XloadTankData);
}

/*
*|------------------------------------------------------------------------------
*|  Routine: MlinkProcessPolledResponse
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
void MlinkProcessPolledResponse(char *pBuf)
{
    UINT8 cmd = *pBuf;
    
    switch(cmd)
    {
        case 8:
            /* skip over cmd byte */
            *pBuf ++;
            memcpy(&XloadTankData, pBuf, sizeof(XloadTankData) );
            break;
    }
}

