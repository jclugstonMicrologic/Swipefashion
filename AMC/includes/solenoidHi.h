/** H Header  ******************************************************************
*
* NAME      gpio.h
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v8.20.2
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2020, MICROLOGIC
* Calgary, Alberta, Canada, www.micrologic.ab.ca
*******************************************************************************/


#ifndef SOLENOID_H
#define SOLENOID_H

/* Includes ------------------------------------------------------------------*/

#define NBR_VALVES      7

#define COMPR_LO 0x01
#define COMPR_HI 0x02

#define RELIEF_LO 0x01
#define RELIEF_HI 0x02


void Solenoid_Init(void);

void Solenoid_OpenValve(uint8_t valveNbr);
void Solenoid_CloseValve(uint8_t valveNbr);

void Solenoid_OpenReliefValve(uint8_t reliefValve);
void Solenoid_CloseReliefValve(uint8_t reliefValve);
void Solenoid_CloseAllValves(void);

void Solenoid_StartCompr(uint8_t compr);
void Solenoid_StopCompr(uint8_t compr);
void Solenoid_SetCompr(uint8_t cState);

#endif


