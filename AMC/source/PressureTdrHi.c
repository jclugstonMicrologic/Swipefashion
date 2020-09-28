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
#include "spiHi.h"
#include "PressureTdrHi.h"

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
  
  double p4;
  double p3;

  double p2;
  double p1;
  
  double t3;
  
  double t2;
  double t1;
  
}press_temp_coeff_float_t;    

press_temp_coeff_float_t PTCoeff;


float PressureValue[8];
float TemperatureValue[8];

BOOL PressureTdr_GetCoeff(UINT8 tdrNbr);

/** Functions *****************************************************************/


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
       
    regData =BMP3_NORMAL_MODE | BMP3_TEMP_EN | BMP3_PRESS_EN; //0x33; // press on,temp on,normal mode
    
    SpiInit(PRESS_TDR_SPI_PORT);

    PressureTdr_Read(BMP3_CHIP_ID_ADDR,0, 1, &chipId);
    PressureTdr_Read(BMP3_CHIP_ID_ADDR,7, 1, &chipId);
    
    PressureTdr_GetCoeff(0);
    PressureTdr_GetCoeff(7);
          
    PressureTdr_Write(BMP3_PWR_CTRL_ADDR, 0, 1, &regData);
    PressureTdr_Write(BMP3_PWR_CTRL_ADDR, 7, 1, &regData);
    
}

// https://github.com/DFRobot/DFRobot_BMP388/blob/master/DFRobot_BMP388.cpp
// https://github.com/DFRobot/DFRobot_BMP388

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
    //SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    //SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    
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
    //SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    //SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    //SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    
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
BOOL PressureTdr_GetCoeff(UINT8 tdrNbr)
{
    UINT8 tempbuf[21];
    
    static press_temp_coeff_t ptcoeff;    
       
    PressureTdr_Read(BMP3_CALIB_DATA_ADDR,tdrNbr, 21, tempbuf);
//    ptcoeff =(press_temp_coeff_t *)tempbuf;        
    //memcpy(&ptc, tempbuf, 21);
    
    ptcoeff.t1 =tempbuf[1]<<8 | tempbuf[0];
    ptcoeff.t2 =tempbuf[3]<<8 | tempbuf[2];
    ptcoeff.t3 =tempbuf[4];
#if 1
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
#endif
    double tempvalue=0.0;
    
    tempvalue=0.00390625;
    PTCoeff.t1 =((double)ptcoeff.t1/tempvalue);
    
    tempvalue =1073741824;
    PTCoeff.t2 =((double)ptcoeff.t2/tempvalue);
    
    tempvalue =281474976710656;
    PTCoeff.t3 =((double)ptcoeff.t3/tempvalue);
    
    tempvalue =1048576;    
    PTCoeff.p1 =(((double)ptcoeff.p1-16384)/tempvalue);
    
    tempvalue =536870912;
    PTCoeff.p2 =(((double)ptcoeff.p2-16384)/tempvalue);
    
    tempvalue =4294967296;
    PTCoeff.p3 =((double)ptcoeff.p3/tempvalue);
    
    tempvalue =137438953472;
    PTCoeff.p4 =((double)ptcoeff.p4/tempvalue);

    tempvalue =0.125;
    PTCoeff.p5 =((double)ptcoeff.p5/tempvalue);

    tempvalue =64;
    PTCoeff.p6 =((double)ptcoeff.p6/tempvalue);    
    
    tempvalue =256;
    PTCoeff.p7 =((double)ptcoeff.p7/tempvalue);

    tempvalue =32768;
    PTCoeff.p8 =((double)ptcoeff.p8/tempvalue);

    tempvalue =281474976710656;
    PTCoeff.p9 =((double)ptcoeff.p9/tempvalue);        
    
    tempvalue =281474976710656;
    PTCoeff.p10 =((double)ptcoeff.p10/tempvalue);

    tempvalue =4294967296;
    PTCoeff.p11 =((double)ptcoeff.p11/tempvalue);        
    tempvalue =8589934592;
    PTCoeff.p11 /=tempvalue;
       
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
    
    float temp;
    PressureTdr_Read(BMP3_PRESS_DATA_ADDR,tdrNbr, 6, tempbuf);
    
    upress =(tempbuf[2]<<16) | (tempbuf[1] <<8)| tempbuf[0];
    utemp  =(tempbuf[5]<<16) | (tempbuf[4] <<8)| tempbuf[3];
    
    
    PressureTdr_GetCoeff(tdrNbr);
        
    double partialdata1, partialdata2;
    double partialdata3, partialdata4;
    
    double out1, out2;

    /* compensate raw temperature */
    partialdata1 =(double)(utemp-PTCoeff.t1);
    partialdata2 =(double)(partialdata1*PTCoeff.t2);
    
    temp =partialdata2 + partialdata1*partialdata1*PTCoeff.t3;      
      
    /* compensate raw pressure */
    partialdata1 =PTCoeff.p6 * temp;
    partialdata2 =PTCoeff.p7 * temp*temp;
    partialdata3 =PTCoeff.p8 * temp*temp*temp;
    out1 =PTCoeff.p5 +partialdata1+partialdata2+partialdata3;
    
    partialdata1 =PTCoeff.p2 * temp;
    partialdata2 =PTCoeff.p3 * temp*temp;
    partialdata3 =PTCoeff.p4 * temp*temp*temp;
      
    out2 =(float)upress*(PTCoeff.p1 +partialdata1+partialdata2+partialdata3);

    partialdata1 =(float)upress*(float)upress;
    partialdata2 =PTCoeff.p9 + PTCoeff.p10*temp;
    partialdata3 =partialdata1*partialdata2;
    partialdata4 =partialdata3 +(float)upress*(float)upress*(float)upress*PTCoeff.p11;
    
    /* return kPa*/
    *pPress =(out1+out2+partialdata4)/1000.0;
     
    *pTemp =temp;
        
    return TRUE;
}



