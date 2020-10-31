/** C Source ******************************************************************
*
* NAME      PCMachine.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded worbench for ARM v8.20.2
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2020, MICROLOGIC
* Calgary, Alberta, Canada, www.micrologic.ab.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "PCMachine.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "crc.h"
#include "adcFd.h"

#include "rtcHi.h"
#include "gpioHi.h"

#include "dataFlashFd.h"
#include "AmcConfig.h"
#include "LogMachine.h"

#include "PressureTdrHi.h"
#include "MainControlTask.h"

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

    AMC_CONFIG_STRUCT amcConfig;
    AMC_SETUP_STRUCT amcSetup;
       
    press_sensor_data_t pressSensorData[8];
    
    UINT16 pressSensorPsi[8];

    UINT8 valveNbr =0;
    UINT8 boardId =0;
    UINT16 nbrTxBytes =0;
    
    //ErrorStatus errStatus =SUCCESS;
    
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
 
            //errStatus =RtcSetDate(RTC_Format_BIN, &RtcDateStruct);            

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
            AmcGetConfig(&amcConfig);
                         
            memcpy(SerialTxBuffer, &amcConfig, sizeof(amcConfig));
            
            nbrTxBytes =sizeof(amcConfig);            
            break;
        case CMD_GW_SET_CONFIG:
            memcpy(&amcConfig, pRxBuf, sizeof(amcConfig));
            break;              
        case CMD_GW_GET_SETUP:
            AmcGetSetup(&amcSetup);
                         
            memcpy(SerialTxBuffer, &amcSetup, sizeof(amcSetup));
            
            nbrTxBytes =sizeof(amcSetup);
            break;
        case CMD_GW_SET_SETUP:
            memcpy(&amcSetup, pRxBuf, sizeof(amcSetup));                   
            break;
        case CMD_GW_GET_ANALOG:
            break;   
        case CMD_OPEN_VALUE:
            valveNbr  =*pRxBuf++;
            
            for(int j=0;j<8; j++)
            {
                if( valveNbr&(0x01<<j) )
                    OpenValve(j+1);
            }
            break;
        case CMD_CLOSE_VALUE:
            valveNbr  =*pRxBuf++;

            for(int j=0;j<8; j++)
            {
                if( valveNbr& (0x01<<j) )
                    CloseValve(j+1);
            }                       
            break;            
        case CMD_GET_PRESS_TEMP:             
            PressureTdr_GetPressTemp(pressSensorData);
            
            memcpy(SerialTxBuffer, &pressSensorData, sizeof(pressSensorData));            
            nbrTxBytes =sizeof(pressSensorData);         
            break;
        case CMD_GET_PRESS:
#if 0          
            sensorPresent =PressureTdr_GetTdrs();
            
            for(int j=0; j<NBR_TRANSDUCERS; j++)
            {
                if( sensorPresent & (0x01<<j) )
                {
                    PressureTdr_ReadPT(j, &pressSensorData[j].press, &pressSensorData[j].temp);
                    
                    /* 16bit value to reduced data packet 
                       also convert to psi, and three decimal
                    */
                    pressSensorPsi[j] =(UINT16)(pressSensorData[j].press/0.00689476);                    
                }
                else
                {
                    pressSensorData[j].press =-1;
                    pressSensorData[j].temp  =-1;
                    
                    pressSensorPsi[j] =-1;
                }                      
            }            
#endif             
            PressureTdr_GetPressTemp(pressSensorData);
      
            for(int j=0; j<NBR_TRANSDUCERS; j++)
            {
                pressSensorPsi[j] =(UINT16)(pressSensorData[j].press/0.00689476);
            }
            
            memcpy(SerialTxBuffer, &pressSensorPsi, sizeof(pressSensorPsi));            
            nbrTxBytes =sizeof(pressSensorPsi);     
            break;            
        case CMD_GET_BRD_ID:
            boardId =BOARD_ID;

            memcpy(SerialTxBuffer, &boardId, sizeof(boardId));            
            nbrTxBytes =sizeof(boardId);                
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




