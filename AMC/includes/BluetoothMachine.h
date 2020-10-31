/** H Header  ******************************************************************
*
* NAME      ZigbeeMachine.h
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

#ifndef ZIGBEE_MACHINE_H
#define ZIGBEE_MACHINE_H

#include "types.h"
//#include "SciAsciiMachine.h"
#include "SciBinaryMachine.h"

BOOL Ble_MachineInit(void);
BOOL Ble_Machine(void);

#endif




