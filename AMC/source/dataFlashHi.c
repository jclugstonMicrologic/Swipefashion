/** C Source *******************************************************************
*
* NAME      dataFlashHi.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded worbench for ARM v7.4
*
* REVISION LOG
*
********************************************************************************
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "dataFlashHi.h"

#include "spiHi.h"
#include "sysTimers.h"
#include "semphr.h"

#define FLASH_ACCESS_SEMAPHORE_ENABLE

BOOL DataFlashWriteEnable(UINT8 chipNbr);

SemaphoreHandle_t xFlashAccessMutex;

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashInit(void)
{    
    SpiInit(FLASH_SPI_PORT);

    xFlashAccessMutex =xSemaphoreCreateMutex();
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashReadDeviceID
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashReadDeviceID(UINT8 chipNbr)
{   
    UINT8 deviceId1, deviceId2;         
    SpiPeripheralEnum device;
    
    if( chipNbr ==1 )
        device =FLASH_SPI_M1_PERIPH;
    else
        device =FLASH_SPI_M2_PERIPH;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_RDID );    
    SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
    SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
    SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
    
    /* read data */
    deviceId1 =SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
    deviceId2 =SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
     
    /* negate cs */
    spiStop(device);
    
    return TRUE;
}

/******************************************************************************
* NAME      DataFlashGetStatus()
*
* SUMMARY   Get the status byte from the Flash.
*
* INPUTS    None.
*
* OUTPUTS   UINT8, Status Byte
*******************************************************************************/
UINT8 DataFlashGetStatus(UINT8 chipNbr)
{
    UINT8 status = 0x00;

    SpiPeripheralEnum device;
    
    if( chipNbr ==1 )
        device =FLASH_SPI_M1_PERIPH;
    else
        device =FLASH_SPI_M2_PERIPH;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_RDSR );    
    status =SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );

    /* negate cs */
    spiStop(device);
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashRead
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashRead(UINT32 addr, UINT16 nbrBytes, UINT8 *pDataBuf)
{         
    UINT16 byteCnt=0;    
    UINT8 chipNbr;
    SpiPeripheralEnum device;
    
#ifdef  FLASH_ACCESS_SEMAPHORE_ENABLE        
    if( xFlashAccessMutex != NULL )
    {
        if( !xSemaphoreTake( xFlashAccessMutex, ( TickType_t ) 10 ) ) 
        {
            return FALSE;
        }
    }
#endif    
    
    if( addr <0x01000000 )
    {
        chipNbr =1;
        device =FLASH_SPI_M1_PERIPH;
    }
    else
    {
        chipNbr =2;
        addr -=0x01000000;
        device =FLASH_SPI_M2_PERIPH;
    }
    
    while( DataFlashGetStatus(chipNbr) &0x01 ==0x01)
    {
        /* write in progress, device is busy */
        TimerDelayUs(1000);
    }    
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_NORD );    
    SPI_TRANSFER_DFLASH( ( (addr >>16)&0xff) );    
    SPI_TRANSFER_DFLASH( ( (addr >>8)&0xff) );    
    SPI_TRANSFER_DFLASH( ( (addr >>0)&0xff) );    
   
    /* read data */
    for(byteCnt =0; byteCnt<nbrBytes; byteCnt++)
    {
        *pDataBuf =SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
        
        *pDataBuf++;
    }
     
    /* negate cs */
    spiStop(device);
    
#ifdef FLASH_ACCESS_SEMAPHORE_ENABLE        
    if( xFlashAccessMutex != NULL )
    {
        xSemaphoreGive( xFlashAccessMutex );     
    }
#endif    
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashWrite
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashWrite(UINT32 addr, UINT16 nbrBytes, UINT8 *pDataBuf)
{      
    UINT16 byteCnt=0;        
    UINT8 chipNbr;
    SpiPeripheralEnum device;

#ifdef FLASH_ACCESS_SEMAPHORE_ENABLE    
    if( xFlashAccessMutex != NULL )
    {
        if( !xSemaphoreTake( xFlashAccessMutex, ( TickType_t ) 10 )  ) 
        {
            return FALSE;
        }
    }
#endif
    
    if( addr <0x01000000 )
    {
        chipNbr =1;             
        device =FLASH_SPI_M1_PERIPH;
        DataFlashWriteEnable(0);
    }
    else
    {
        chipNbr =2;
        addr -=0x01000000;
        device =FLASH_SPI_M2_PERIPH;
        DataFlashWriteEnable(1);
    }

    while( DataFlashGetStatus(chipNbr) &0x01 ==0x01)
    {
        /* write in progress, device is busy */
        TimerDelayUs(1000);
    }        
   
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_PP );    
    SPI_TRANSFER_DFLASH( ( (addr >>16)&0xff) );    
    SPI_TRANSFER_DFLASH( ( (addr >>8)&0xff) );    
    SPI_TRANSFER_DFLASH( ( (addr >>0)&0xff) );    
   
    /* write data */
    for(byteCnt =0; byteCnt<nbrBytes; byteCnt++)
    {      
        SPI_TRANSFER_DFLASH( *pDataBuf );
        
        *pDataBuf++;
    }
    
    /* negate cs */
    spiStop(device);
    
#ifdef  FLASH_ACCESS_SEMAPHORE_ENABLE        
    if( xFlashAccessMutex != NULL )
    {
        xSemaphoreGive( xFlashAccessMutex );     
    }
#endif    
       
    return TRUE;
}



/******************************************************************************
* NAME      DataFlashEraseSector()
*
* SUMMARY   Erase the given sector (valid range 0..127)
*
* INPUTS    UINT8 bSector, sector index to erase.
*
* OUTPUTS   None.
*******************************************************************************/
void DataFlashEraseSector(UINT16 sector)
{
    SpiPeripheralEnum device;
    //UINT8 status =0;
    UINT8 chipNbr;
    UINT32 addr =0;
        
#ifdef FLASH_ACCESS_SEMAPHORE_ENABLE    
    if( xFlashAccessMutex != NULL )
    {
        if( !xSemaphoreTake( xFlashAccessMutex, ( TickType_t ) 10 ) ) 
        {
            return;
        }
    }
#endif
    
    if (sector < DATAFLASH_NUM_SECTORS_PER_CHIP)
    {
        chipNbr =1;
        device =FLASH_SPI_M1_PERIPH;
        DataFlashWriteEnable(0);
    }
    else
    {
        chipNbr =2;
        sector -=4096;
        device =FLASH_SPI_M2_PERIPH;
        DataFlashWriteEnable(1);
    }
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);

    addr = sector * DATAFLASH_SIZE_SECTOR_BYTES;
      
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_SECTOR_ER1 );    
    SPI_TRANSFER_DFLASH( ( (addr >>16)&0xff) );    
    SPI_TRANSFER_DFLASH( ( (addr >>8)&0xff) );
    SPI_TRANSFER_DFLASH( ( (addr >>0)&0xff) );    
     
     /* negate cs */
    spiStop(device);        
    
    while( DataFlashGetStatus(chipNbr) &0x01 ==0x01)
    {
        /* write in progress, device is busy */
        TimerDelayUs(1000);
    }    

#ifdef FLASH_ACCESS_SEMAPHORE_ENABLE    
    if( xFlashAccessMutex != NULL )
    {
        xSemaphoreGive( xFlashAccessMutex );     
    }
#endif      
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashEraseBlock
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashEraseBlock(UINT16 block)
{      
    UINT32 addr;
    SpiPeripheralEnum device;
    UINT8 chipNbr;
      
#ifdef FLASH_ACCESS_SEMAPHORE_ENABLE    
    if( xFlashAccessMutex != NULL )
    {
        if( !xSemaphoreTake( xFlashAccessMutex, ( TickType_t ) 10 ) ) 
        {
            return FALSE;
        }
    }
#endif
    
    if( block <DATAFLASH_NUM_BLOCKS_PER_CHIP )
    {
        chipNbr =1; 
        device =FLASH_SPI_M1_PERIPH;
        DataFlashWriteEnable(0);
    }
    else
    {
        chipNbr =2; 
        device =FLASH_SPI_M2_PERIPH;
        DataFlashWriteEnable(1);
    }
    
    addr =BLOCK_TO_ADDRESS();
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_BER64K );    
    SPI_TRANSFER_DFLASH( ( (addr >>16)&0xff) );    
    SPI_TRANSFER_DFLASH( ( (addr >>8)&0xff) );
    SPI_TRANSFER_DFLASH( ( (addr >>0)&0xff) );    
     
    /* negate cs */
    spiStop(device);
    
    while( DataFlashGetStatus(chipNbr) &0x01 ==0x01)
    {
        TimerDelayUs(1000);
    } 

#ifdef  FLASH_ACCESS_SEMAPHORE_ENABLE    
    if( xFlashAccessMutex != NULL )
    {
        xSemaphoreGive( xFlashAccessMutex );     
    }
#endif    
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashWriteEnable
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashWriteEnable(UINT8 chipNbr)
{      
    SpiPeripheralEnum device;
    
    if( chipNbr ==0 )
        device =FLASH_SPI_M1_PERIPH;
    else
        device =FLASH_SPI_M2_PERIPH;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_WREN );    
    
    /* negate cs */
    spiStop(device);
  
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: DataFlashReadStatus
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL DataFlashReadStatus(UINT8 chipNbr)
{      
    //UINT8 status  = 0x00;
    SpiPeripheralEnum device;
    
    if( chipNbr ==1 )
        device =FLASH_SPI_M1_PERIPH;
    else
        device =FLASH_SPI_M2_PERIPH;
    
    SpiDeviceInit(device);
    
    /* assert cs */
    spiStart(device);
    
    SPI_TRANSFER_DFLASH( DATAFLASH_CMD_RDSR );    
    SPI_TRANSFER_DFLASH( SPI_DUMMY_BYTE );
    
    /* negate cs */
    spiStop(device);
  
    return TRUE;
}
