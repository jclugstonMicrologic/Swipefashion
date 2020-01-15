/** C Source ******************************************************************
*
* NAME      MeterTicketMachine.c
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
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include "MeterTicketMachine.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "crc.h"
#include "adcFd.h"
#include "rtcHi.h"
#include "LogMachine.h"
#include "MainControlTask.h"

UINT8 LogBuffer[256];

UINT8 MeterTcktDataPending =0;

void MeterTcktProcessCommands(int cmd,char *pBuf);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: MeterTcktMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL MeterTcktMachineInit(void)
{ 
    if( !SciBinaryReceiverInit(SCI_MTR_TCKT_COM, SCI_MTR_TCKT_BAUD_RATE, MeterTcktProcessCommands) )
    {
        //!!!
        return FALSE;
    }         
       
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: MeterTcktProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void MeterTcktProcessCommands
(
    int cmd,
    char *pRxBuf  /* pointer to the receive buffer */      
)
{
    switch(cmd)
    {
        case 0:            
            MeterTcktDataPending =GetNbrPendingMeterTickets();
            
            MeterTcktDataPending ++;
            
            SetNbrPendingMeterTickets(MeterTcktDataPending);
            
            memset(LogBuffer, 0x00, sizeof(LogBuffer) );
            memcpy(LogBuffer, pRxBuf, sizeof(LogBuffer) );        
 
            /* invoke log machine (log data to NV memory) */
            LogDataStart(METER_TCKT_LOG);
            break;
        default:
            cmd =0x7fff;
            break;
    }      
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

