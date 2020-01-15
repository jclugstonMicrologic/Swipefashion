/** C Source *******************************************************************
*
* NAME      AdcFd.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
********************************************************************************
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

/** Include Files *************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "adcFd.h"
#include "adcHi.h"

#include "GatewayConfig.h"
#include "math.h"

#include "dataFlashFd.h"
#include "crc.h"

#include "gpioHi.h"

enum
{
    SL_LEVEL_CH =0,
    FLUID_LEVEL_CH,          
    VSENSE1_CH,              
    VSENSE2_CH,              
    TEMPERATURE_CH,          
    BATTERY_VOLTS_CH,        
      
    LAST_CH   
};

//#define INTERNAL_TEMPSENSOR_V25       ((int32_t)741)   /* internal temperature sensor, parameter V25 (unit: mV). Refer to device datasheet for min/typ/max values. */
//#define INTERNAL_TEMPSENSOR_AVGSLOPE  ((int32_t)4300)  /* internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */

#define INTERNAL_TEMPSENSOR_V25       ((int32_t)760)   /* internal temperature sensor, parameter V25 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_AVGSLOPE  ((int32_t)2500)  /* internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */

#define ADC_VREF  3300 /* mV */
#define ADC_12BIT 4095 
#define SHUNT_RESISTOR 150    /* ohms */
    
#define BATTERY_VOLTS_SCALE 5 /* volts */

/*
*****************************************************************************
 L O C A L    T Y P E    D E F I N I T I O N S
*****************************************************************************
*/
typedef struct
{
    float mIsense1;       // calibrated slope value for Isense
    float mIsense2;       // calibrated slope value for Isense

    UINT16 spare;
    UINT16 crc;
}ADC_CALIBRATION;

typedef struct
{
   float avg[15];
   float avgValue;
   UINT8 nbrSamples;
   UINT8 smplCnt;
   UINT32 smplRate;
   UINT8 nbrValues; // number of values to average
}RUN_AVG;

/*
*****************************************************************************
 P R I V A T E   G L O B A L   D A T A
*****************************************************************************
*/
ADC_CALIBRATION AdcCalibration;

ADC_READINGS_STRUCT AdcReadings;

UINT16 RawData[NBR_ADC_CHANNELS];
    
float AdcV1, AdcV2;    
float I1, I2;

void AdcCalcSlope();

RUN_AVG AvgSloadLevel;

/*
*****************************************************************************
 P R I V A T E   F U N C T I O N   P R O T O T Y P E S
*****************************************************************************
*/
void AdcConvertToEngUnits(UINT16 rawValue, UINT8 channel, float *pEngData);
float AdcRawToVoltage( UINT16 rawValue );
float AdcRawToCurrent( UINT16 rawValue );

BOOL AdcGetAdcCalibration(ADC_CALIBRATION *pAdcCalibration);
BOOL AdcSetAdcCalibration(ADC_CALIBRATION *pAdcCalibration);

void CalcRunningAverage(RUN_AVG *pAvg,float arg);

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine:  AdcUseCalibrationDefaults
*|  Description:
*|----------------------------------------------------------------------------
*/
void AdcUseCalibrationDefaults(ADC_CALIBRATION *pAdcCalibration)
{
    /* init structure */
    memset(pAdcCalibration, 0x00, sizeof(ADC_CALIBRATION));
      
    /* populate struct with default values */        
    pAdcCalibration->mIsense1 =0.007368;
    pAdcCalibration->mIsense2 =0.007368;
    
    pAdcCalibration->crc =CrcCalc16( (unsigned char *)pAdcCalibration, sizeof(ADC_CALIBRATION)-sizeof(UINT16) );    
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine:  AdcInit
*|  Description:
*|----------------------------------------------------------------------------
*/
void AdcInit(void)
{
    memset( &AdcCalibration, 0x00, sizeof(AdcCalibration) );

#define MANUAL_CALIBRATION
#ifdef MANUAL_CALIBRATION    
AdcV1 =620;
I1 =4.002;
AdcV2 =1897;
I2 =12.21;
AdcCalcSlope();
#endif

    if( !AdcGetAdcCalibration( &AdcCalibration ) )
    {
        AdcUseCalibrationDefaults(&AdcCalibration);
        
        AdcSetAdcCalibration( &AdcCalibration );      
    }
    
//    AdcCalcSlope();
    
    memset( &AvgSloadLevel, 0x00, sizeof(AvgSloadLevel) );      
    AvgSloadLevel.smplRate =100; 
    AvgSloadLevel.nbrValues =11;
   
    adc_init();
}

#define NBR_AVERAGE 15
/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine:  AdcMeasureReadings
*|  Description:
*|----------------------------------------------------------------------------
*/
void AdcMeasureReadings(void)
{ 
    UINT8 channel;
    UINT8 avg;

    memset( &RawData, 0x00, sizeof(RawData) );
    
    for(avg =0; avg<NBR_AVERAGE; avg++)
    {    
        for(channel =0; channel <NBR_ADC_CHANNELS; channel++)
        {
            RawData[channel] +=adc_getRawData();                
        }        
    }
    
    for(channel =0; channel <NBR_ADC_CHANNELS; channel++)
    {
        RawData[channel] /=NBR_AVERAGE;
    }
    
    AdcConvertToEngUnits(RawData[SL_LEVEL_CH], SL_LEVEL_CH, &AdcReadings.slLevel);
    AdcConvertToEngUnits(RawData[FLUID_LEVEL_CH], FLUID_LEVEL_CH, &AdcReadings.fluidLevel);
    AdcConvertToEngUnits(RawData[VSENSE1_CH], VSENSE1_CH, &AdcReadings.vsense1);    
    AdcConvertToEngUnits(RawData[VSENSE2_CH], VSENSE2_CH, &AdcReadings.vsense2);    
    AdcConvertToEngUnits(RawData[TEMPERATURE_CH], TEMPERATURE_CH, &AdcReadings.temp);        
    
    CalcRunningAverage( &AvgSloadLevel, AdcReadings.slLevel );
} 

/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd
*|  Routine:  AdcConvertToEngUnits
*|  Description:
*|----------------------------------------------------------------------------
*/ 
void AdcConvertToEngUnits(UINT16 rawValue, UINT8 channel, float *pEngData)
{    
    float voltage =0.0;
    float current =0.0;
    
    switch( channel )
    {
        case SL_LEVEL_CH:
            current =AdcRawToCurrent(rawValue);
            
            *pEngData =current; //dc4To20Conversion(current);
            break;      
        case FLUID_LEVEL_CH:
            *pEngData =AdcRawToVoltage(rawValue);
            break;                  
        case VSENSE1_CH:
            *pEngData =AdcRawToVoltage(rawValue);
            break;                              
        case VSENSE2_CH:
            *pEngData =AdcRawToVoltage(rawValue);
            break;                                          
        case TEMPERATURE_CH:
            //*pEngData =( ((float)rawValue *3300)/4095 -741)/4.3 +25;           
            voltage =AdcRawToVoltage(rawValue);            
            
            *pEngData =( (( voltage -INTERNAL_TEMPSENSOR_V25)*1000)/INTERNAL_TEMPSENSOR_AVGSLOPE +25);
            break;
        case BATTERY_VOLTS_CH:
            *pEngData =AdcRawToVoltage(rawValue);
            break;
    }
}

/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd 
*|  Routine: AdcGetData
*|  Description:
*|----------------------------------------------------------------------------
*/
void AdcGetData(ADC_READINGS_STRUCT *pAdcData)
{
    memcpy(pAdcData, &AdcReadings, sizeof(ADC_READINGS_STRUCT));
}

/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd 
*|  Routine: AdcGetRawData
*|  Description:
*|----------------------------------------------------------------------------
*/
void AdcGetRawData(UINT16 *pRawData)
{
    memcpy(pRawData, &RawData, sizeof(RawData));
}

/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd 
*|  Routine: AdcRawToVoltage
*|  Description:
*|----------------------------------------------------------------------------
*/
float AdcRawToVoltage( UINT16 rawValue )
{
    return ( ((float)rawValue *ADC_VREF)/ADC_12BIT); //mV
}

/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd 
*|  Routine: AdcRawToCurrent
*|  Description:
*|----------------------------------------------------------------------------
*/
float AdcRawToCurrent(UINT16 rawValue )
{
    //return AdcCalibration.mIsense1*((float)rawValue);
      
    return ( ((float)rawValue *ADC_VREF)/ADC_12BIT)/SHUNT_RESISTOR; //mA  
}


/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd 
*|  Routine: Adc4To20Conversion
*|  Description:
*|----------------------------------------------------------------------------
*/
float Adc4To20Conversion(float current, UINT16 range1, UINT16 range2)
{
    static float value=0;
    static float m =0;
    
    //value =(float)((float)scale/16.0)*(current-4);
    
    /* 4mA-20mA is 40cm - 400cm */
    //value =(float)((float)360/16.0)*current - 50;
    
    m =(float)(range2-range1)/16; //run =(20-4)mA =16
    
    value =m*(current - 4) + range1;
    
    if( current <3.6 )
        value =0.0;
          
    return value;
}


/*
*|----------------------------------------------------------------------------
*|  Module: AdcFd 
*|  Routine: AdcCalcSlope
*|  Description:
*|----------------------------------------------------------------------------
*/
void AdcCalcSlope()
{        
    AdcCalibration.mIsense1 =(I2-I1)/(AdcV2-AdcV1);   
        
    AdcSetAdcCalibration( &AdcCalibration );
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GwSetConfigFlash
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AdcSetAdcCalibration(ADC_CALIBRATION *pAdcCalibration)
{   
    memcpy( &AdcCalibration, pAdcCalibration, sizeof(ADC_CALIBRATION));  
    
    AdcCalibration.crc =CrcCalc16( (unsigned char *)&AdcCalibration, sizeof(AdcCalibration)-sizeof(UINT16) );  
    
    /* erase this sector */
    DataFlashEraseSector(MEMORY_ADC_CAL_ADDRESS/DATAFLASH_SIZE_SECTOR_BYTES);
    
    if( !DataFlashWrite( MEMORY_ADC_CAL_ADDRESS, sizeof(ADC_CALIBRATION), (UINT8 *)&AdcCalibration) ) 
    {
        return FALSE;
    }
    
    else    
        return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: AdcGetAdcCalibration
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AdcGetAdcCalibration(ADC_CALIBRATION *pAdcCalibration)
{   
    UINT16 rxCrc =0;
    
    if( !DataFlashRead(MEMORY_ADC_CAL_ADDRESS, sizeof(ADC_CALIBRATION), (UINT8 *)pAdcCalibration) )
    {
        return FALSE;
    }
    
    memcpy( &AdcCalibration, pAdcCalibration, sizeof(ADC_CALIBRATION));  
    
    rxCrc =CrcCalc16( (unsigned char *)pAdcCalibration, sizeof(ADC_CALIBRATION)-sizeof(UINT16) );  
        
    if( rxCrc !=pAdcCalibration->crc )
        return FALSE;
    else    
        return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: CalcRunningAverage
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void CalcRunningAverage
(
   RUN_AVG *pAvg,       
   float arg            /* value to put into the average */
)
{
    UINT8 j;
    static UINT32 sampleTime =0;
    TickType_t xTicks=xTaskGetTickCount();
   
    if( (xTicks -sampleTime) > pAvg->smplRate )
    {	
        sampleTime =xTicks;

        pAvg->avg[pAvg->smplCnt] =arg;
	 
        pAvg->avgValue =0;

        if( pAvg->nbrSamples <pAvg->nbrValues )
            pAvg->nbrSamples ++;

        for(j=0; j<pAvg->nbrSamples; j++)
        {
            pAvg->avgValue +=pAvg->avg[j];
        }

        pAvg->avgValue /=pAvg->nbrSamples;      

        if( ++pAvg->smplCnt  >=pAvg->nbrValues )
            pAvg->smplCnt =0;  
    }
     
}/* end CalcRunningAverage() */


/*
*|----------------------------------------------------------------------------
*|  Routine: GetSlLevelAverage
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
float GetSloadLevelAverage(void)
{
    return AvgSloadLevel.avgValue;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: CalcFluidVolume
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
float CalcFluidVolume(float range)
{
    GW_SETUP_STRUCT gwSetup;
    
    float fluidVolume;
    
    GwGetSetup(&gwSetup); 
    
    fluidVolume =0.0;
      
    for(int coeff=0; coeff<7; coeff++)
    {
        /* use metric for outage, and returned volume (liters) */
        fluidVolume +=(float)(gwSetup.volCoeff[coeff] * pow(range, coeff));
    }
    
    return fluidVolume;
}


