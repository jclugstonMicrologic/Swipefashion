/** H Header  ******************************************************************
*
* NAME      SciAsciiMachine.H
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


#ifndef SCI_ASCII_MACHINE_H
#define SCI_ASCII_MACHINE_H

#include "types.h"
#include "sciFd.h"
#include "sciHi.h"

typedef enum
{
    AT_CMD_UNKNOWN =0,
    AT_CMD_CSQ,
    AT_CMD_CPIN,
          
      
    AT_CMD_LAST
      
}AtCmdEnum;

BOOL SciAsciiReceiverInit(UINT8 sciPort,UINT32 baudRate,void (*pCallBack)(int, char *), void (*pAltCallBack)(int, char *));
BOOL SciAsciiStartReceiver(void);

#endif


