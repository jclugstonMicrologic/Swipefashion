/** H header  ******************************************************************
*
* NAME      crc.h
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
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

#ifndef CRC_H
#define CRC_H

#include "types.h"
#include "stm32f4xx.h"

void CrcInit32(void);

UINT16 CrcCalc16(UINT8 *pData, UINT16 nbrBytes);
UINT32 CrcCalc32(UINT8 *pData, UINT16 nbrBytes);

#endif


