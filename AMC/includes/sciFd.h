/** H Header  ******************************************************************
*
* NAME      SciFd.h
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


#ifndef SCI_FD_H
#define SCI_FD_H

#include "types.h"

typedef struct
{
    UINT32 timeoutTimer;
    UINT8 machState;
    UINT8 subState;
    UINT8 prevState;

}COMMON;


typedef struct
{
    UINT16 command;
    UINT16 nbrBytes;
    UINT16 rxPacketNbr;
    UINT16 calcChecksum;

    UINT16 byteCnt;

    UINT16 termChar;
    
    char rxBuffer[256];
    
    COMMON common;
  
    void (*pCmdFunction)(int, char *);
    void (*pCmdAltFunction)(int, char *);
    
    UINT16 (*pCheckSumFunction)(UINT8 *, UINT16);
    
}sci_data_t;


#define SCI_BLUETOOTH_COM        USART_1
#define SCI_BLUETOOTH_BAUD_RATE  115200

#define SCI_PC_COM              UART_4
#define SCI_PC_BAUD_RATE        115200 //230400

   
BOOL SciCheckTimeout(COMMON *pCommonData,UINT32 timeout);
void SciStateProcess(COMMON *pCommonData,UINT8 nextState);
BOOL SciTxPacket(UINT8 sciPort,UINT16 nbrBytes,char *pPckt);

#endif



