/** C Source ******************************************************************
*
* NAME      SciFd.c
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


/** Include Files *************************************************************/

#include "SciFd.h"
#include "sciHi.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "sysTimers.h"


/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: SciCheckTimeout
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SciCheckTimeout
(
    COMMON *pCommonData,
    UINT32 timeout  //msec
)
{
   TickType_t xTicks=xTaskGetTickCount();
   
   if( (xTicks - pCommonData->timeoutTimer) >timeout )
   {
      return TRUE;
   }
   
   return FALSE;   
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: SciStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciStateProcess
(
    COMMON *pCommonData,
    //UINT8 prevState,
    UINT8 nextState
)
{
    TickType_t xTicks=xTaskGetTickCount();

    pCommonData->prevState =pCommonData->machState;//prevState;
    pCommonData->machState =nextState;
		
    pCommonData->timeoutTimer =xTicks;	
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: SciTxPacket
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SciTxPacket
(	
    UINT8 sciPort,
    UINT16 nbrBytes,
    char *pPckt
)
{    
    UINT16 byteCnt;
    
    for(byteCnt =0; byteCnt<nbrBytes; byteCnt++)
    {
//TimerDelayUs(10000);
        SciSendByte(sciPort, *pPckt);   
        
        *pPckt ++;
    }
    
    return TRUE;
}






