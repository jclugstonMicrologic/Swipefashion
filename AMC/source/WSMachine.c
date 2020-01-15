/** C Source *******************************************************************
*
* NAME      WSMachine.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
********************************************************************************
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

/** Include Files *************************************************************/
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "WSClient.h"

#include "CellularMachine.h"
#include "WSMachine.h"
#include "Transport.h"
#include "ServerPacket.h"

#include "FtpSocket.h"

#include "lcdHi.h"

#include "MainControlTask.h"
#include "GatewayConfig.h"
#include "LogMachine.h"

#include <pb_encode.h>
#include <pb_decode.h>

#define RX_MSG_BYTE_POS     3
#define PING_TIMEOUT    60000

enum
{
    WS_INIT_STATE =0,
    
    WS_WAIT_HANDSHAKE,
    WS_CHECK_FOR_MSG,
    WS_CLIENT_MSG_STATE,
    WS_WAIT_SERVER_RESPONSE_STATE,    
    WS_LISTEN_SERVER_STATE,
    WS_WAIT_NACK_STATE,

    WS_LAST_STATE
};


int WSClientMachState =0;
int WSNextMachState =0;

int MessageParse(uint8_t *pRxData);
int MessageHandler(SCI_DATA_STRUCT *pSerialData);


BOOL CheckFwUpdateAvailable(float updateVer);
void ResetPingTimer();

BOOL FwUpdateAvailable =FALSE;

char FwFilename[20];

UINT32 PingTimer =0;
    
/*
*|----------------------------------------------------------------------------
*|  Module: WSMachine
*|  Routine: WSMach_ClientInit
*|  Description:
*|----------------------------------------------------------------------------
*/
void WSMach_ClientInit(void)
{
    FwUpdateAvailable =FALSE;
  
    WSClientMachState =WS_INIT_STATE;
}


/*
*|----------------------------------------------------------------------------
*|  Module:WSMachine 
*|  Routine: WSMach_ClientStart
*|  Description:
*|----------------------------------------------------------------------------
*/
void WSMach_ClientStart(void)
{
    WSClientMachState =1;
}

/*
*|----------------------------------------------------------------------------
*|  Module:WSMachine 
*|  Routine: WSMach_ClientShutdown
*|  Description:
*|----------------------------------------------------------------------------
*/
void WSMach_ClientShutdown(void)
{
    WSClient_close();  
    
    WSClientMachState =WS_INIT_STATE;
}

/*
*|----------------------------------------------------------------------------
*|  Module:WSMachine 
*|  Routine: WSMach_ClientMachine
*|  Description:
*|----------------------------------------------------------------------------
*/
int WSMach_ClientMachine(UINT8 msg)
{
    int status =SOCK_STATUS_IDLE;   
    int cellResponse =0;
        
    static unsigned char payloadMsg[200];       
    static TickType_t timeoutTimer =0;
             
    TickType_t xTicks=xTaskGetTickCount();
    
    static int len =0;
             
    switch(WSClientMachState)
    {
        case WS_INIT_STATE:      
            WSClient_connect_from_url("ws://192.168.20.120:80/ws/test/binary/", NULL);                      
                        
            timeoutTimer =xTicks; 
            
            /* init to no fw update available, we will ask server later if new
               fw is availalbe for download
            */
            FwUpdateAvailable =FALSE;
            
            WSNextMachState =WS_CLIENT_MSG_STATE; 
            WSClientMachState =WS_WAIT_HANDSHAKE;
            break;
        case WS_WAIT_HANDSHAKE:
            cellResponse =CellularGetResponse();
            
            if( cellResponse ==CELL_REPLY_WS_OPEN )
            {
                WSClientMachState =WS_CHECK_FOR_MSG; 
            }
            else if( (xTicks -timeoutTimer) >10000 ||
                     cellResponse ==CELL_REPLY_ERROR   
                   )
            {
                timeoutTimer =xTicks;                
                status =SOCK_STATUS_FAIL;
                
                WSClientMachState =WS_INIT_STATE;
            }                   
            break;    
        case WS_CHECK_FOR_MSG:
            if( msg )
                WSClientMachState =WS_CLIENT_MSG_STATE; 
            else
                WSClientMachState =WS_LISTEN_SERVER_STATE; 
            break;
        case WS_CLIENT_MSG_STATE:      
            /* we have a message to send */                        
            memset(payloadMsg, 0x00, sizeof(payloadMsg));

            len =ServerBuildMessage( (char *)payloadMsg, (ServerMsgsTypeEnum)msg);    
                                 
            WSClient_send_message_binary((char *)payloadMsg, len);
            
            timeoutTimer =xTicks;
                
            /* wait for server ACK/reply */
            WSClientMachState =WS_WAIT_SERVER_RESPONSE_STATE;                       
            break;
        case WS_WAIT_SERVER_RESPONSE_STATE:
#if 1
            /* only accept binary frames for now (0x02) */
            if( WSClient_check_opcode() ==0x02 )  
            {               
                /* parse message */
                if( MessageParse(ReceivedWsData.rxBuf) )
                {
                    ResetPingTimer();
                    
                    memset( &ReceivedWsData, 0x00, sizeof(ReceivedWsData) );
                    
                    timeoutTimer =xTicks;
                    
                    status =SOCK_STATUS_PASS;
                    
                    WSClientMachState =WS_CHECK_FOR_MSG;
                }
                else
                {
                    timeoutTimer =xTicks;                
                    status =SOCK_STATUS_FAIL;
                
                    WSClientMachState =WS_INIT_STATE;
                }
            }
            else if( (xTicks -timeoutTimer) >10000 )
            {
                timeoutTimer =xTicks;                
                status =SOCK_STATUS_RETRY;
                
                WSClientMachState =WS_INIT_STATE;
            }               
#else
            /* skip waiting for server reply, just continue on */
            memset( &ReceivedWsData, 0x00, sizeof(ReceivedWsData) );
                
            timeoutTimer =xTicks;
                
            status =SOCK_STATUS_PASS;
            
            WSClientMachState =WS_CHECK_FOR_MSG;
#endif           
            break;          
        case WS_LISTEN_SERVER_STATE:
            if( WSClient_check_opcode() ==0x02 ) 
            {
                if( MessageParse(ReceivedWsData.rxBuf) )
                {
                    ResetPingTimer();
                    
                    memset( &ReceivedWsData, 0x00, sizeof(ReceivedWsData) );
                    
                    timeoutTimer =xTicks;
                    
                    status =SOCK_STATUS_PASS;
                    
                    WSClientMachState =WS_CHECK_FOR_MSG;
                }
            }
            else if( msg )
            {
                /* we have a message to transmit, go do that */
                WSClientMachState =WS_CLIENT_MSG_STATE; 
            }         
            break;            
    }
    
    return status;
}


/*
*|----------------------------------------------------------------------------
*|  Module: WSMachine
*|  Routine: WSMach_CheckForPing
*|  Description:
*|----------------------------------------------------------------------------
*/
BOOL WSMach_CheckForPing(void)
{
    BOOL status =TRUE;
    
    if( WSClient_check_opcode() ==0x09 )
    {
        ResetPingTimer();    
        WSClient_clear_opcode();
    }
    else if( (xTaskGetTickCount() -PingTimer) >PING_TIMEOUT ) 
    {
        status =FALSE;
    }
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Module: WSMachine
*|  Routine: MessageParse
*|  Description:
*|----------------------------------------------------------------------------
*/
int MessageParse(uint8_t *pRxData)
{
    static LOAD_PLAN loadPlan;
    static STATUS_PACKET statusPckt;
    static FW_UPDATE_VERSION fwUpdateVer;
    static GW_PARAMETERS gwParameters;
     
    GW_SETUP_STRUCT gwSetup;
        
    int msg =pRxData[RX_MSG_BYTE_POS];
    int status =0; 

    switch(msg)
    {
        case MESSAGE_SENSOR_DATA_POST:
        case MESSAGE_GPS_DATA_POST:
        case MESSAGE_REGISTER_GTW_POST:            
        case MESSAGE_ACCELEROMETER_DATA_POST:            
        case MESSAGE_COMPLETED_DATA_POST:
        case MESSAGE_METER_TCKT_DATA_POST:
            /* we should not receive post messages from the server, they should
              only be status message responses.  If we do it is actually an echo
              from the radio which indicates the socket is closed, 
              so we need to restart (re-esablished the socket connection)
            */          
            break;
        case MESSAGE_LEVEL_DATA_GET:            
            status =1; 
            break;                        
        case MESSAGE_LOAD_PLAN_GET:     
            /* decode message */
            DecodeTloadLoadPlan(pRxData, &loadPlan);            
            status =1; 
            
            /* !!!save to memory!!! */
            break;   
        case MESSAGE_FW_UPDATE_VERSION_GET: 
            /* check if there is a newer version of fw to download */ 
            DecodeFwVersionAvailable(pRxData, &fwUpdateVer);         
            
            if( CheckFwUpdateAvailable(fwUpdateVer.fwVersion) &&
                !GetFwUpdateStatus() /* no firmware update pending */
              ) 
            {
                /* server has a newer (different) firmware version and there are 
                   no previously downloaded firmware images in memory that are 
                   pending programming(re-flash), so lets download this image
                */                
                CreateFwFilename(fwUpdateVer.fwVersion);
                
                ServerQueFwUpdate();               
            }            
            status =1;
            break;        
        case MESSAGE_GW_PARAMETER_SET:
            /* decode message */
            DecodeTloadParameter(pRxData, &gwParameters);
            
            gwSetup.highLevel =gwParameters.highLevelSetting;
            gwSetup.remoteUpdateRate =(gwParameters.sensRemoteUpdateRate*1000);
            gwSetup.vOffset =gwParameters.voffset;
            gwSetup.logRate =gwParameters.sensLogRate;
            ServerForceLogTransfer(PLC_LOG, (BOOL)gwParameters.forceLogTransfer);
                               
            GwWriteSetupToFlash(&gwSetup);  
            
            /* ack the server */
            //ServerSetStatus(TRUE);
                        
            status =1;           
            break;
        case MESSAGE_GW_PARAMETER_GET:           
            /* ack the server */
            ServerSetStatus(TRUE);
                        
            status =1;           
            break;            
        case MESSAGE_STATUS:     
            /* decode message */
            DecodeTloadStatus(pRxData, &statusPckt);               
            status =1;
            break;      
        default:
            /* this could be the server PING, so check if opcode ==0x09 */
            status =0;
            break;
    }    
      
    return status;
}


/*
*|----------------------------------------------------------------------------
*|  Module: WSMachine
*|  Routine: CheckFwUpdateAvailable
*|  Description: bool
*|----------------------------------------------------------------------------
*/
BOOL CheckFwUpdateAvailable(float updateVer)
{
    float currVer =0.0;
    
    currVer =atof(FW_VERSION); 
    
    if( (updateVer-currVer) >=0.005 || (currVer-updateVer) >= 0.005 )return TRUE;
    else return FALSE;
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Module: WSMachine
*|  Routine: GetFwUpdateAvailStatus
*|  Description: bool
*|----------------------------------------------------------------------------
*/
BOOL GetFwUpdateAvailStatus(void)
{
    static UINT8 TestValue =0;
 
    if( (TestValue++ %2) ==0 )
        return TRUE;
    else
        return FALSE;
}
#endif



/*
*|----------------------------------------------------------------------------
*|  Routine: CreateFwFilename
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void CreateFwFilename(float fwVer)
{   
    sprintf( FwFilename, "Gateway_v%1.2f.hex", fwVer);
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GetFwFilename
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GetFwFilename(char *pFileName)
{   
    strcpy(pFileName, FwFilename);
}

/*
*|----------------------------------------------------------------------------
*|  Module: WSMachine
*|  Routine: ResetPingTimer
*|  Description: bool
*|----------------------------------------------------------------------------
*/
void ResetPingTimer()
{
    PingTimer =xTaskGetTickCount();
}


