/** C Source ******************************************************************
*
* NAME      PCMachine.c
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

#include "PCMachine.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "crc.h"
#include "adcFd.h"

#include "rtcHi.h"

#include "dataFlashFd.h"
#include "GatewayConfig.h"
#include "LogMachine.h"
#include "TLoadMachine.h"
#include "MainControlTask.h"

#include "MlinkMachine.h"

#include "PlcMachine.h"

char SerialTxBuffer[256]; /* transmit buffer */   

void PCProcessCommands(int cmd,char *pBuf);
void PadString(char *pStr, UINT8 strLen, UINT8 nbrBytesPad);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: PCMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PCMachineInit(void)
{
    if( !SciBinaryReceiverInit(SCI_PC_COM, SCI_PC_BAUD_RATE, NULL_PTR, PCProcessCommands) )
    {
        //!!!return FALSE;
    }         
       
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PCProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PCProcessCommands
(
    int cmd,
    char *pRxBuf  /* pointer to the receive buffer */      
)
{          
    RTC_TimeTypeDef RtcTimeStruct;  
    RTC_DateTypeDef RtcDateStruct; 

    GW_CONFIG_STRUCT gwConfig;
    GW_SETUP_STRUCT gwSetup;
      
    ADC_READINGS_STRUCT adcData;
    
    GwLogsEnum logType =(GwLogsEnum)0; 
    
    //char txBuffer[256]; /* transmit buffer */   
    
    UINT8 status =0;        
    UINT16 nbrTxBytes =0;
    UINT32 flashMemoryAddr =0;     
    
    ErrorStatus errStatus =SUCCESS;
    
    /* skip over packet number, only needed for messages that are receiving 
       data from PC */
    //pRxBuf +=2;
    
    memset( SerialTxBuffer, 0x00, sizeof(SerialTxBuffer) );
    
    switch(cmd)
    {
        case CMD_GW_RESET:
            NVIC_SystemReset();
            break;
        case CMD_GW_GET_FW_VERSION:
            strcpy(SerialTxBuffer, FW_VERSION);
            strcat(SerialTxBuffer, __DATE__);
         //   strcat(SerialTxBuffer, __TIME__);
            
            nbrTxBytes =strlen(SerialTxBuffer);
            break;
        case CMD_GW_GET_RTC:
            memset(&RtcTimeStruct, 0x00, sizeof(RtcTimeStruct));
            RtcGetTime( RTC_Format_BIN, &RtcTimeStruct);
          
            memset(&RtcDateStruct, 0x00, sizeof(RtcDateStruct));
            RtcGetDate( RTC_Format_BIN, &RtcDateStruct);  
            
            memcpy(SerialTxBuffer, &RtcTimeStruct, sizeof(RtcTimeStruct));
            memcpy(&SerialTxBuffer[sizeof(RtcTimeStruct)], &RtcDateStruct, sizeof(RtcDateStruct));  
            
            nbrTxBytes =sizeof(RtcTimeStruct) +sizeof(RtcTimeStruct);           
            break;
        case CMD_GW_SET_RTC:          
            /* set the date */
            RtcDateStruct.RTC_WeekDay =0; /* make sure this is 0 */
            
            RtcDateStruct.RTC_Year  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcDateStruct.RTC_Year += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcDateStruct.RTC_Month  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcDateStruct.RTC_Month += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcDateStruct.RTC_Date  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcDateStruct.RTC_Date += (*pRxBuf &0x0f);    *pRxBuf++;            
 
            errStatus =RtcSetDate(RTC_Format_BIN, &RtcDateStruct);            

            /* read back */
            memset(&RtcDateStruct, 0x00, sizeof(RtcDateStruct));
            RTC_GetDate( RTC_Format_BIN, &RtcDateStruct);  
            
            /* set the time */
            RtcTimeStruct.RTC_Hours  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcTimeStruct.RTC_Hours += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcTimeStruct.RTC_Minutes  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcTimeStruct.RTC_Minutes += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcTimeStruct.RTC_Seconds  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcTimeStruct.RTC_Seconds += (*pRxBuf &0x0f);    *pRxBuf++; 
            
            RtcSetTime(RTC_Format_BIN, &RtcTimeStruct);                                   
            break;            
        case CMD_GW_GET_CONFIG:
            GwGetConfig(&gwConfig);
                         
            memcpy(SerialTxBuffer, &gwConfig, sizeof(gwConfig));
            
            nbrTxBytes =sizeof(gwConfig);            
            break;
        case CMD_GW_SET_CONFIG:
            memcpy(&gwConfig, pRxBuf, sizeof(gwConfig));
            
            //gwConfig.crc =CrcCalc16( (unsigned char *)&gwConfig, sizeof(gwConfig)-sizeof(UINT16) );
            GwWriteConfigToFlash(&gwConfig);
            break;              
        case CMD_GW_GET_SETUP:
            GwGetSetup(&gwSetup);
                         
            memcpy(SerialTxBuffer, &gwSetup, sizeof(gwSetup));
            
            nbrTxBytes =sizeof(gwSetup);
            break;
        case CMD_GW_SET_SETUP:
            memcpy(&gwSetup, pRxBuf, sizeof(gwSetup));
                    
            //gwSetup.crc =CrcCalc16( (unsigned char *)&gwSetup, sizeof(gwSetup)-sizeof(UINT16) );
            GwWriteSetupToFlash(&gwSetup);          
            break;
        case CMD_GW_GET_ANALOG:
            AdcGetData(&adcData);
                         
            memcpy(SerialTxBuffer, &adcData, sizeof(adcData));
            
            nbrTxBytes =sizeof(adcData);            
            break;   
        default:
            cmd =0x7fff;
            break;
    }  
    
    /* use NACK to indicate command received, but request failed */ 
    cmd |=ACK;
    
    /* send a packet */
    SciSendPacket(SCI_PC_COM, cmd, nbrTxBytes, SerialTxBuffer);     
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: PadString
*|  Description: pad remainder of a null terminated string with spaces
*|  Retval:
*|----------------------------------------------------------------------------
*/
/*!!!!!!!!! move this to a different file ********/
void PadString(char *pStr, UINT8 strLen, UINT8 nbrBytesPad)
{
    UINT8 j;
    
    /* move pointer to end of string */
    pStr +=strLen;
    
    for(j=strLen; j<nbrBytesPad; j++)
    {
        *pStr ++ =' ';
    }
}




