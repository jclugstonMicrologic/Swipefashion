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

#include "spiHi.h"
#include "PressureTdrHi.h"



UINT32 PressureValue[8];

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
    
    PressureTdr_Write(BMP3_PWR_CTRL_ADDR, 0, 1, &regData);
    PressureTdr_Write(BMP3_PWR_CTRL_ADDR, 8, 1, &regData);
    
    PressureTdr_Read(BMP3_CHIP_ID_ADDR, 0, 1, &chipId);
    PressureTdr_Read(BMP3_CHIP_ID_ADDR, 8, 1, &chipId);
}

// https://github.com/DFRobot/DFRobot_BMP388/blob/master/DFRobot_BMP388.cpp
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
    
    SPI_TRANSFER_PTDR( addr | 0x80 );    
    //SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
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





