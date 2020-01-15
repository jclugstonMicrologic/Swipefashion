/** C Source *******************************************************************
*
* NAME      FtpSocket.c
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

#include "gpioHi.h"
#include "rtcHi.h"

#include "dataFlashFd.h"
#include "LogMachine.h"

#include "CellularMachine.h"
//#include "Transport.h"

#include "ServerPacket.h"
#include "MeterTicketMachine.h"
#include "FtpSocket.h"

#include "wdtHi.h"
#include "flash_if.h"

//#define FTP_UPLOAD_DATA_LIMIT 1500000
//#define FTP_GET_SIZE 256

enum
{
    FTP_SOCKET_INIT_STATE =0,
    FTP_SOCKET_CONNECT_STATE,
    
    FTP_SOCKET_PUT_STATE1,
    FTP_SOCKET_PUT_STATE2,
    
    FTP_SOCKET_GET_STATE,
    FTP_SOCKET_CLOSE_STATE,
    FTP_SOCKET_COMPLETE_STATE, 
        
    FTP_SOCKET_ERASE_FLASH,
      
    FTP_SOCKET_WAIT_REPLY_STATE,
    FTP_SOCKET_WAIT_RECV_STATE,

    FTP_SOCKET_TRANSFER_COMPLETE_STATE,
      
    FTP_SOCKET_LAST_STATE
};

enum
{
    STATUS_INIT =0,
    STATUS_OK,
    STATUS_FAIL,
};

typedef struct
{
    UINT32 current;
    UINT32 start;
    UINT32 end;
}LOG_MEMORY_ADDRESS;


LOG_MEMORY_ADDRESS LogMemoryAddress;


typedef struct
{
    UINT8 machState;
    
    UINT32 timer;    
          
}FTP_SOCKET;

FTP_SOCKET FtpSocket;


UINT32 FlashPgmAddress =0;

/** Local Function Prototypes *************************************************/



/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: FtpSocket_Init
*|  Description:
*|----------------------------------------------------------------------------
*/
void FtpSocket_Init(void)
{
    FtpSocket.machState =FTP_SOCKET_INIT_STATE;
}

void FtpSocket_Start(void)
{
    FtpSocket.machState =FTP_SOCKET_INIT_STATE;
}

void FtpSocket_Shutdown(void)
{
    FtpSocket.machState =FTP_SOCKET_INIT_STATE;
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: GetFileToTransfer
*|  Description:
*|----------------------------------------------------------------------------
*/
BOOL GetFileToTransfer(UINT8 file)
{
    int nbrPendingMeterTickets;
    int nbrTckstEndOfMemory =0;
    
    while( !LogStopAllLogs() ){}  

#if 0        
    LogMemoryAddress.current =LogGetCurrentAddress(METER_TCKT_LOG);
    LogMemoryAddress.start =LogGetStartAddress(METER_TCKT_LOG);

    if( (LogMemoryAddress.current-FTP_UPLOAD_DATA_LIMIT) <LogMemoryAddress.start )
    {
        LogMemoryAddress.end =LogMemoryAddress.current;
    }
    else
    {
        LogMemoryAddress.start =(LogMemoryAddress.current-FTP_UPLOAD_DATA_LIMIT);
        LogMemoryAddress.end =LogMemoryAddress.current;
    }  
#endif
    
    if( file ==SERVER_MSG_METER_TCKT )
    {
        nbrPendingMeterTickets =GetNbrPendingMeterTickets();
        
        LogMemoryAddress.start =( LogGetCurrentAddress(METER_TCKT_LOG) -(nbrPendingMeterTickets*0x100) );                      
        
        if( LogMemoryAddress.start <LogGetStartAddress(METER_TCKT_LOG) )
        {
            /* need to get meter data at end of log */
            nbrTckstEndOfMemory =(LogGetStartAddress(METER_TCKT_LOG) -LogMemoryAddress.start)/256;
            
            LogMemoryAddress.start =LogGetEndAddress(METER_TCKT_LOG) -(nbrTckstEndOfMemory*0x100);
        }
        
        LogMemoryAddress.end =LogMemoryAddress.start +0x100;
        
        return TRUE;
    }
    else if( file ==SERVER_MSG_INT_LOG_UPDATE )
    {
        //nbrPendingMeterTickets =GetNbrPendingMeterTickets();
        
        LogMemoryAddress.start =LogGetStartAddress(PLC_LOG); //( LogGetCurrentAddress(PLC_LOG) -(nbrPendingMeterTickets*0x100) );                      
        
    #if 0        
        if( LogMemoryAddress.start <LogGetStartAddress(PLC_LOG) )
        {
            /* need to get meter data at end of log */
            nbrTckstEndOfMemory =(LogGetStartAddress(PLC_LOG) -LogMemoryAddress.start)/256;
            
            LogMemoryAddress.start =LogGetEndAddress(PLC_LOG) -(nbrTckstEndOfMemory*0x100);
        }
    #endif        
        //LogMemoryAddress.end =LogGetCurrentAddress(PLC_LOG); //LogMemoryAddress.start +0x100;
        LogMemoryAddress.end =LogGetEndAddress(PLC_LOG);
        
        return TRUE;
    }    
    else if( file ==SERVER_MSG_STARTUP ||
             file ==SERVER_MSG_SENSOR ||
             file ==SERVER_MSG_GPS
           )
    {
        return TRUE;
    }
    else if( file ==0 )
        return TRUE;  
    else
        return FALSE;
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: CheckUploadStatus
*|  Description:
*|----------------------------------------------------------------------------
*/
BOOL CheckUploadStatus(UINT8 file)
{
    BOOL status =FALSE;
    
    if( file ==SERVER_MSG_METER_TCKT )
    {
        if( LogMemoryAddress.start >=LogMemoryAddress.end )
        {
            /* done */
            LogStartAllLogs();
            status =TRUE; /* done */
        }   
    }    
    else if( file ==SERVER_MSG_INT_LOG_UPDATE )
    {
        if( LogMemoryAddress.start >=LogMemoryAddress.end )
        {
            /* done */
            LogStartAllLogs();
            status =TRUE; /* done */
        }   
    }    
    else if( file ==SERVER_MSG_STARTUP ||
             file ==SERVER_MSG_SENSOR ||
             file ==SERVER_MSG_GPS
           )
    {
        /* done */
        return TRUE;
    } 
    else
        status =FALSE;
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: GetFilePacket
*|  Description:
*|----------------------------------------------------------------------------
*/
void GetFilePacket(UINT8 file, char *pDataBuf)
{
    if( file ==SERVER_MSG_METER_TCKT )
    {
        DataFlashRead(LogMemoryAddress.start, 256, (UINT8 *)pDataBuf);
        
        LogMemoryAddress.start +=256;
    }
    else if( file ==SERVER_MSG_INT_LOG_UPDATE )
    {
        DataFlashRead(LogMemoryAddress.start, 256, (UINT8 *)pDataBuf);
        
        LogMemoryAddress.start +=256;
    }    
    else if( file ==SERVER_MSG_STARTUP )
    {
        strcpy( pDataBuf, "GW START UP\n");
    }
    else if( file ==SERVER_MSG_SENSOR )
    {
        strcpy( pDataBuf, "GW SENSOR\n");
    }
    else if( file ==SERVER_MSG_GPS )
    {
        strcpy( pDataBuf, "GW GPS\n");
    }
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: EraseFwSection
*|  Description:
*|----------------------------------------------------------------------------
*/
void EraseFwSection(void)
{
#ifdef USE_EXTERNAL_FLASH  
    int block =0;
    
    for(block=FW_FILE_ADDR_START/DATAFLASH_SIZE_BLOCK_BYTES; block<FW_FILE_ADDR_END/DATAFLASH_SIZE_BLOCK_BYTES; block++) 
    {
        DataFlashEraseBlock(block);               
    }
#endif    
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: SaveFwFile
*|  Description:
*|----------------------------------------------------------------------------
*/
void SaveFwFile(void)
{
#ifdef USE_EXTERNAL_FLASH  
    DataFlashWrite( LogMemoryAddress.start, FTP_GET_SIZE, (UINT8 *)&FtpRecvArray );

    LogMemoryAddress.start +=FTP_GET_SIZE;
#else    
    #if 1
    while( (FLASH_GetStatus() == FLASH_BUSY)){}
                 
    for(int j=0; j<256; j+=8)
    {
        FLASH_If_ProgramWord(FlashPgmAddress, (uint32_t*)&FtpRecvArray[j]);
        
        FlashPgmAddress +=8;
    }
    #endif
#endif    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GetFwUpdateStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT32 GetFwUpdateStatus(void)
{
    return RtcReadBackupData(RTC_BACKUP_FWUPDATE_STATUS_ADDRESS);
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SetNbrPendingMeterTickets
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SetFwUpdateStatus(UINT32 status)
{   
    RtcWriteBackupData(RTC_BACKUP_FWUPDATE_STATUS_ADDRESS, status);
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: FtpSocket_Machine
*|  Description:
*|----------------------------------------------------------------------------
*/
int FtpSocket_Machine(UINT8 file)
{
    int status =STATUS_INIT;
    int len =0;
    int mysock;        
    int cellResponse =0;
    
    static char payloadMsg[256];
    
    mysock =SCI_CELLULAR_COM;
               
    memset( payloadMsg, 0x00, sizeof(payloadMsg) );

    switch(FtpSocket.machState)
    {
        case FTP_SOCKET_INIT_STATE:     
            CellularInitFtpSocket();
            
            FtpSocket.timer =xTaskGetTickCount();
            
            FtpSocket.machState =FTP_SOCKET_CONNECT_STATE;
            break;
        case FTP_SOCKET_CONNECT_STATE:     
            if( CellularConnectFtpSocket(file) )
            {
                if( file ==0 )
                {                                       
                    FlashPgmAddress =0x8080000;                                        

                    /* flash unlock SERVER_MSG_FW_DOWNLOAD*/                                       
                    FLASH_If_FlashUnlock();
                #if 0
                    FLASH_If_EraseSectors( (FlashPgmAddress &0x00fffff) );
                #endif                    
                    
                    SetFwUpdateStatus(0x00000000);
                
                    FtpSocket.machState =FTP_SOCKET_GET_STATE;                     
                }
                else  if( GetFileToTransfer(file) ) 
                {                  
                    FtpSocket.machState =FTP_SOCKET_PUT_STATE2; 
                }
                else
                {
                    /* return fail status */
                    status =STATUS_FAIL;                
            
                    FtpSocket.machState =FTP_SOCKET_INIT_STATE;                                   
                }
            }
            else
            {
                FtpSocket.timer =xTaskGetTickCount();
                FtpSocket.machState =FTP_SOCKET_WAIT_REPLY_STATE;
            }
            break;
        case FTP_SOCKET_WAIT_REPLY_STATE:    
             cellResponse =CellularGetResponse();
             
             if( cellResponse ==CELL_REPLY_OK ||
                 cellResponse ==CELL_REPLY_CONNECT
               )
             {
                 CellularClearResponse();
                 FtpSocket.machState =FTP_SOCKET_CONNECT_STATE;
             }
             else if( (xTaskGetTickCount() - FtpSocket.timer) >90000 ||
                       cellResponse ==CELL_REPLY_ERROR                     
                    )
             {
                 /* no reply, return fail status */
                 status =STATUS_FAIL;                
            
                 FtpSocket.machState =FTP_SOCKET_INIT_STATE;   
             }
            break;
        case FTP_SOCKET_PUT_STATE1:
            CellularClearResponse();
            strcpy( payloadMsg,"AT#FTPAPPEXT=16,0\r\n");
            len =strlen( (char *)payloadMsg);                                     
            SciTxPacket(mysock, len, (char *)payloadMsg);     
            FtpSocket.machState =FTP_SOCKET_PUT_STATE2;            
            break;          
        case FTP_SOCKET_PUT_STATE2:
#if 0
            cellResponse =CellularGetResponse();
            
            if( cellResponse ==CELL_REPLY_OK )          
            {
                GetFilePacket(file, payloadMsg);
                  
                while( CELL_CTS )
                {       
                    /* cell busy */
                    if( (xTaskGetTickCount() -FtpSocket.timer )>1000 )
                    {
                        /* failure, taking too long */
                        return STATUS_FAIL;
                    }
                }
                
                len =16;
                SciTxPacket(mysock, len, (char *)payloadMsg);  
                
                FtpSocket.timer =xTaskGetTickCount();
                      
                if( CheckUploadStatus(file) )
                {
                    /* done */
                    FtpSocket.machState =FTP_SOCKET_TRANSFER_COMPLETE_STATE;
                }                  
                else
                    FtpSocket.machState =FTP_SOCKET_PUT_STATE1;
            }
                        
#else          
            /* we have a file to send to FTP server */
            len =256;
                        
            while( 1 )
            {           
                GetFilePacket(file, payloadMsg);
                  
                while( CELL_CTS )
                {       
                    /* cell busy */
                    if( (xTaskGetTickCount() -FtpSocket.timer )>5000 )
                    {
                        /* failure, taking too long */
                        return STATUS_FAIL;
                    }
                }
                
                SciTxPacket(mysock, len, (char *)payloadMsg);  
                
                FtpSocket.timer =xTaskGetTickCount();
                      
                if( CheckUploadStatus(file) )
                {
                    /* done */
                    break;
                }                                  
                
                /* special case, allow WDT kick here */
                KickWdt(); 
            }    

    #define FTP_CLOSE_FTP_SOCKET            
    #ifdef FTP_CLOSE_FTP_SOCKET
            FtpSocket.machState =FTP_SOCKET_CLOSE_STATE;
    #else       
            FtpSocket.machState =FTP_SOCKET_TRANSFER_COMPLETE_STATE;
    #endif                       
#endif
            break;
        case FTP_SOCKET_GET_STATE:                                      
            CellularClearResponse();
                                    
            strcpy( payloadMsg,"AT#FTPRECV=256\r\n");
            len =strlen( (char *)payloadMsg);                                     
            SciTxPacket(mysock, len, (char *)payloadMsg); 
            
            FtpSocket.timer =xTaskGetTickCount();
            FtpSocket.machState =FTP_SOCKET_WAIT_RECV_STATE;
            break;
        case FTP_SOCKET_WAIT_RECV_STATE:
            cellResponse =CellularGetResponse();
            
            if( cellResponse ==CELL_REPLY_OK )
            {
                if( (FlashPgmAddress %2048) ==0 )
                    FLASH_If_EraseSectors( (FlashPgmAddress &0x00fffff) );
                
                /* write data to flash */
                SaveFwFile();
               
                FtpSocket.machState =FTP_SOCKET_GET_STATE;                
            }
            
            if( GetFtpStatus() )
            {    
                status =STATUS_OK;
                
                /* bootloader will read this status flag to determine if there  
                   is a fw update available in memory
                */                
                SetFwUpdateStatus(0x00000001);   
                
                FLASH_Lock();
                   
            #if 0
                strcpy( payloadMsg,"AT#FTPDELE=\"Gateway.hex\"\r\n");
                len =strlen( (char *)payloadMsg);                                     
                SciTxPacket(mysock, len, (char *)payloadMsg);  
            #endif
            
                /* let's reset here so new program will be flashed */
                NVIC_SystemReset();
                
                FtpSocket.machState =FTP_SOCKET_COMPLETE_STATE;                
            }            
            else if( (xTaskGetTickCount() - FtpSocket.timer) >10000 ||
                      cellResponse ==CELL_REPLY_ERROR
                   )
            {    
                /* ftp download has failed */
                status =STATUS_FAIL;       
               
                FLASH_Lock();
                
                FtpSocket.machState =FTP_SOCKET_COMPLETE_STATE;
            }            
            break;
        case FTP_SOCKET_CLOSE_STATE:
            /* wait a bit before sending the command mode message */
            if( (xTaskGetTickCount() -FtpSocket.timer) >1500 )   
            {
                /* this will close the file on the FTP server */
                CellularSendCmdMode();
             
                ServerClearFtpMessageToSend(file);
                
                FtpSocket.timer =xTaskGetTickCount();
                 
                FtpSocket.machState =FTP_SOCKET_COMPLETE_STATE;
            }
            break;
        case FTP_SOCKET_COMPLETE_STATE:                                
            if( CellularGetResponse() ==CELL_REPLY_NONE )
            {                                       
                if( (xTaskGetTickCount() -FtpSocket.timer )>5000 )
                {
                    status =STATUS_FAIL;            
            
                    FtpSocket.machState =FTP_SOCKET_INIT_STATE;                                                        
                    
                    return status;
                }
            }
            else
            {
    #if 1
                strcpy( payloadMsg,"AT#FTPCLOSE\r\n");
                len =strlen( (char *)payloadMsg);                                     
                SciTxPacket(mysock, len, (char *)payloadMsg);  
    #endif                        
                
                while( CellularGetResponse() ==CELL_REPLY_NONE ){}

                status =STATUS_OK;            
                
                FtpSocket.machState =FTP_SOCKET_INIT_STATE;                        
            }
            break;            
        case FTP_SOCKET_TRANSFER_COMPLETE_STATE:
            status =STATUS_OK;            
                
            FtpSocket.machState =FTP_SOCKET_INIT_STATE;                                  
            break;
    }    
    
    return status;
}


