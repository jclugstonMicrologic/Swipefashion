/** H Header  ******************************************************************
*
* NAME      PressureTdr.h
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

#ifndef PRESSURE_TDR_H
#define PRESSURE_TDR_H

#include "types.h"

#define PRESS_TDR_SPI_PORT    SPI_PORT2
#define PRESS_TDR_SPI_PERIPH  SPI_PERIPHERAL2

#define SPI_TRANSFER_PTDR(txByte)    SpiTransferSpi2(txByte)

void PressureTdrInit(void);
BOOL PressureTdrRead(UINT8 addr, UINT8 nbrBytes, UINT8 *pDataBuf);

#endif




