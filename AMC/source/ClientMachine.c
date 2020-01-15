/** C Source *******************************************************************
*
* NAME      ClientMachine.c
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

#include "ClientMachine.h"
#include "CellularMachine.h"
#include "ServerPacket.h"
#include "PowerManagement.h"

#include "MQTTClient.h"
#include "SocketClient.h"
#include "FtpSocket.h"
#include "WSMachine.h"

enum
{
    SOCKET_IDLE_STATE =0,
    SOCKET_CONNECT_STATE,
    SOCKET_FAIL_STATE,
    
    SOCKET_LAST_STATE
    
};

typedef enum
{    
    CLIENT_INIT_STATE =0,
    CLIENT_IDLE_STATE,
    CLIENT_NEXT_STATE,
    CLIENT_CONNECTION_STATE,
    CLIENT_MSG_STATE,
    CLIENT_FTP_INIT_STATE,
    CLIENT_FTP_STATE,

    CLIENT_WAIT_STATE,
    CLIENT_CMD_MODE_STATE, 
    
    CLIENT_SD_STATE,
  
    
    CLIENT_LAST_STATE
      
}ClientStatesTypeEnum;


typedef struct
{
    ClientStatesTypeEnum machState;
    ClientStatesTypeEnum prevMachState;
    
    UINT32 timer;

    int (*pConnectionProtocol)(void);// * pvParameters);
    
    int (*pMsgProtocol)(UINT8);
    
}CLIENT_DATA_STRUCT;

CLIENT_DATA_STRUCT ClientData;

//#define MQTT_MESSAGING
//#define SOCKET_MESSAGING
#define WEBSOCKET_MESSAGING
/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: ClientMachineInit
*|  Description:
*|----------------------------------------------------------------------------
*/
void ClientMachineInit(void)
{
    ClientData.machState =CLIENT_INIT_STATE;
    ClientData.pConnectionProtocol =CellularMachine;

#if defined(MQTT_MESSAGING)
    ClientData.pMsgProtocol = MQTTClientMachine;
#elif defined(SOCKET_MESSAGING)
    ClientData.pMsgProtocol =SocketClientMachine; //FtpSocket_Machine;
#elif defined(WEBSOCKET_MESSAGING)
    ClientData.pMsgProtocol = WSMach_ClientMachine;
#endif        
}


/*
*|----------------------------------------------------------------------------
*|  Routine: ClientMachineStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void ClientMachineStateProcess
(
    CLIENT_DATA_STRUCT *structInfo,
//    ClientStatesTypeEnum prevState,
    ClientStatesTypeEnum nextState
)
{
    structInfo->prevMachState =structInfo->machState;//prevState;
    structInfo->machState =nextState;
    
    structInfo->timer =xTaskGetTickCount();
}

void ClientShutdown(void)
{
#if defined(MQTT_MESSAGING)
    MQTTClientShutdown();            
#elif defined(SOCKET_MESSAGING)
    SocketClientShutdown();            
#elif defined(WEBSOCKET_MESSAGING)
    WSMach_ClientShutdown();            
#endif    
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: ClientMachine
*|  Description: connect to a socket using hardware available, and manage 
*|               client messaging protocol.
*|----------------------------------------------------------------------------
*/
void ClientMachine(void)
{
    //BaseType_t xReturn;     
    static ServerMsgsTypeEnum ServerMsg =SERVER_MSG_NONE;
    //static TaskHandle_t xHandle;
    
    UINT8 connStatus =SOCKET_IDLE_STATE;    
    UINT8 msgStatus =0;    
    static UINT8 filePut =0;
    
    //static BOOL fwupdateQued =FALSE;

static UINT16 DisconnectCnt =0;
   
    connStatus =CellularSocketState();
    
    if( connStatus ==SOCKET_FAIL_STATE &&
        ClientData.machState !=CLIENT_INIT_STATE
      )
    {
        DisconnectCnt ++;
        /* force all but the init state to shut down if connStatus is SOCKET_DISCONNECT_STATE */
        ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);    
    }
       
    /* spawn the socket connection task, cellular, satellite, ethernet, etc */
    //xTaskCreate( ClientData.pConnectionProtocol, "socket connection task", configMINIMAL_STACK_SIZE, (void *)1 , (tskIDLE_PRIORITY + 1UL), &xHandle); //( TaskHandle_t * ) NULL);
    if( !ClientData.pConnectionProtocol() )
    {
        DisconnectCnt ++;
        /* socket error, close, etc, shutdown??? */
        ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);        
    }
    
#if 0    
    if( CheckPowerManagementSleep() )
    {
        /* power management wants us to sleep, so attempt to 
           shutdown radio/socket gracefully      
        */
        DisconnectCnt ++;        
        ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);          
    }
#endif
    
    switch(ClientData.machState)
    {
        case CLIENT_INIT_STATE:
            //CellularMachineInit();
            ClientMachineInit();
            CellularInitRequestRssi();
            
            ClientServerMessagingInit();
            
            ClientMachineStateProcess(&ClientData, CLIENT_IDLE_STATE);
            break;
        case CLIENT_IDLE_STATE:
            ServerMsg =ServerCheckMessageToSend();
                                    
            if( ServerMsg !=SERVER_MSG_NONE )
            {      
                //PowerUpPeripheral(PWR_CELL_MODULE);
                /* lets power up the GPS as well */
                //PowerUpPeripheral(PWR_GPS_MODULE);
                
                CellularMachineStart();
                ClientMachineStateProcess(&ClientData, CLIENT_NEXT_STATE);
            }    
            break;
        case CLIENT_NEXT_STATE:                                                
            if( connStatus ==SOCKET_CONNECT_STATE )
            {
                /* we have made a socket connection */              
            #if defined(MQTT_MESSAGING)
                MQTTClientStart();
                //ClientMachineStateProcess( &ClientData, CLIENT_CONNECTION_STATE);
            #elif defined(SOCKET_MESSAGING)              
                SocketClientInit();
                //ClientMachineStateProcess( &ClientData, CLIENT_MSG_STATE);
            #elif defined(WEBSOCKET_MESSAGING)                               
                WSMach_ClientInit();
            #endif
                                
                if( ServerCheckMessageToSend() )
                    ClientMachineStateProcess( &ClientData, CLIENT_MSG_STATE);
                else
                    ClientMachineStateProcess( &ClientData, CLIENT_FTP_INIT_STATE);
            }
            /* else in process, we should add a timeout here */
            break;           
        case CLIENT_MSG_STATE:            
            /* communicate to server using our messaging protocol */             
            msgStatus =ClientData.pMsgProtocol(ServerMsg);            

//    #define FIELD_SURVEY_TEST
    #ifdef FIELD_SURVEY_TEST
            if(  msgStatus==1 || msgStatus ==2 )
            {
                ServerClearMessageToSend(ServerMsg);                                   

                /* check for any new messages */
                ServerMsg =ServerCheckMessageToSend();
                
                if( ServerMsg ==SERVER_MSG_NONE )
                {
                    /* no messages to send, let's wait for another with cell and GPS up and running */                                                          
                    ClientMachineStateProcess( &ClientData, CLIENT_WAIT_STATE);
                }
                // else stay here
            }                                
            return;
    #endif
            
//#define FTP_SOCKET_TESTING            
#ifdef FTP_SOCKET_TESTING
            if(  msgStatus==1 )
            {
                ServerClearMessageToSend(ServerMsg);
                                   
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);               
            }               
            else if( msgStatus ==2 )
            {
                /* messaging failure, shut down???, retry??? */
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);               
            }            
#else            
            if(  msgStatus==SOCK_STATUS_PASS )
            {
                ServerClearMessageToSend(ServerMsg);
                    
                /* check for any new messages */
                ServerMsg =ServerCheckMessageToSend();                              
            }               
            else if( msgStatus ==SOCK_STATUS_FAIL )
            {
                /* messaging failure, shut down???, retry??? */
                //ServerClearMessageToSend(ServerMsg);
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);               
            }
            else if( msgStatus ==SOCK_STATUS_RETRY )
            {
                /* retry, so lets stay in this state */
                //ClientMachineStateProcess( &ClientData, CLIENT_MSG_STATE);              
            }            
            // else in progress
            
//    #define SD_AFTER_MESSAGE
    #ifdef SD_AFTER_MESSAGE            
            if( ServerMsg ==SERVER_MSG_NONE )
            {
                /* no messages to send, shut down */
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);
            }            
    #else        
            if( ServerMsg ==SERVER_MSG_NONE )
            {
                /* no messages to send, let's wait for another with 
                   cell and GPS up and running 
                */
                ClientMachineStateProcess( &ClientData, CLIENT_WAIT_STATE);
            }
    #endif
#endif            
            break;            
        case CLIENT_WAIT_STATE:      
        #if defined(WEBSOCKET_MESSAGING)      
            if( !WSMach_CheckForPing() )
            {             
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);                  
                return;
            }
        #endif
              
            ServerMsg =ServerCheckMessageToSend();
            
            if( ServerMsg !=SERVER_MSG_NONE )
            {      
            #ifdef FIELD_SURVEY_TEST
                /* let's shutdown (reset) the cell radio first */
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);
            #else
                ClientMachineStateProcess( &ClientData, CLIENT_MSG_STATE);
            #endif
            }     
            else
            {
                /* check for any FTP messasge */
                ServerMsg =ServerCheckFtpMessageToSend();
                
                if( ServerMsg !=SERVER_MSG_NONE )
                {
                    ClientMachineStateProcess( &ClientData, CLIENT_FTP_INIT_STATE);              
                }
            #if 0
                else
                {
                    /* check for any server initiated messages (listen state) */
                    ClientData.pMsgProtocol(SERVER_MSG_NONE); 
                }
            #endif
            }
            
        #ifdef FIELD_SURVEY_TEST            
            if( (xTaskGetTickCount() -ClientData.timer) >60000 )
            {
                ClientMachineStateProcess( &ClientData, CLIENT_CMD_MODE_STATE);
            }
        #endif
            break;
        case CLIENT_CMD_MODE_STATE:
            if( CellularRequestRssi() )
            {
                ClientMachineStateProcess( &ClientData, CLIENT_WAIT_STATE);
            }
            break;
        case CLIENT_FTP_INIT_STATE:
            FtpSocket_Init();
                   
            if( ServerMsg ==SERVER_MSG_FW_DOWNLOAD )
                filePut =0;
            else 
                filePut =ServerMsg;
            
            ClientData.pMsgProtocol =FtpSocket_Machine;
                
            ClientMachineStateProcess( &ClientData, CLIENT_FTP_STATE);          
            break;
        case CLIENT_FTP_STATE:
            if( ClientData.pMsgProtocol(filePut) )
            {       
                ServerClearFtpMessageToSend(ServerMsg);
              
                ClientData.pMsgProtocol =SocketClientMachine;
                    
                /* let's shutdown/reset radio and start a fresh socket connection */
                ClientMachineStateProcess( &ClientData, CLIENT_SD_STATE);
            }
            break;
        case CLIENT_SD_STATE:
            /* shutdown comms and connections */
            //vTaskDelete(xHandle);
            
            ClientShutdown();
           
            PowerDownPeripheral(PWR_CELL_MODULE);
                                               
            /*!!! 
               should we remove any qued messages that have not been sent
               or keep trying???, 
               for now let's remove them
            !!!*/
            //ServerClearMessageToSend(ServerMsg);
            //ServerClearFtpMessageToSend(ServerMsg);
            
            ClientMachineStateProcess( &ClientData, CLIENT_INIT_STATE);
            break;     
    }    
}






