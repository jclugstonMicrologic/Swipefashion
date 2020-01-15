/** C Source ******************************************************************
*
* NAME      accelHi.c
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

#include "accelHi.h"
#include "spiHi.h"


/** Functions *****************************************************************/
BOOL AccelReadBytes(UINT8 cmd, UINT8 nbrBytes, UINT8 *pRxData);
UINT8 AccelReadByte(UINT8 cmd);
BOOL AccelWriteRegister(UINT8 reg, UINT8 value);

/*
*|----------------------------------------------------------------------------
*|  Routine: AccelInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AccelInit(void)
{      
    SpiInit(ACCEL_SPI_PORT);
 
    AccelGetChipId();
   
    AccelWriteRegister(LIS3DH_REG_CTRL_REG1, LIS3DH_CTRL_REG1_ODR_1250HZ | LIS3DH_CTRL_REG1_XYZ_ENABLE);
    AccelWriteRegister(LIS3DH_REG_CTRL_REG4, LIS3DH_CTRL_REG4_FSCALE);               
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: AccelGetChipId
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AccelGetChipId(void)
{
    if( AccelReadByte(LIS3DH_REG_WHO_AM_I) ==LIS3DH_REG_WHO_AM_I_VALUE )
        return TRUE;
    else
        return FALSE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: AccelGetAccelData
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AccelGetAccelData(AccelDataStruct *ptAccelData)
{
   // static AccelDataStruct accelData;
    UINT8                  dataBuffer[7];
    
    int cmd =(LIS3DH_REG_STATUS_REG2 & 0x3F) | LIS3DH_REG_MASK_MULTIPLE;
    
    AccelReadBytes(cmd, sizeof(dataBuffer), dataBuffer);
    
    ptAccelData->bStatus  = dataBuffer[0];
    ptAccelData->iwAccelX = (dataBuffer[1] | (dataBuffer[2] << 8));
    ptAccelData->iwAccelY = (dataBuffer[3] | (dataBuffer[4] << 8));
    ptAccelData->iwAccelZ = (dataBuffer[5] | (dataBuffer[6] << 8));

    // Shift bits for high resolution mode
    ptAccelData->iwAccelX >>= 4;
    ptAccelData->iwAccelY >>= 4;
    ptAccelData->iwAccelZ >>= 4;
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: AccelReadBytes
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AccelReadBytes(UINT8 cmd, UINT8 nbrBytes, UINT8 *pRxData)
{      
    int j;    
    
    SpiDeviceInit(ACCEL_SPI_PERIPH);
    
    /* assert cs */
    spiStart(ACCEL_SPI_PERIPH);
        
    SPI_TRANSFER_ACCEL( (cmd |LIS3DH_REG_MASK_READ) );       
    //SPI_TRANSFER_ACCEL( (LIS3DH_REG_STATUS_REG2 & 0x3F) | LIS3DH_REG_MASK_MULTIPLE |LIS3DH_REG_MASK_READ );       
    
    for(j=0; j<nbrBytes; j++)
    {
        *pRxData ++ =SPI_TRANSFER_ACCEL(SPI_DUMMY_BYTE );
    }

    spiStop(ACCEL_SPI_PERIPH);

    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: AccelWriteRegister
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AccelWriteRegister(UINT8 reg, UINT8 value)
{         
    SpiDeviceInit(ACCEL_SPI_PERIPH);
    
    /* assert cs */
    spiStart(ACCEL_SPI_PERIPH);
        
    SPI_TRANSFER_ACCEL(reg &0x3F);
    SPI_TRANSFER_ACCEL(value);

    spiStop(ACCEL_SPI_PERIPH);

    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: AccelReadByte
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT8 AccelReadByte(UINT8 cmd)
{      
    UINT8 rxData =0;

    SpiDeviceInit(ACCEL_SPI_PERIPH);
    
    /* assert cs */
    spiStart(ACCEL_SPI_PERIPH);
    
    SPI_TRANSFER_ACCEL( (cmd |LIS3DH_REG_MASK_READ) );   
    rxData =SPI_TRANSFER_ACCEL(SPI_DUMMY_BYTE );

    spiStop(ACCEL_SPI_PERIPH);

    return rxData;
}





