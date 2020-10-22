/** C Source ******************************************************************
*
* NAME      GatewayConfig.c
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

#include "AmcConfig.h"

#include "dataFlashFd.h"
//#include "dataFlashHi.h"
#include "crc.h"


AMC_CONFIG_STRUCT AmcConfig;
AMC_SETUP_STRUCT  AmcSetup;

/** Functions *****************************************************************/

/*
*|----------------------------------------------------------------------------
*|  Routine: AmcUseConfigDefaults
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void AmcUseConfigDefaults(AMC_CONFIG_STRUCT *pGwConfig)
{
    /* init structure */
    memset(pGwConfig, 0x00, sizeof(AMC_CONFIG_STRUCT));
      
    /* populate struct with default values */    
    pGwConfig->network.gatewayId =0;
    pGwConfig->network.serverPort =80; 
    pGwConfig->network.ipAddr[0] =159;
    pGwConfig->network.ipAddr[1] =89;
    pGwConfig->network.ipAddr[2] =117;
    pGwConfig->network.ipAddr[3] =98;    
    strcpy( (char *)pGwConfig->network.apn, "m2minternet.apn");
       
    /* default every 60seconds */
    pGwConfig->shedule.report =60;
    pGwConfig->shedule.sleepEnabled =FALSE;
    
    pGwConfig->sensor.facilityId =0;
    pGwConfig->sensor.spotId =0;
    pGwConfig->sensor.deviceType =0;
    
    pGwConfig->crc =CrcCalc16( (unsigned char *)pGwConfig, sizeof(AMC_CONFIG_STRUCT)-sizeof(UINT16) );  
}

/*
*|----------------------------------------------------------------------------
*|  Routine: AmcUseSetupDefaults
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void AmcUseSetupDefaults(AMC_SETUP_STRUCT *pGwSetup)
{   
    /* init structure */
    memset(pGwSetup, 0x00, sizeof(AMC_SETUP_STRUCT));
      
    /* populate struct with default values */        
    pGwSetup->railCarNbr =123456;
    pGwSetup->highLevel =19;
    pGwSetup->vOffset =15;
    pGwSetup->remoteUpdateRate =10000;
    pGwSetup->sensorRange1 =40;
    pGwSetup->sensorRange2 =400;
    
    
    pGwSetup->crc =CrcCalc16( (unsigned char *)pGwSetup, sizeof(AMC_SETUP_STRUCT)-sizeof(UINT16) );  
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GwConfigInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL AmcConfigInit(void)
{      
    /* clear the structure */
    memset( &AmcConfig, 0x00, sizeof(AmcConfig) );
    
    memset( &AmcSetup, 0x00, sizeof(AmcSetup) );
       
#if 0    
    /* read from flash, if failure use defaults */
    if( !GwReadConfigFromFlash(&GwConfig) )
    {
        GwUseConfigDefaults(&GwConfig);
        
        GwWriteConfigToFlash( &GwConfig );
    }
    
    if( !GwReadSetupFromFlash(&GwSetup) )
    {
        GwUseSetupDefaults(&GwSetup);
        
        GwWriteSetupToFlash( &GwSetup );
    }    
#endif
    
    return TRUE;
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: GwWriteConfigToFlash
*|  Description: Write the configuration to the external flash
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GwWriteConfigToFlash(AMC_CONFIG_STRUCT *pGwConfig)
{   
    memcpy( &GwConfig, pGwConfig, sizeof(AMC_CONFIG_STRUCT));  
    
    GwConfig.crc =CrcCalc16( (unsigned char *)&GwConfig, sizeof(GwConfig)-sizeof(UINT16) );  
    
    /* erase this sector */
    DataFlashEraseSector(MEMORY_CONFIG_ADDRESS/DATAFLASH_SIZE_SECTOR_BYTES);
    
    if( !DataFlashWrite( MEMORY_CONFIG_ADDRESS, sizeof(GW_CONFIG_STRUCT), (UINT8 *)&GwConfig) ) 
    {
        return FALSE;
    }
    
    else    
        return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GwReadConfigFromFlash
*|  Description: Read the configuration from the external flash
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GwReadConfigFromFlash(GW_CONFIG_STRUCT *pGwConfig)
{   
    UINT16 rxCrc =0;
    
    if( !DataFlashRead(MEMORY_CONFIG_ADDRESS, sizeof(GW_CONFIG_STRUCT), (UINT8 *)pGwConfig) )
    {
        return FALSE;
    }
    
    memcpy( &GwConfig, pGwConfig, sizeof(GW_CONFIG_STRUCT));  
    
    rxCrc =CrcCalc16( (unsigned char *)pGwConfig, sizeof(GW_CONFIG_STRUCT)-sizeof(UINT16) );  
        
    if( rxCrc !=pGwConfig->crc )
        return FALSE;
    else    
        return TRUE;
}
#endif

/*
*|----------------------------------------------------------------------------
*|  Routine: AmcGetConfig
*|  Description: Get the configuration from RAM
*|  Retval:
*|----------------------------------------------------------------------------
*/
void AmcGetConfig(AMC_CONFIG_STRUCT *pGwConfig)
{
    memcpy( pGwConfig, &AmcConfig, sizeof(AMC_CONFIG_STRUCT));  
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: GwWriteSetupToFlash
*|  Description: Write the setup to the external flash
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GwWriteSetupToFlash(GW_SETUP_STRUCT *pGwSetup)
{   
    memcpy( &GwSetup, pGwSetup, sizeof(GW_SETUP_STRUCT));  
    
    GwSetup.crc =CrcCalc16( (unsigned char *)&GwSetup, sizeof(GwSetup)-sizeof(UINT16) );  
    
    /* erase this sector */
    DataFlashEraseSector(MEMORY_SETUP_ADDRESS/DATAFLASH_SIZE_SECTOR_BYTES);
    
    if( !DataFlashWrite( MEMORY_SETUP_ADDRESS, sizeof(GW_SETUP_STRUCT), (UINT8 *)&GwSetup) ) 
    {
        return FALSE;
    }
    
    else    
        return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: GwReadSetupFromFlash
*|  Description: Read the setup from the external flash
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GwReadSetupFromFlash(GW_SETUP_STRUCT *pGwSetup)
{   
    UINT16 rxCrc =0;
    
    if( !DataFlashRead(MEMORY_SETUP_ADDRESS, sizeof(GW_SETUP_STRUCT), (UINT8 *)pGwSetup) )
    {
        return FALSE;
    }
    
    memcpy( &GwSetup, pGwSetup, sizeof(GW_SETUP_STRUCT));  
    
    rxCrc =CrcCalc16( (unsigned char *)pGwSetup, sizeof(GW_SETUP_STRUCT)-sizeof(UINT16) );  
        
    if( rxCrc !=pGwSetup->crc )
        return FALSE;
    else    
        return TRUE;
}
#endif

/*
*|----------------------------------------------------------------------------
*|  Routine: AmcGetSetup
*|  Description: Get the setup from RAM
*|  Retval:
*|----------------------------------------------------------------------------
*/
void AmcGetSetup(AMC_SETUP_STRUCT *pGwSetup)
{
    memcpy( pGwSetup, &AmcSetup, sizeof(AMC_SETUP_STRUCT));  
}



#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: GwGetReportInterval
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT16 GwGetSensorRange(void)
{
    return GwSetup.sensorRange1;
}
#endif

/*
*|----------------------------------------------------------------------------
*|  Routine: GwGetShutoffLevel
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT16 GwGetShutoffLevel(void)
{
    return AmcSetup.highLevel;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: GwGetSleepEnabled
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL GwGetSleepEnabled(void)
{   
    return AmcConfig.shedule.sleepEnabled;
}



