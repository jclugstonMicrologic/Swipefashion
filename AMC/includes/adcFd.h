/** H Header  ******************************************************************
*
* NAME      AdcFd.h
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/



#ifndef ADC_FD_H
#define ADC_FD_H

#include "types.h"
#include "stm32f4xx.h"

typedef struct
{
    float slLevel; /* TLoad load level (Vega sensor) */
    float fluidLevel;
    float vsense1;
    float vsense2;
    float temp;
    float batteryVolts;
      
}adc_readings_t;

void Adc_Init(void);

void AdcMeasureReadings(void);
void AdcGetData(adc_readings_t *pAdcData);
void AdcGetRawData(UINT16 *pRawData);
float Adc4To20Conversion(float current, UINT16 range1, UINT16 range2);

float GetSloadLevelAverage(void);
float CalcFluidVolume(float range);
#endif


