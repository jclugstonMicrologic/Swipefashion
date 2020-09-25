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


//#define SCI_ZIGBEE_COM          USART_3
//#define SCI_ZIGBEE_BAUD_RATE    115200

#define SCI_BLUETOOTH_COM        USART_1
#define SCI_BLUETOOTH_BAUD_RATE  115200

#define SCI_PC_COM              UART_4
#define SCI_PC_BAUD_RATE        115200 //230400

#define SCI_MLINK_COM           UART_5
#define SCI_MLINK_BAUD_RATE     57600

#define SCI_MTR_TCKT_COM        SCI_MLINK_COM
#define SCI_MTR_TCKT_BAUD_RATE  19200

#define SCI_GPS_COM             UART_4 
#define SCI_GPS_BAUD_RATE       9600
//#define SCI_IRIDIUM_COM         USART_2 
//#define SCI_IRIDIUM_BAUD_RATE   19200

#define SCI_HART_COM            USART_2
#define SCI_HART_BAUD_RATE      1200
    

BOOL SciCheckTimeout(COMMON *pCommonData,UINT32 timeout);
void SciStateProcess(COMMON *pCommonData,UINT8 nextState);
BOOL SciTxPacket(UINT8 sciPort,UINT16 nbrBytes,char *pPckt);

#endif



