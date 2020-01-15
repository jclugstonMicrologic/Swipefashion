/** C Source ******************************************************************
*
* NAME      Transport.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "sciHi.h"
#include "sciFd.h"

#include "MQTTClient.h"
#include "transport.h"


/*
*|----------------------------------------------------------------------------
*|  Routine: Transport.c
*|  Description: transport_init
*|  retval:
*|----------------------------------------------------------------------------
*/
void transport_init(void)
{  
    /*
    ***********************
     L O C A L   D A T A
    ***********************
    */

    /*
    *************************
     E R R O R   C H E C K S
    *************************
    */


    /*
    *************************
     C O D E
    *************************
    */
    //SciSocketInit();
        
}


/*
*|----------------------------------------------------------------------------
*|  Module: Transport.c
*|  Routine: transport_getdata
*|  Description:
*|----------------------------------------------------------------------------
*/
int transport_getdata(unsigned char* buf, int count)
{
    TickType_t timeoutTimer =0;       
    TickType_t xTicks=xTaskGetTickCount();

    timeoutTimer =xTicks;    
    
    while( !SciSocketRecv(buf, count) )
    {
        xTicks=xTaskGetTickCount();
      
        if( (xTicks -timeoutTimer) >100 ) //100
        {
            /* wait a bit, but not too long */
            timeoutTimer =xTicks;
            count =0;
            break;
        }
    }
    
    return count;    
}


/*
*|----------------------------------------------------------------------------
*|  Module: Transport.c
*|  Routine: transport_getbyte
*|  Description:
*|----------------------------------------------------------------------------
*/
int transport_getbyte(unsigned char* buf)
{   
    return SciSocketRecv(buf,1);
}

/*
*|----------------------------------------------------------------------------
*|  Module: Transport.c
*|  Routine: transport_checksocketDisconnect
*|  Description:
*|----------------------------------------------------------------------------
*/
int transport_checksocketDisconnect()
{
    return 1;
}

/*
*|----------------------------------------------------------------------------
*|  Module: Transport.c
*|  Routine: transport_sendPacketBuffer
*|  Description:
*|----------------------------------------------------------------------------
*/
int transport_sendPacketBuffer(int mysock, char *buf, int len)
{
    if(mysock ==SCI_CELLULAR_COM ) 
        SciTxPacket(mysock, len, (char *)buf);  
    
    return len;
}





