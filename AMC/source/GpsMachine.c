/** C Source ******************************************************************
*
* NAME      GpsMachine.c
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
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/
#include "GpsMachine.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "rtcHi.h"
#include "gpioHi.h"

#include "PowerManagement.h"

#include <stdlib.h>

GPS_DATA_STRUCT GpsData;

BOOL FirstFix =FALSE;

/** Functions *****************************************************************/

void GpsProcessCommands(int cmd,char *pCcmd);

void GpsParseGGAMsg(UINT16 byteCnt,char *pRxBuf,GPS_DATA_STRUCT *pGpsData);
void GpsParseVTGMsg(UINT16 byteCnt,char *pRxBuf,GPS_DATA_STRUCT *pGpsData);
void GpsParseRMCMsg(UINT16 byteCnt,char *pRxBuf,GPS_DATA_STRUCT *pGpsData);
void GpsParseGSVMsg(UINT16 byteCnt,char *pRxBuf,GPS_DATA_STRUCT *pGpsData);

UINT8 CalcUbloxChecksum(void);

BOOL GpsCalcChecksum(char *pBuf);
void GpsSetRtcWithGpsDateTime(void);

void IntToAscii(int value,char *pAscii);


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GpsMachineInit(void)
{
    //char txBuf[32], j=0;
    
    if( !SciAsciiReceiverInit(SCI_GPS_COM, SCI_GPS_BAUD_RATE, GpsProcessCommands, NULL_PTR) )
    {
        //!!!
        return FALSE;
    }         
            
    PowerUpPeripheral(PWR_GPS_MODULE);
           
    CalcUbloxChecksum();
      
#if 0    
    txBuf[j++] =0xb5;
    txBuf[j++] =0x62;    
    txBuf[j++] =0x06;
    txBuf[j++] =0x17;
    txBuf[j++] =0x00;
       
    txBuf[j++] =0x1d;
    txBuf[j++] =0x40;    
    
    SciTxPacket(SCI_GPS_COM, j, txBuf);  
#endif    
    
    memset( &GpsData, 0x00, sizeof(GpsData) );
        
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpsProcessCommands
(
    int cmd,
    char *pRxBuf
)
{  
    /* calculate checksum here */   
    //static char *p =0;
    
    if( !GpsCalcChecksum(pRxBuf) )
        return;
    
    if( strstr(pRxBuf,"$GPGGA") !=0 )
    {
        /* parse data, Global Positioning System Fix Data */       
//        p = strstr(pRxBuf,"$GPGGA");
        
//        GpsCalcChecksum(p);
        
        GpsParseGGAMsg(strlen(pRxBuf), pRxBuf, &GpsData);
    }
    if( strstr(pRxBuf,"$GPVTG") !=0 )
    {
        /* parse data, GPS Satellites in view */
        GpsParseVTGMsg(strlen(pRxBuf), pRxBuf, &GpsData);
    }         
    if( strstr(pRxBuf,"$GPGSV") !=0 )
    {
        /* parse data, GPS Satellites in view */
        GpsParseGSVMsg(strlen(pRxBuf), pRxBuf, &GpsData);
    }     

    if( strstr(pRxBuf,"$GPRMC") !=0 )
    {
        /* parse data, GPS Satellites in view */
        GpsParseRMCMsg(strlen(pRxBuf), pRxBuf, &GpsData);
    }  
    
#ifdef NEMA_ZDA_MSG_ENABLED    
    if( strstr(pRxBuf,"$GPZDA") !=0 )
    {
        /* parse data, GPS time zone ofset is suppose to be in this message, 
           currently not enabled */
        GpsParseZDAMsg(strlen(pRxBuf), pRxBuf, &GpsData);
    } 
#endif
      
    /* echo for debugging */
//    SciTxPacket(SCI_PC_COM, strlen(pRxBuf), pRxBuf);

#if 0
    if( GpsGetStatus() )
    {
        GpsSetRtcWithGpsDateTime();
    }
#endif    
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: GetGpsData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpsGetGpsData
(
    GPS_DATA_STRUCT *pGpsData
)
{
    memcpy(pGpsData, &GpsData, sizeof(GPS_DATA_STRUCT));
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsGetLatitude
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
float GpsGetLatitude
(
    void
)
{
    static float lat=0;
    //static float latMin=0;
 
    if( GpsData.info.status[0] == '0' ) 
        lat = -1;
    else
    {    
        /* format from GPS is ddmm.mmmmm */
        lat =(float)atof(GpsData.info.latitude);
        //latMin =(float)(atof(&GpsData.info.latitude[2])/60.0);

        if( GpsData.info.latDir[0] =='S')
            lat = -lat;
    }
    
    return lat;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsGetLongitude
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
float GpsGetLongitude
(
    void
)
{
    float lon =0;
 
    if( GpsData.info.status[0] == '0' ) 
        lon = -1;
    else
    {
        lon =(float)atof(GpsData.info.longitude);
        //lonMin =(float)(atof(&GpsData.info.longitude[3])/60.0);
        if( GpsData.info.longDir[0] =='W')
            lon = -lon;
    }
                        
    return lon;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GpsGetAltitude
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
float GpsGetAltitude
(
    void
)
{
    float alt =0;
    
#if 0    
    GpsData.info.altitude[0] ='1';
    GpsData.info.altitude[1] ='0';
    GpsData.info.altitude[2] ='0';
    GpsData.info.altitude[3] ='1';
    GpsData.info.altitude[4] ='.';
    GpsData.info.altitude[5] ='1';
    GpsData.info.altitude[6] =0;
    GpsData.info.status[0] = '1';
#endif
    
    if( GpsData.info.status[0] == '0' ) 
        alt  = -1;  
    else 
    {        
        alt =(float)atof(GpsData.info.altitude);
    }
    
    return alt;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsGetSpeed
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
float GpsGetSpeed
(
    void
)
{
    float speed =0;
        
    if( GpsData.info.status[0] == '0' ) 
        speed  = -1;  
    else 
    {        
        speed =(float)atof(GpsData.info.speed);
    }
    
    return speed;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GpsGetStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GpsGetStatus
(
    void
)
{
    if(GpsData.info.status[0] =='0')
        return FALSE;
    else
        return TRUE;
        
}

/*
*|----------------------------------------------------------------------------
*|  Module: GpsMachine
*|  Routine: GpsParseGGAMsg
*|  Description:
*|   Parse the GGA message into its appropriate fields
*|                 GPGGA fields used are
*|                     UTC time
*|                     Lat, Long, Alt
*|                     Status (GPS quality)
*|                     Number of satellites
*|----------------------------------------------------------------------------
*/
void GpsParseGGAMsg
(
   UINT16 byteCnt,  // the number of bytes in the buffer
   char *pRxBuf,    // the data buffer, includes all overhead
   GPS_DATA_STRUCT *pGpsData
)
{
   UINT8 index;
   UINT8 commaSkip;
   UINT8 commaCnt =0;
   //char thirdDecPos; // GPS structure only has two decimal time position, just use this for third

   for( index=0; index<byteCnt; index++)
   {
      if( *(pRxBuf++) == ',' )
      {
         switch( commaCnt )
         {
            case 0:
               // get time
               memcpy( &pGpsData->dateTime, pRxBuf, 6 );

#if 0
               if( pGpsData->dateTime.hour[0] !=0x2c // , ,
                             &&
                   pGpsData->startRtc ==1
                 )
               {
                  // first time we have received a valid
                  // GPS time, set system time to this time.
                  // from this point on, system time will be
                  // updated every second on the PPS interrupt
                  SetSystemTime(pGpsData, 0);

                  pGpsData->startRtc =2;
               }
#endif
               break;
            case 1:
               memcpy( &pGpsData->info.latitude, pRxBuf, sizeof(pGpsData->info.latitude) );
               //TerminateString( pGpsData->info.latitude, sizeof(pGpsData->info.latitude) );
               break;
            case 2:
               pGpsData->info.latDir[0] =*pRxBuf;
               pGpsData->info.latDir[1]= 0; // null terminate
               break;
            case 3:
               memcpy( &pGpsData->info.longitude, pRxBuf, sizeof(pGpsData->info.longitude) );
               //TerminateString( pGpsData->info.longitude, sizeof(pGpsData->info.longitude) );
               break;
            case 4:
               pGpsData->info.longDir[0] =*pRxBuf;
               pGpsData->info.longDir[1] =0x00; // null terminate
               break;
            case 5:
               pGpsData->info.status[0] = *pRxBuf;
               pGpsData->info.status[1] =0x00; // null terminate
               break;
            case 6:
               // convert the two char value into a decimal value now
               // only if not ','
               if( *pRxBuf != ',' )
               {
                  // valid data in this field
                  pGpsData->info.numSats[0] =  (*(pRxBuf ++) -0x30) *10;
                  pGpsData->info.numSats[0] += *(pRxBuf)-0x30;
                  pGpsData->info.numSats[1] =0x00; // null terminate
               }
               else
               {
                  pGpsData->info.numSats[0] =0x00; // null terminate
                  pGpsData->info.numSats[1] =0x00; // null terminate
               }
               break;
            case 7:
               // horz. dilution of precision 0.5 to 99.9 (HDOP)
               memcpy( pGpsData->info.hdop, pRxBuf, sizeof(pGpsData->info.hdop) );

               // filter HDOP
               for( commaSkip =0; commaSkip< sizeof(pGpsData->info.hdop); commaSkip++ )
               {
                  if( pGpsData->info.hdop[commaSkip] ==',' )
                  {
                     pGpsData->info.hdop[commaSkip] =0;
                     break;
                  }
               }
               break;
            case 8:
               // altitude in meters
               memcpy( &pGpsData->info.altitude, pRxBuf, sizeof(pGpsData->info.altitude) );
               //TerminateString( pGpsData->info.altitude, sizeof(pGpsData->info.altitude) );
               break;
            case 10:
               memcpy( &pGpsData->info.geoidalSep, pRxBuf, sizeof(pGpsData->info.geoidalSep) );
               //TerminateString( pGpsData->info.geoidalSep, sizeof(pGpsData->info.geoidalSep) );
               break;
            case 12:
               if( *pRxBuf ==',' )
               {
                  // rtcm field is empty, use -1 as default
                  pGpsData->info.rtcm[0] = '-';
                  pGpsData->info.rtcm[1] = '1';
               }
               else
                  memcpy( &pGpsData->info.rtcm, pRxBuf, sizeof(pGpsData->info.rtcm) );
               break;
            default:
               break;
         }
         commaCnt ++;
      }
   }

} // end ParseGGAMsg()


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsParseVTGMsg
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpsParseVTGMsg
(
   UINT16 byteCnt,  // the number of bytes in the buffer
   char *pRxBuf,    // the data buffer, includes all overhead
   GPS_DATA_STRUCT *pGpsData
)
{
    UINT8 index;
    UINT8 commaCnt =0;

    for( index=0; index<byteCnt; index++)
    {
        if( *(pRxBuf++) == ',' )
        {
            switch( commaCnt )
            {
                case 6:
                    // get speed
                    memcpy( &pGpsData->info.speed, pRxBuf, 6 );
            }
            
            commaCnt ++;
        }
    }  
    
    return;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsParseRMCMsg
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpsParseRMCMsg
(
   UINT16 byteCnt,  // the number of bytes in the buffer
   char *pRxBuf,    // the data buffer, includes all overhead
   GPS_DATA_STRUCT *pGpsData
)
{
    UINT8 index;
    UINT8 commaCnt =0;

    for( index=0; index<byteCnt; index++)
    {
        if( *(pRxBuf++) == ',' )
        {
            switch( commaCnt )
            {
                case 8:
                    // get date
                    memcpy( &pGpsData->dateTime.date, pRxBuf, 6 );
            }
            
            commaCnt ++;
        }
    }  
    
    return;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GpsParseGSVMsg
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpsParseGSVMsg
(
   UINT16 byteCnt,  // the number of bytes in the buffer
   char *pRxBuf,    // the data buffer, includes all overhead
   GPS_DATA_STRUCT *pGpsData
)
{   
    return;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GpsCalcChecksum
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GpsCalcChecksum(char *p)
{
    int chksum =0;
    char rxchksum[2];
    char asciiChksum[2];       
            
    for(int k=0; k<100; k++)
    {        
        *p ++;
           
        if( *p=='*' )
        {          
            strncpy(rxchksum, &p[1], 2);            
            break;
        }
            
        chksum ^= *p;            
    } 
    
    /* convert the integer calculated checksum to ascii 
       for comparison with received string (no itoa library)
    */
    IntToAscii(chksum,asciiChksum);
    
    if( (asciiChksum[0] ==rxchksum[0]) &&
        (asciiChksum[1] ==rxchksum[1])
      )
    {        
        return TRUE;
    }
    else 
        return FALSE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GpsSetRtcWithGpsDateTime
*|  Description: GPS time is UTC
*|  Retval:
*|----------------------------------------------------------------------------
*/
void GpsSetRtcWithGpsDateTime(void)
{
    RTC_TimeTypeDef rtcTimeStruct;  
    RTC_DateTypeDef rtcDateStruct;   
         
    /* set the date */
    rtcDateStruct.RTC_WeekDay =0; /* make sure this is 0 */
    rtcDateStruct.RTC_Year   =(GpsData.dateTime.year[0] &0x0f)*10;
    rtcDateStruct.RTC_Year  +=(GpsData.dateTime.year[1] &0x0f);
    
    rtcDateStruct.RTC_Month  =(GpsData.dateTime.month[0]&0x0f)*10;
    rtcDateStruct.RTC_Month +=(GpsData.dateTime.month[1]&0x0f);
    
    rtcDateStruct.RTC_Date   =(GpsData.dateTime.date[0]&0x0f)*10;
    rtcDateStruct.RTC_Date  +=(GpsData.dateTime.date[1]&0x0f);
            
    RtcSetDate(RTC_Format_BIN, &rtcDateStruct);
    
    /* set the time */
    rtcTimeStruct.RTC_Hours  =(GpsData.dateTime.hour[0] &0x0f)*10; 
    rtcTimeStruct.RTC_Hours +=(GpsData.dateTime.hour[1] &0x0f);
    
    rtcTimeStruct.RTC_Minutes  =(GpsData.dateTime.min[0]&0x0f)*10;
    rtcTimeStruct.RTC_Minutes +=(GpsData.dateTime.min[1]&0x0f);
    
    rtcTimeStruct.RTC_Seconds  =(GpsData.dateTime.sec[0]&0x0f)*10;
    rtcTimeStruct.RTC_Seconds +=(GpsData.dateTime.sec[1]&0x0f);
            
    RtcSetTime(RTC_Format_BIN, &rtcTimeStruct);
}

         
/*
*|----------------------------------------------------------------------------
*|  Routine: IntToAscii
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void IntToAscii
(
    int value,
    char *pAscii
)
{
    /* make check sum ASCII */
    *pAscii  =((value &0xf0)>>4);

    if( *pAscii <=9 )
        *pAscii +='0';
    else
        *pAscii +=55;

    *pAscii ++;

    *pAscii  =(value &0x0f);

    if( *pAscii <=9 )
        *pAscii +='0';
    else
        *pAscii +=55;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: CalcUbloxChecksum
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT8 CalcUbloxChecksum(void)
{
    static UINT8 CK_A = 0, CK_B = 0;
    char  rxBuf[128];
    UINT8 j=0;
    
    
    rxBuf[j++] =0x06;
    rxBuf[j++] =0x17;
    rxBuf[j++] =0x00;
    
    for(j=0; j<3; j++)
    {
        CK_A =((CK_A + rxBuf[j])&0xff);
        CK_B =((CK_B + CK_A)&0xff);
    }  
    
    return 0;
}


