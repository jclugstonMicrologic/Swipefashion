/** H Header  ******************************************************************
*
* NAME      adcHi.h
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



#ifndef ADC_HI_H
#define ADC_HI_H

#include "types.h"
#include "stm32f4xx.h"

#define NBR_ADC_CHANNELS 5

void adc_init(void);
UINT16 adc_getRawData(void);

#endif


