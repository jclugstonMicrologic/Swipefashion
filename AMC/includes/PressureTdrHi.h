/** H Header  ******************************************************************
*
* NAME      PressureTdr.h
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

#ifndef PRESSURE_TDR_H
#define PRESSURE_TDR_H

#include "types.h"


/**\name BMP3 chip identifier */
#define BMP3_CHIP_ID   (0x50)
/**\name BMP3 pressure settling time (micro secs)*/
#define BMP3_PRESS_SETTLE_TIME   (392)
/**\name BMP3 temperature settling time (micro secs) */
#define BMP3_TEMP_SETTLE_TIME     (313)
/**\name BMP3 adc conversion time (micro secs) */
#define BMP3_ADC_CONV_TIME     (2000)

/**\name Register Address */
#define BMP3_CHIP_ID_ADDR     (0x00)
#define BMP3_ERR_REG_ADDR     (0x02)
#define BMP3_SENS_STATUS_REG_ADDR   (0x03)
#define BMP3_PRESS_DATA_ADDR     (0x04)
#define BMP3_TEMP_DATA_ADDR      (0x07)

#define BMP3_EVENT_ADDR     (0x10)
#define BMP3_INT_STATUS_REG_ADDR   (0x11)
#define BMP3_FIFO_LENGTH_ADDR     (0x12)
#define BMP3_FIFO_DATA_ADDR     (0x14)
#define BMP3_FIFO_WM_ADDR     (0x15)
#define BMP3_FIFO_CONFIG_1_ADDR   (0x17)
#define BMP3_FIFO_CONFIG_2_ADDR   (0x18)
#define BMP3_INT_CTRL_ADDR     (0x19)
#define BMP3_IF_CONF_ADDR     (0x1A)
#define BMP3_PWR_CTRL_ADDR     (0x1B)
#define BMP3_OSR_ADDR       (0X1C)
#define BMP3_CALIB_DATA_ADDR     (0x31)
#define BMP3_CMD_ADDR       (0x7E)

/**\name Power mode macros */
#define BMP3_SLEEP_MODE     (0x00)
#define BMP3_FORCED_MODE    (0x10)
#define BMP3_NORMAL_MODE    (0x30)

#define BMP3_PRESS_EN       (0x01)
#define BMP3_TEMP_EN        (0x02)

#define PRESS_TDR_SPI_PORT    SPI_PORT2

#define SPI_TRANSFER_PTDR(txByte)    SpiTransferSpi2(txByte)


#define NBR_TRANSDUCERS 8

typedef struct
{
  float press;
  float temp;
}press_sensor_data_t;

//extern float PressureValue[8];
//extern float TemperatureValue[8];

void PressureTdr_Init(void);
BOOL PressureTdr_Read(UINT8 addr, UINT8 tdrNbr, UINT8 nbrBytes, UINT8 *pDataBuf);
BOOL PressureTdr_Write(UINT8 addr, UINT8 tdrNbr, UINT8 nbrBytes, UINT8 *pDataBuf);

BOOL PressureTdr_ReadPT(UINT8 tdrNbr, float *pPress, float *pTemp);

UINT8 PressureTdr_GetTdrs(void);

BOOL PressureTdr_CheckOverPres(void);
void PressureTdr_GetPressTemp(press_sensor_data_t *pPr);
#endif




