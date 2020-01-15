/** C Source *******************************************************************
*
* NAME      lcdHi.c
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

#include "lcdHi.h"

#include "gpioHi.h"
#include "spiHi.h"
#include "sysTimers.h"


/** Functions *****************************************************************/
void Lcd_SetContrast(UINT8 cntrstLvl);

/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_Init
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL Lcd_Init(void)
{  
    SpiInit(LCD_SPI_PORT);
       
    TimerDelayUs(100000); 
       
    Lcd_Clear();
    Lcd_SetContrast(40);
    Lcd_SetBacklight(2);
       
    return TRUE;
}



/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_Send
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL Lcd_Send(UINT8 nbrBytes, char *pTxData)
{      
    int j;    
    
    SpiDeviceInit(LCD_SPI_PERIPH);
    
    /* assert cs */
    spiStart(LCD_SPI_PERIPH);
           
    TimerDelayUs(250); // reduce effective clock rate
     
    for(j=0; j<nbrBytes; j++)
    {              
        SPI_TRANSFER_LCD(*pTxData);
        
        *pTxData ++;                         
        
        TimerDelayUs(150); // reduce effective clock rate
    }

    spiStop(LCD_SPI_PERIPH);

    TimerDelayUs(250); // reduce effective clock rate
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_Clear
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_Clear(void)
{
    char txBuf[3];
  
    txBuf[0] =LCD_PREFIX;
    txBuf[1] =LCD_CLEAR;
    
    Lcd_Send(2, txBuf);
    
    TimerDelayUs(5000); // execution time
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_SetContrast
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_SetContrast(UINT8 cntrstLvl)
{
    char txBuf[3];
  
    txBuf[0] =LCD_PREFIX;
    txBuf[1] =LCD_SET_CNTRST;
    txBuf[2] =cntrstLvl;
    
    Lcd_Send(3, txBuf);
    
    TimerDelayUs(2000); // execution time
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_SetBacklight
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_SetBacklight(UINT8 bckLighLvl)
{
    char txBuf[3];
  
    txBuf[0] =LCD_PREFIX;
    txBuf[1] =LCD_SET_BL_LVL;
    txBuf[2] =bckLighLvl;
    
    Lcd_Send(3, txBuf);
    
    TimerDelayUs(1000); // execution time
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_SetCursor
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_SetCursor(UINT8 pos)
{
    char txBuf[3];
  
    txBuf[0] =LCD_PREFIX;
    txBuf[1] =LCD_SET_CURSOR;
    txBuf[2] =pos;
    
    Lcd_Send(3, txBuf);
    
    TimerDelayUs(100); // execution time
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_DisplayOn
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_DisplayOn(BOOL displayOn)
{
    char txBuf[3];
  
    txBuf[0] =LCD_PREFIX;
    
    if( displayOn )
        txBuf[1] =LCD_DISPLAY_ON;
    else
        txBuf[1] =LCD_DISPLAY_OFF;    
        
    Lcd_Send(2, txBuf);
    
    TimerDelayUs(100); // execution time
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_DisplayFwVer
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_DisplayFwVer(void)
{
    char txBuf[3];
  
    txBuf[0] =LCD_PREFIX;
    txBuf[1] =LCD_DISPLAY_FW_VER;
    
    Lcd_Send(2, txBuf);
    
    TimerDelayUs(4000); // execution time
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_SendString
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_SendString(UINT8 row, char *pBuf)
{   
    UINT8 nbrBytes;
    nbrBytes =strlen( (char *)pBuf);
    
    if( row ==1)
        row =0x00;
    else if(row ==2 ) 
        row =0x40;
    else if(row ==3 ) 
        row =0x14;    
    else
        row =0x54;
    
    Lcd_SetCursor(row);
    Lcd_Send(nbrBytes, pBuf);
    
    for(int j=0; j<(20-nbrBytes); j++)
    {
        Lcd_Send(1, " ");
    }    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Lcd_LcdTest
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Lcd_LcdTest(void)
{    
   
    while(1)
    {         
        Lcd_Clear();
        Lcd_SetContrast(40);
        Lcd_SetBacklight(5);
        Lcd_DisplayOn(FALSE);    
        Lcd_DisplayOn(TRUE);    
        
        Lcd_DisplayFwVer();
          
        Lcd_SetCursor(0x40);
        Lcd_SendString(20, "Hello LCD 0123456789");
    }
}



