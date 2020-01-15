/** C Source ******************************************************************
*
* NAME      rtcHi.c
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


#include "rtcHi.h"

#include <time.h>                
/*
*|----------------------------------------------------------------------------
*|  Routine: RtcInit
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
BOOL RtcInit(void)
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */


   /*
   *************************
    E R R O R   C H E C K S
   *************************
   */


   /*
   *************************
    C O D E
   *************************
   */
    RTC_InitTypeDef RTC_InitStructure;
//RTC_TimeTypeDef RTC_TimeStructure;
//RTC_DateTypeDef RTC_DateStructure;
    
    /* Enable write access to the RTC ****************************/
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
    /* Configure the RTC clock source ****************************/
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */   
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
        /* !!! timeout, and RTC_DeInit(); */
    }  
 
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
    
    /* Wait for RTC APB registers synchronisation */
    if( RTC_WaitForSynchro() != SUCCESS )
    {
        RTC_DeInit();
        return FALSE;
    }
    
    /* Configure the RTC calendar, Time and Date *****************/
    /* RTC time base = LSE / ((AsynchPrediv+1) * (SynchPrediv+1))
    = 1 Hz
    */
    RTC_InitStructure.RTC_AsynchPrediv = 127;
    RTC_InitStructure.RTC_SynchPrediv = 255;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
    
#if 0
    /* Set the Time */
    RTC_TimeStructure.RTC_Hours = 0x08;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

    /* Set the Date */
    RTC_DateStructure.RTC_Month = RTC_Month_July;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
    RTC_DateStructure.RTC_Date = 0x02;
    RTC_DateStructure.RTC_Year = 0x18;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
    
    RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);    
    RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);    
#endif
    //RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);        
    //RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);        
       
    return TRUE;
}


/******************************************************************************
* NAME      RtcGetEpochTime()
*
* SUMMARY   
*
* INPUTS    None
*
* OUTPUTS   None
*******************************************************************************/
uint32_t RtcGetEpochTime(void)
{
    time_t epoch;
    struct tm tm;

    RTC_TimeTypeDef RtcTimeStruct;  
    RTC_DateTypeDef RtcDateStruct; 
    RTC_GetTime(RTC_Format_BIN, &RtcTimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RtcDateStruct); 
    
    tm.tm_year =(RtcDateStruct.RTC_Year +100);
    tm.tm_mon =RtcDateStruct.RTC_Month-1;
    tm.tm_mday =RtcDateStruct.RTC_Date;

    tm.tm_hour =RtcTimeStruct.RTC_Hours;
    tm.tm_min =RtcTimeStruct.RTC_Minutes;
    tm.tm_sec =RtcTimeStruct.RTC_Seconds;

    /* tm_isdst, greater than zero if Daylight Saving Time is in effect, 
       zero if Daylight Saving Time is not in effect, 
       and less than zero if the information is not available */
    tm.tm_isdst =-1;
        
    epoch = mktime(&tm);
  
    //if(epcoh <1540000000 || epoch>2000000000)
        //epcoh =1540000000;
       
    return (uint32_t)epoch;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: RtcSetTime
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
ErrorStatus RtcSetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
{
    return RTC_SetTime(RTC_Format, RTC_TimeStruct);  
}


/*
*|----------------------------------------------------------------------------
*|  Routine: RtcSetDate
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
ErrorStatus RtcSetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
{
    return RTC_SetDate(RTC_Format, RTC_DateStruct);  
}


/*
*|----------------------------------------------------------------------------
*|  Routine: RtcGetTime
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void RtcGetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
{
    RTC_GetTime(RTC_Format, RTC_TimeStruct);  
}


/*
*|----------------------------------------------------------------------------
*|  Routine: RtcGetDate
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void RtcGetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
{
    RTC_GetDate(RTC_Format, RTC_DateStruct);  
}


/*
*|----------------------------------------------------------------------------
*|  Routine: RtcReadBackupData
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
uint32_t RtcReadBackupData(uint32_t addr)
{
    return RTC_ReadBackupRegister(addr);
}

/*
*|----------------------------------------------------------------------------
*|  Routine: RtcWriteBackupData
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void RtcWriteBackupData(uint32_t addr, uint32_t data)
{
    RTC_WriteBackupRegister(addr, data);
}


/*
*|----------------------------------------------------------------------------
*|  Routine: RtcStartWakeupTimer
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void RtcStartWakeupTimer(UINT32 wakeupTimeSec)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    EXTI_InitTypeDef EXTI_InitStructure;
    
    RTC_WakeUpCmd(DISABLE);
    
    /* EXTI configuration for wakeup timer */
    EXTI_ClearITPendingBit(EXTI_Line20);
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* interrupt configuration for wakeup timer */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);      
    
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_SetWakeUpCounter(wakeupTimeSec);    
    
    /* Clear pending flags */
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line20);
    
    /* enable RTC wakeup interrupt */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
    /* enable wakeup counter */
    RTC_WakeUpCmd(ENABLE);
     
}

/*
*|----------------------------------------------------------------------------
*|  Routine: RtcStopWakeupTimer
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void RtcStopWakeupTimer(void)
{
    /* disable RTC wakeup interrupt */
    NVIC_DisableIRQ(RTC_WKUP_IRQn);
    RTC_ITConfig(RTC_IT_WUT, DISABLE);
    
    /* stop timer */
    RTC_WakeUpCmd(DISABLE);
}


/*
*|----------------------------------------------------------------------------
*|  Routine: RTC_WKUP_IRQHandler
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void RTC_WKUP_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
    {
        /* acknowledge the interrupt */
        RTC_ClearITPendingBit(RTC_IT_WUT);
        EXTI_ClearITPendingBit(EXTI_Line20);
        
        RtcStopWakeupTimer();
    } 
}


