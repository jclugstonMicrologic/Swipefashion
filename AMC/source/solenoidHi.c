/** C Source ******************************************************************
*
* NAME      gpioHi.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v8.20.2
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2020, MICROLOGIC
* Calgary, Alberta, Canada, www.micrologic.ab.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "gpioHi.h"

#include "sysTimers.h"
#include "solenoidHi.h"

#define PULSE_PERIOD 10000 //msec

/** Functions *****************************************************************/


void Solenoid_Init(void)
{
    memset(&BoardStatus, 0x00, sizeof(BoardStatus) );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: OpenValve
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_OpenValve(uint8_t valveNbr)
{
    switch(valveNbr)
    {
        case 1:
          S_DRIVE1_ASSERT;S_PULSE1_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE1_OFF;                    
          break;
        case 2:
          S_DRIVE2_ASSERT;S_PULSE2_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE2_OFF;                    
          break;          
        case 3:
          S_DRIVE3_ASSERT;S_PULSE3_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE3_OFF;                    
          break; 
        case 4:
          S_DRIVE4_ASSERT;S_PULSE4_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE4_OFF;                              
          break;          
        case 5:
          S_DRIVE5_ASSERT;S_PULSE5_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE5_OFF;                    
          break;          
        case 6:
          S_DRIVE6_ASSERT;S_PULSE6_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE6_OFF;                    
          break;  
        case 7:
          S_DRIVE7_ASSERT;S_PULSE7_ON;                    
          TimerDelayUs(PULSE_PERIOD);
          S_PULSE7_OFF;                    
          break;          
    }   
    
    BoardStatus.valve |=(0x01<< (valveNbr-1) );
}

/*
*|----------------------------------------------------------------------------
*|  Routine: CloseValve
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_CloseValve(uint8_t valveNbr)
{
    switch(valveNbr)
    {
        case 1:
          S_DRIVE1_NEGATE;
          break;
        case 2:
          S_DRIVE2_NEGATE;
          break;          
        case 3:
          S_DRIVE3_NEGATE;
          break;  
        case 4:
          S_DRIVE4_NEGATE;
          break;  
        case 5:
          S_DRIVE5_NEGATE;
          break;  
        case 6:
          S_DRIVE6_NEGATE;
          break;  
        case 7:
          S_DRIVE7_NEGATE;
          break;                    
    }   
    
    BoardStatus.valve &= ~(0x01<< (valveNbr-1) );
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Solenoid_CloseAllValves
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_CloseAllValves(void)
{
    for(int j=0; j<NBR_VALVES; j++)
    {
        Solenoid_CloseValve(j+1);
    }
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Solenoid_StartCompr
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_StartCompr(uint8_t compr)
{
    if( (compr &COMPR_LO) )
    {
        COMP_LO_ASSERT;
        BoardStatus.status.b.compr |= COMPR_LO;
    }    
    
    if( (compr &COMPR_HI) )
    {
        COMP_HI_ASSERT;
        BoardStatus.status.b.compr |= COMPR_HI;
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Solenoid_StopCompr
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_StopCompr(uint8_t compr)
{
    if( (compr &COMPR_LO) )
    {
        COMP_HI_NEGATE;
        BoardStatus.status.b.compr &= ~COMPR_LO;
    }   
    
    if( (compr &COMPR_HI) )      
    {
        COMP_LO_NEGATE;
        BoardStatus.status.b.compr &= ~COMPR_HI;
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Solenoid_SetCompr
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_SetCompr(uint8_t cState)
{
    if( (cState &COMPR_LO) )
    {
        COMP_LO_ASSERT;
        BoardStatus.status.b.compr |= COMPR_LO;
    }    
    else
    {
        COMP_LO_NEGATE;
        BoardStatus.status.b.compr &= ~COMPR_LO;      
    }
    
    if( (cState &COMPR_HI) )
    {
        COMP_HI_ASSERT;
        BoardStatus.status.b.compr |= COMPR_HI;
    } 
    else
    {
        COMP_HI_NEGATE;
        BoardStatus.status.b.compr &= ~COMPR_HI;
    }    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Solenoid_OpenReliefValve
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_OpenReliefValve(uint8_t reliefValve)
{
    switch(reliefValve)
    {
        case RELIEF_LO:
          //RELIEF_LO_ASSERT;
          BoardStatus.status.b.relief |= RELIEF_LO;
          break;  
        case RELIEF_HI:
          RELIEF_HI_ASSERT;
          BoardStatus.status.b.relief |=RELIEF_HI;
          break;  
    }    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Solenoid_CloseReliefValve
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Solenoid_CloseReliefValve(uint8_t reliefValve)
{
    switch(reliefValve)
    {
        case RELIEF_LO:
          //RELIEF_LO_NEGATE;
          BoardStatus.status.b.relief &= ~RELIEF_LO;
          break;  
        case RELIEF_HI:
          RELIEF_HI_NEGATE;
          BoardStatus.status.b.relief &= ~RELIEF_HI;
          break;  
    }    
}

    