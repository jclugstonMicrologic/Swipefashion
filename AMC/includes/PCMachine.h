/** H Header  ******************************************************************
*
* NAME      PCMachine.h
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

#ifndef PC_MACHINE_H
#define PC_MACHINE_H

#include "types.h"
#include "SciBinaryMachine.h"

/* PC bootloader commands */
//#define CMD_GW_TEST_PACKET                      0x0000
#define CMD_GW_RESET                            0x0100
#define CMD_GW_FW_ERASE_PACKET                  0x0101
#define CMD_GW_FW_UPDATE_PACKET                 0x0102
#define CMD_GW_FW_UPDATE_COMPLETE_PACKET        0x0103

#define CMD_GET_UPDATE_STATUS                   0x0106
#define CMD_GET_XLOAD_READINGS                  0x0107
#define CMD_GET_DEV_INFO                        0x0108
#define CMD_GET_FW_VER                          0x0109


/* PC application commands */
#define CMD_GW_GET_FW_VERSION                   0x0200
#define CMD_GW_GET_RTC                          0x0201
#define CMD_GW_SET_RTC                          0x0202

#define CMD_GW_GET_CONFIG                       0x0203
#define CMD_GW_SET_CONFIG                       0x0204

#define CMD_GW_GET_ANALOG                       0x0205
#define CMD_GW_GET_LOG_INFO                     0x0206
    
#define CMD_GW_GET_SETUP                        0x0207
#define CMD_GW_SET_SETUP                        0x0208

#define CMD_GW_DUMP_MEMORY                      0x0209
#define CMD_ERASE_LOG_MEMORY                    0x020a
#define CMD_ERASE_ALL_LOG_MEMORY                0x020b
#define CMD_GET_ERASE_STATUS                    0x020c
          
#define CMD_OPEN_VALVE                          0x020d
#define CMD_CLOSE_VALVE                         0x020e

#define CMD_GET_PRESS_TEMP                      0x020f
#define CMD_GET_BRD_ID                          0x0210
#define CMD_GET_PRESS                           0x0211

#define CMD_SET_VALVE                           0x0212

#define CMD_START_COMR                          0x0213
#define CMD_STOP_COMR                           0x0214
#define CMD_SET_COMR                            0x0215

#define CMD_SET_LED_LEVEL                       0x0216

#define CMD_ERASE_FLASH                         0x0217


#define CMD_GW_LAST                             0x02ff


#define CMD_LAST                                0x7fff

#define ACK                                     0x8000

BOOL PCMachineInit(void);

#endif




