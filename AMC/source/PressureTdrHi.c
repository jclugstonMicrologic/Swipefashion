/** C Source ******************************************************************
*
* NAME      PressureTdr.c
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


/** Include Files *************************************************************/

#include "FreeRTOS.h"
#include "timers.h"

#include "math.h"

#include "gpioHi.h"
#include "spiHi.h"
#include "PressureTdrHi.h"

#define BLADDER_OVER_PRESSURE ((float)20.67) //3psi

typedef struct
{
  int8_t p11;
  int8_t p10;
  int16_t p9;
  int8_t p8;
  int8_t p7;
  
  uint16_t p6;
  uint16_t p5;  
  
  int8_t p4;
  int8_t p3;

  int16_t p2;
  int16_t p1;
  
  int8_t t3;  
  uint16_t t2;
  uint16_t t1;
  
}press_temp_coeff_t;    

typedef struct
{
  float p11;
  float p10;
  float p9;
  float p8;
  float p7;
  
  float p6;
  float p5;  
  
  float p4;
  float p3;

  float p2;
  float p1;
  
  float t3;
  
  float t2;
  float t1;
  
}press_temp_coeff_float_t;    

press_temp_coeff_float_t PTCoeff[NBR_TRANSDUCERS];


//float PressureValue[8];
//float TemperatureValue[8];

UINT8 TdrPresent =0;

press_sensor_data_t PSensorData[8];


BOOL PressureTdr_GetCoeff(UINT8 tdrNbr,press_temp_coeff_float_t *pCoeff);
void PressureTdr_StartPeriodicRead(void);

/** Functions *****************************************************************/



// https://github.com/DFRobot/DFRobot_BMP388/blob/master/DFRobot_BMP388.cpp
// https://github.com/DFRobot/DFRobot_BMP388


/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_Init
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PressureTdr_Init( void )
{
    UINT8 regData =0;
    UINT8 chipId =0;
    
    TdrPresent =0;
    memset(&PTCoeff, 0x00, sizeof(PTCoeff));
    
    regData =BMP3_NORMAL_MODE | BMP3_TEMP_EN | BMP3_PRESS_EN; // press on,temp on,normal mode
    
    SpiInit(PRESS_TDR_SPI_PORT);

    for(int tdr =0; tdr<NBR_TRANSDUCERS; tdr++)
    {
        PressureTdr_Read(BMP3_CHIP_ID_ADDR,tdr, 1, &chipId);

        if( chipId ==BMP3_CHIP_ID )
        {
            PressureTdr_GetCoeff(tdr, &PTCoeff[tdr]);    
            PressureTdr_Write(BMP3_PWR_CTRL_ADDR, tdr, 1, &regData);            
            
            TdrPresent |= (1<<tdr);
        }
    }
    
    PressureTdr_StartPeriodicRead();
}

void PressureTdr_GetPressTemp(press_sensor_data_t *pData)
{
//    pPr =PSensorData;
    memcpy(pData, PSensorData, sizeof(PSensorData));
}

BOOL PressureTdr_CheckOverPres(void)
{
    BOOL status =FALSE;
    
    for(int j=0; j<NBR_TRANSDUCERS; j++)
    {
        if( (PSensorData[j].press -PSensorData[7].press)>BLADDER_OVER_PRESSURE )
        {
            CloseValve(j+1);
            status =TRUE;
        }
        else
            status =FALSE;
    }
    
     return status;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_Read_Timer_Callback
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
static void PressureTdr_Read_Timer_Callback (void * pvParameter)
{      
    UINT8 sensorPresent =0;
    sensorPresent =PressureTdr_GetTdrs();
    
    for(int sensor =0; sensor<NBR_TRANSDUCERS; sensor++)
    {
        if( sensorPresent & (0x01<<sensor) )
            PressureTdr_ReadPT(sensor, &PSensorData[sensor].press, &PSensorData[sensor].temp);
        else
        {
            PSensorData[sensor].press =-1;
            PSensorData[sensor].temp  =-1;
        }
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_StartPeriodicRead
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PressureTdr_StartPeriodicRead(void)
{
    #define TIMER_PERIOD      500          /**< Timer period (msec) */
    /* Start timer for LED1 blinking */
    TimerHandle_t read_timer_handle; 
    read_timer_handle =xTimerCreate( "TdrRead", TIMER_PERIOD, pdTRUE, NULL, PressureTdr_Read_Timer_Callback);
    xTimerStart(read_timer_handle, 0);
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_Read
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PressureTdr_Read(UINT8 addr, UINT8 tdrNbr, UINT8 nbrBytes, UINT8 *pDataBuf)
{
    UINT8 device;
    UINT8 byteCnt=0;    
    
    device =tdrNbr;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_PTDR( addr |0x80 );    
    SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    
    /* read data */    
    for(byteCnt =0; byteCnt<nbrBytes; byteCnt++)
    {
        *pDataBuf =SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
        
        *pDataBuf++;
    }
          
    /* negate cs */
    spiStop(device);
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_Write
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PressureTdr_Write(UINT8 addr, UINT8 tdrNbr, UINT8 nbrBytes, UINT8 *pDataBuf)
{
    UINT8 device;
    UINT8 byteCnt=0;    
    
    device =tdrNbr;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_PTDR( addr );    
    
    /* read data */    
    for(byteCnt =0; byteCnt<nbrBytes; byteCnt++)
    {
        SPI_TRANSFER_PTDR( *pDataBuf );
        
        *pDataBuf++;
    }
          
    /* negate cs */
    spiStop(device);
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_GetCoeff
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PressureTdr_GetCoeff(UINT8 tdrNbr, press_temp_coeff_float_t *pFloatCoeff)
{
    UINT8 tempbuf[21];
    
    static press_temp_coeff_t ptcoeff;    
       
    PressureTdr_Read(BMP3_CALIB_DATA_ADDR,tdrNbr, 21, tempbuf);
    
    ptcoeff.t1 =tempbuf[1]<<8 | tempbuf[0];
    ptcoeff.t2 =tempbuf[3]<<8 | tempbuf[2];
    ptcoeff.t3 =tempbuf[4];

    ptcoeff.p1 =tempbuf[6]<<8 | tempbuf[5];
    ptcoeff.p2 =tempbuf[8]<<8 | tempbuf[7];
    
    ptcoeff.p3 =tempbuf[9];
    ptcoeff.p4 =tempbuf[10];
    
    ptcoeff.p5 =tempbuf[12]<<8 | tempbuf[11];
    ptcoeff.p6 =tempbuf[14]<<8 | tempbuf[13];

    ptcoeff.p7 =tempbuf[15];
    ptcoeff.p8 =tempbuf[16];
    
    ptcoeff.p9 =tempbuf[18]<<8 | tempbuf[17];
    
    ptcoeff.p10 =tempbuf[19];
    ptcoeff.p11 =tempbuf[20];

    /* convert coefficients to floating point values */
    double tempvalue=0.0;
    
    tempvalue=0.00390625;
    pFloatCoeff->t1 =((float)ptcoeff.t1/tempvalue);
    
    tempvalue =1073741824;
    pFloatCoeff->t2 =((float)ptcoeff.t2/tempvalue);
   
    tempvalue =281474976710656;
    pFloatCoeff->t3 =((float)ptcoeff.t3/tempvalue);

    tempvalue =1048576;    
    pFloatCoeff->p1 =(((float)ptcoeff.p1-16384)/tempvalue);
    
    tempvalue =536870912;
    pFloatCoeff->p2 =(((float)ptcoeff.p2-16384)/tempvalue);
    
    tempvalue =4294967296;
    pFloatCoeff->p3 =((float)ptcoeff.p3/tempvalue);
    
    tempvalue =137438953472;
    pFloatCoeff->p4 =((float)ptcoeff.p4/tempvalue);

    tempvalue =0.125;
    pFloatCoeff->p5 =((float)ptcoeff.p5/tempvalue);

    tempvalue =64;
    pFloatCoeff->p6 =((float)ptcoeff.p6/tempvalue);    
    
    tempvalue =256;
    pFloatCoeff->p7 =((float)ptcoeff.p7/tempvalue);

    tempvalue =32768;
    pFloatCoeff->p8 =((float)ptcoeff.p8/tempvalue);

    tempvalue =281474976710656;
    pFloatCoeff->p9 =((float)ptcoeff.p9/tempvalue);        
    
    tempvalue =281474976710656;
    pFloatCoeff->p10 =((float)ptcoeff.p10/tempvalue);

    tempvalue =4294967296;
    pFloatCoeff->p11 =((float)ptcoeff.p11/tempvalue);        
    tempvalue =8589934592;
    pFloatCoeff->p11 /=tempvalue;
    
    return TRUE;
}
 
/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdr_ReadPT
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PressureTdr_ReadPT(UINT8 tdrNbr, float *pPress, float *pTemp)
{
    UINT8 tempbuf[6];
    UINT32 utemp, upress;
    
    double partialdata1, partialdata2;
    double partialdata3, partialdata4;    
    double out1, out2;
    
    float temp;
    PressureTdr_Read(BMP3_PRESS_DATA_ADDR,tdrNbr, 6, tempbuf);
    
    upress =(tempbuf[2]<<16) | (tempbuf[1] <<8)| tempbuf[0];
    utemp  =(tempbuf[5]<<16) | (tempbuf[4] <<8)| tempbuf[3];    
   
     /* get transducer coefficients */
    //PressureTdr_GetCoeff(tdrNbr, &PtCoeff[tdrNbr]);

    /* compensate raw temperature */
    partialdata1 =(double)(utemp-PTCoeff[tdrNbr].t1);
    partialdata2 =(double)(partialdata1*PTCoeff[tdrNbr].t2);
    
    temp =partialdata2 + partialdata1*partialdata1*PTCoeff[tdrNbr].t3;      
  
    /* compensate raw pressure */
    partialdata1 =PTCoeff[tdrNbr].p6 * temp;
    partialdata2 =PTCoeff[tdrNbr].p7 * temp*temp;
    partialdata3 =PTCoeff[tdrNbr].p8 * temp*temp*temp;
    out1 =PTCoeff[tdrNbr].p5 +partialdata1+partialdata2+partialdata3;
    
    partialdata1 =PTCoeff[tdrNbr].p2 * temp;
    partialdata2 =PTCoeff[tdrNbr].p3 * temp*temp;
    partialdata3 =PTCoeff[tdrNbr].p4 * temp*temp*temp;
      
    out2 =(float)upress*(PTCoeff[tdrNbr].p1 +partialdata1+partialdata2+partialdata3);

    partialdata1 =(float)upress*(float)upress;
    partialdata2 =PTCoeff[tdrNbr].p9 + PTCoeff[tdrNbr].p10*temp;
    partialdata3 =partialdata1*partialdata2;
    partialdata4 =partialdata3 +(float)upress*(float)upress*(float)upress*PTCoeff[tdrNbr].p11;
    
    /* return kPa */
    *pPress =(out1+out2+partialdata4)/1000.0;

    /* return DegC */
    *pTemp =temp;
        
    return TRUE;
}

UINT8 PressureTdr_GetTdrs(void)
{
    return TdrPresent;
}
