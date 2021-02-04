/** H Header  ******************************************************************
*
* NAME      SerialBinaryMachine.H
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded wrobench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


#ifndef SCI_BINARY_MACHINE_H
#define SCI_BINARY_MACHINE_H

#include "types.h"
#include "sciFd.h"
#include "sciHi.h"

#define SCI_DLE (UINT16)0x0010 //DLE
#define SCI_STX (UINT16)0x0002 //STX
#define SCI_ETX (UINT16)0x0003 //ETX

#define SIZEOF_HEAD ((UINT16)6) // [DLE][STX][nbrByteMSB][nbrByteLSB][cmdMSB][cmdLSB]
#define SIZEOF_TAIL (2)         // [crcMSB][crcLSB]
#define SIZEOF_HEAD_TAIL (SIZEOF_HEAD +SIZEOF_TAIL)

#define SCI_START_OF_HEADER_INFO  (UINT8)2

#define MODBUS_SIZEOF_RTU_HEAD      ((UINT8)3)
#define MODBUS_START_OF_HEADER_INFO ((UINT8)1)

#define HART_SIZEOF_HEADER        ((UINT8)8)
#define HART_SIZEOF_PREAMBLE      ((UINT8)5)
#define HART_SIZEOF_CHKSUM        ((UINT8)1)

#define MLINK_SOM_MSB (UINT16)0x00a0
#define MLINK_SOM_LSB (UINT16)0x00a1

#define MLINK_EOM_MSB (UINT16)0x00b0
#define MLINK_EOM_LSB (UINT16)0x00b1

#define MLINK_SIZEOF_HEAD ((UINT16)5) // [somMSB][somLSB][lenMSB][lenLSB][id]
#define MLINK_SIZEOF_TAIL (4)         // [crcMSB][crcLSB]
#define MLINK_SIZEOF_HEAD_TAIL (MLINK_SIZEOF_HEAD +MLINK_SIZEOF_TAIL)

#define MLINK_START_OF_HEADER_INFO   (UINT8)2

BOOL SciBinaryReceiverInit(UINT8 sciPort,UINT32 baudRate, sci_data_t *pDataCom, void (*pCallBack)(int, char *) );
BOOL SciBinaryStartReceiver(void);
void SciSendPacket(UINT8 sciPort, UINT16 cmd, UINT16 nbrBytes, char *pPayload);

int MlinkBinaryRxMachine(sci_data_t *pSerialData,char sciPort);
void SendMlinkPacket(UINT8 sciPort, UINT16 cmd, UINT16 nbrBytes, char *pPayload);
void SendMlinkAsciiPacket(UINT8 sciPort, UINT16 nbrBytes, char *pPayload);

UINT8 Sci_GetAsciiString(char sciPort,char *pString);

#endif


