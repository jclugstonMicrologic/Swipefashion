/** H Header  ******************************************************************
*
* NAME      DataFlashFd.h
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
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

#ifndef DATA_FLASH_HI_H
#define DATA_FLASH_HI_H

#include "types.h"

/* DATAFLASH Sizes */
#define DATAFLASH_SIZE_PAGE_BYTES       (256)
#define DATAFLASH_SIZE_SECTOR_BYTES     (4*1024)
#define DATAFLASH_SIZE_BLOCK_BYTES      (64*1024)
#define DATAFLASH_NUM_PAGES_PER_SECTOR  (16)
#define DATAFLASH_NUM_SECTORS_PER_BLOCK (16)
#define DATAFLASH_NUM_BLOCKS_PER_CHIP   (256) //(8)
#define DATAFLASH_NUM_SECTORS_PER_CHIP  (DATAFLASH_NUM_SECTORS_PER_BLOCK * DATAFLASH_NUM_BLOCKS_PER_CHIP)
#define DATAFLASH_NUM_PAGES_PER_CHIP    (DATAFLASH_NUM_SECTORS_PER_CHIP * DATAFLASH_NUM_PAGES_PER_SECTOR)
#define DATAFLASH_NUM_BYTES_PER_CHIP    (DATAFLASH_NUM_PAGES_PER_CHIP * DATAFLASH_SIZE_PAGE_BYTES)

#define DATAFLASH_CMD_NORD              (0x03) // normal read operation
#define DATAFLASH_CMD_PP                (0x02) // page program
#define DATAFLASH_CMD_SER               (0x20) // sector erase
#define DATAFLASH_CMD_BER64K            (0xD8) // block erase
#define DATAFLASH_CMD_CER               (0x60) // chip erase
#define DATAFLASH_CMD_WREN              (0x06) // write enable 
#define DATAFLASH_CMD_WRDI              (0x04) // write disable
#define DATAFLASH_CMD_RDSR              (0x05) // read status register 
#define DATAFLASH_CMD_WRSR              (0x01) // write status register 
#define DATAFLASH_CMD_RDFR              (0x48) // read function register 
#define DATAFLASH_CMD_WRFR              (0x42) // write function register 
#define DATAFLASH_CMD_RDID              (0xAB) // read device ID
#define DATAFLASH_CMD_SECTOR_ER1        (0xD7) // Sector Erase 4 Bytes
#define DATAFLASH_CMD_SECTOR_ER2        (0x20) // Sector Erase 4 Bytes

#define FLASH_SPI_PORT       SPI_PORT2
#define FLASH_SPI_M1_PERIPH  SPI_PERIPHERAL2
#define FLASH_SPI_M2_PERIPH  SPI_PERIPHERAL3

/************** MACROS *****************/
#define BLOCK_TO_ADDRESS() block *DATAFLASH_SIZE_BLOCK_BYTES;

//#define SPI_TRANSFER_DFLASH(txByte, rxBuf)    SpiTransferSpi1(txByte, rxBuf)
#define SPI_TRANSFER_DFLASH(txByte)    SpiTransferSpi2(txByte)


BOOL DataFlashInit(void);
BOOL DataFlashReadDeviceID(UINT8 chipNbr);
BOOL DataFlashReadStatus(UINT8 chipNbr);

BOOL DataFlashRead(UINT32 addr, UINT16 nbrBytes, UINT8 *pDataBuf);
BOOL DataFlashWrite(UINT32 addr, UINT16 nbrBytes, UINT8 *pDataBuf);

void DataFlashEraseSector(UINT16 sector);
BOOL DataFlashEraseBlock(UINT16 block);

#endif




