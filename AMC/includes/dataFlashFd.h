/** H Header  ******************************************************************
*
* NAME      dataFlashFd.h
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
* Copyright (c) 2018 TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

#ifndef DATA_FLASH_FD_H
#define DATA_FLASH_FD_H

#include "dataFlashHi.h"

#define MEMORY_CONFIG_ADDRESS   0x00000000
#define MEMORY_CONFIG_SIZE      DATAFLASH_SIZE_SECTOR_BYTES//0x00000100

#define MEMORY_SETUP_ADDRESS    (MEMORY_CONFIG_ADDRESS +MEMORY_CONFIG_SIZE)
#define MEMORY_SETUP_SIZE       DATAFLASH_SIZE_SECTOR_BYTES

#define MEMORY_ADC_CAL_ADDRESS  (MEMORY_SETUP_ADDRESS +MEMORY_SETUP_SIZE)
#define MEMORY_ADC_CAL__SIZE    DATAFLASH_SIZE_SECTOR_BYTES

#define MEMORY_LOG_START        0x00200000
#define MEMORY_LOG_END          0x01000000
                                

/* this set of data is logged on every meter ticket message received */
#define METER_TCKT_LOG_ADDR_START    MEMORY_LOG_START
#define METER_TCKT_LOG_ADDR_END      (MEMORY_LOG_START+ 0x00100000)
#define METER_TCKT_LOG_PACKET_SIZE   256

#define DIAGNOSTIC_LOG_ADDR_START    METER_TCKT_LOG_ADDR_END
#define DIAGNOSTIC_LOG_ADDR_END      (DIAGNOSTIC_LOG_ADDR_START+32*DATAFLASH_SIZE_BLOCK_BYTES) 
#define DIAGNOSTIC_LOG_PACKET_SIZE   16 //64

#define FIELD_SURVEY_DIAG_START      DIAGNOSTIC_LOG_ADDR_END
#define FIELD_SURVEY_DIAG_END        (FIELD_SURVEY_DIAG_START +64*DATAFLASH_SIZE_BLOCK_BYTES)
#define FIELD_SURVEY_DIAG_LOG_PACKET_SIZE   16

#define PLC_LOG_START                FIELD_SURVEY_DIAG_END
#define PLC_LOG_END                  (PLC_LOG_START +20*DATAFLASH_SIZE_BLOCK_BYTES) //(PLC_LOG_START +64*DATAFLASH_SIZE_BLOCK_BYTES)
#define PLC_LOG_PACKET_SIZE          32

//#define FW_FILE_ADDR_START    DIAGNOSTIC_LOG_ADDR_END
//#define FW_FILE_ADDR_END      (FW_FILE_ADDR_START+ 0x40000)
//#define FW_FILE_PACKET_SIZE   256

#define LOG_ADDR_START    MEMORY_LOG_START
#define LOG_ADDR_END      MEMORY_LOG_END /* absolute memory end for current hardware */
#define LOG_LAST_LOG_END  DIAGNOSTIC_LOG_ADDR_END

/* table containing all current log addressing */
#define LOG_ADDR_TABLE          (0x01000000-DATAFLASH_SIZE_SECTOR_BYTES) //0x00ffff00
#define LOG_ADDR_TABLE_SIZE     DATAFLASH_SIZE_SECTOR_BYTES //256

#endif