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



/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdrInit
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void PressureTdrInit( void )
{
    SpiInit(PRESS_TDR_SPI_PORT);
}


/*
*|----------------------------------------------------------------------------
*|  Routine: PressureTdrRead
*|  Description:
*|  Arguments:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL PressureTdrRead(UINT8 addr, UINT8 nbrBytes, UINT8 *pDataBuf)
{
    SpiPeripheralEnum device;
    UINT8 byteCnt=0;    
    
    device =PRESS_TDR_SPI_PERIPH;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_PTDR( 0x00 );    
    SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
    SPI_TRANSFER_PTDR( SPI_DUMMY_BYTE );
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







