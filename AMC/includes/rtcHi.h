/** H header  ******************************************************************
*
* NAME      rtcHi.h
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

#ifndef RTC_HI
#define RTC_HI

#include "types.h"
#include "stm32f4xx.h"

#define RTC_BACKUP_NBR_MTR_TCKTS_ADDRESS        0 
#define RTC_BACKUP_FWUPDATE_STATUS_ADDRESS      4 

BOOL RtcInit(void);

ErrorStatus RtcSetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
ErrorStatus RtcSetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

void RtcGetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
void RtcGetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

uint32_t RtcReadBackupData(uint32_t addr);
void RtcWriteBackupData(uint32_t addr, uint32_t data);

void RtcStartWakeupTimer(UINT32 wakeupTimeSec);

uint32_t RtcGetEpochTime(void);

#endif








