/** H Header  ******************************************************************
*
* NAME      LogMachine.h
*
* SUMMARY   
*
* TARGET    
*
* TOOLS    
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2019 TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

#ifndef LOG_MACHINE_H
#define LOG_MACHINE_H

#include "types.h"

typedef enum
{
    METER_TCKT_LOG =0,
    DIAGNOSTIC_LOG,
    FIELD_SURVEY_DIAG_LOG,
    PLC_LOG,

    LAST_LOG

}GwLogsEnum;

extern UINT8 LogBuffer[196];

BOOL LogMachineInit(void);

void LogDataStart(GwLogsEnum log);
void LogDataErase(GwLogsEnum log);
BOOL LogDataEraseAll(void);

BOOL LogStopAllLogs(void);
void LogStartAllLogs(void);

UINT32 LogGetStartAddress(GwLogsEnum log);
UINT32 LogGetCurrentAddress(GwLogsEnum log);
UINT32 LogGetEndAddress(GwLogsEnum log);
UINT32 LogGetWrapAddress(GwLogsEnum log);

UINT8 LogGetEraseStatus(GwLogsEnum log);

void LogSetResetReason(UINT32 resetReason);
void LogSetHwErrorFlags(UINT32 hwErrFlags);

void LogMachineTask(void * pvParameters);

void LogCheckDiagnosticLogTime(void);
void LogCheckFieldSurveyDiagLogTime(void);
void LogCheckFillLogTime(void);

void LogResetLogtimer(void);

UINT32 LogGetCurrentAddr(UINT8 log);
UINT32 LogGetStartAddr(UINT8 log);
  
#endif






