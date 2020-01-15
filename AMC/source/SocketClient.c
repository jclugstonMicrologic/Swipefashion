/** C Source *******************************************************************
*
* NAME      SocketClient.c
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
* Copyright (c) 2018, TRIG
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

#include "gpioHi.h"

#include "dataFlashFd.h"
#include "LogMachine.h"

#include "CellularMachine.h"
#include "SocketClient.h"
#include "Transport.h"
#include "ServerPacket.h"

enum
{
    SOCKET_INIT_STATE =0,
    
    SOCKET_WAIT_HANDSHAKE,
    SOCKET_CLIENT_MSG_STATE,
    SOCKET_WAIT_SERVER_RESPONSE_STATE,
    SOCKET_WAIT_NACK_STATE,

    SOCKET_LAST_STATE
};

int SocketClientMachState =0;

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: SocketClientInit
*|  Description:
*|----------------------------------------------------------------------------
*/
void SocketClientInit(void)
{
   SocketClientMachState =SOCKET_INIT_STATE;
}

void SocketClientStart(void)
{
   SocketClientMachState =1;
}

void SocketClientShutdown(void)
{
    SocketClientMachState =SOCKET_INIT_STATE;
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: SocketClientMachine
*|  Description:
*|----------------------------------------------------------------------------
*/
int SocketClientMachine(UINT8 msg)
{
    int status =90;
    int mysock;
    int len;
    static unsigned char payloadMsg[200];
    static UINT32 timer =0;
    
    mysock =SCI_CELLULAR_COM;
    
    switch(SocketClientMachState)
    {
        case SOCKET_INIT_STATE:
            /* we have a message to send */
            ServerBuildMessage( (char *)payloadMsg, (ServerMsgsTypeEnum)msg); 
    
            len =strlen( (char *)payloadMsg);
            transport_sendPacketBuffer(mysock, ( char *)payloadMsg, len);
              
            timer =xTaskGetTickCount();
            
            /* wait for server ACK (just timeout for now) */
            while( (xTaskGetTickCount() -timer) < 1000 ){}
            
            status =1;
            break;
            
    }    
    return status;
}

