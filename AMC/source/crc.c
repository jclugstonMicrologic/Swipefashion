/** C Source ******************************************************************
*
* NAME      crc.c
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

#include "crc.h"

#define CRC16_POLY      0x1021

/*
*|----------------------------------------------------------------------------
*|  Routine: CrcInit32
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void CrcInit32(void)
{
    /* Enable clock for CRC peripheral. */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);            
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: CrcCalc32
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
UINT32 CrcCalc32(UINT8 *pData, UINT16 nbrBytes)
{     
    UINT32 calcCrc =0;
    
    /* calculate CRC. */
    CRC_ResetDR(); /* Reset data reg to initial value 0xffffffff. */
    CRC_CalcBlockCRC( (uint32_t *)pData, nbrBytes/4 );

    //CRC_ResetDR(); /* Reset data reg to initial value 0xffffffff. */
    //CRC_CalcCRC(dwCRC);

    /* read back calculated CRC */
    calcCrc = CRC_GetCRC();
    
    return calcCrc;
}



/*
*|----------------------------------------------------------------------------
*|  Routine: CrcCalc16
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
UINT16 CrcCalc16(UINT8 *pData, UINT16 nbrBytes)
{     
    int i,j;
    int x;
    UINT16 crc, feed;
  
    crc =0;
    
    for( j=0; j<nbrBytes; j++)      
    {
        feed = (unsigned int)(*pData);

        for ( i=0; i<8; i++)
        {
            x = crc & 0x0001;
            crc= crc>>1;
            if ( feed & 0x0001 )
            {
                crc = crc | 0x8000;
            }
            feed = feed>>1;
            if ( x )
            {
                crc = crc ^ CRC16_POLY;
            }
        }
        
        *pData ++;
    }
    
    return crc;
}



