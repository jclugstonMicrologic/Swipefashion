/** C Source ******************************************************************
*
* NAME      IridiumMachine.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded worbench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "IridiumMachine.h"
#include "ServerPacket.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "gpioHi.h"
#include "stdio.h"

static const char* STARTUP_MSGS[] ={
                                    "ATZ\r\n", //"AT+CGMI\r",
                                    "AT+CGMM\r\n",
                                    "AT+CGSN\r\n",
                                    "AT+CSQ\r\n"
                                   };

typedef enum
{
    IRIDIUM_INIT_STATE =0,   
    IRIDIUM_IDLE_STATE,
    IRIDIUM_RSSI_STATE,
    IRIDIUM_INIT_SBD_STATE,    
    IRIDIUM_TX_SBD_STATE,
 
    IRIDIUM_LAST_STATE
      
}IridiumStatesTypeEnum;


enum
{
    IRIDIUM_SBD_STATE0 =0,   
    IRIDIUM_SBD_STATE1,
    IRIDIUM_SBD_STATE2,
    IRIDIUM_SBD_STATE3,
    IRIDIUM_SBD_STATE4,
    IRIDIUM_SBD_STATE5,
       
    IRIDIUM_SBD_LAST_STATE
      
};

typedef struct
{
    IridiumStatesTypeEnum machState;
    
    UINT32 sleepTimer;
          
}IRIDIUM_STRUCT;


typedef struct
{
    UINT8 prevState;
    UINT8 machState;
    UINT32 timeoutTimer;
}STATE_INFO;

typedef struct
{
    UINT8 errorCode;
    UINT8 rssi;
    UINT8 imei[15]; 
}IRIDIUM_INFO;

IRIDIUM_STRUCT IridiumData;
IRIDIUM_INFO IridiumInfo;

STATE_INFO IridSbdTransfer;
STATE_INFO IridStartup;


BOOL IridiumAck;


void IridiumProcessCommands(int cmd,char *pCcmd);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: IridiumMachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL IridiumMachineInit(void)
{
#if 0  
    if( !SciAsciiReceiverInit(SCI_IRIDIUM_COM, SCI_IRIDIUM_BAUD_RATE, IridiumProcessCommands, NULL_PTR) )
    {
        //!!!
        return FALSE;
    }         
     
    //POWER_5V_ON;
    //SAT_ENABLE;        
    //SAT_ON;
    
    memset( &IridiumData, 0x00, sizeof(IridiumData) );

    memset( &IridiumInfo, 0x00, sizeof(IridiumInfo) );
    
    IridSbdTransfer.machState =0;
    IridStartup.machState =0;
#endif

    return TRUE;    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: IridiumCheckTimeout
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL IridiumCheckTimeout
(
    STATE_INFO *stateInfo,
    UINT32 timeout  //msec
)
{  
   if( (xTaskGetTickCount() - stateInfo->timeoutTimer) >timeout )
   {
      return TRUE;
   }
   
   return FALSE;   
} 

/*
*|----------------------------------------------------------------------------
*|  Routine: IridiumStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void IridiumStateProcess
(
    STATE_INFO *stateInfo,
    UINT8 nextState
)
{
    stateInfo->prevState =stateInfo->machState;
    stateInfo->machState =nextState;
    
    stateInfo->timeoutTimer =xTaskGetTickCount();
}


//#define RSSI_TEST
/*
*|----------------------------------------------------------------------------
*|  Routine: IridiumStartup
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL IridiumStartup(void)
{
    static UINT8 msgCnt =0;    
    BOOL status =FALSE;
           
#if 0    
    switch(IridStartup.machState)
    {
        case 0:
            IridiumAck =FALSE;
            //POWER_5V_ON;
        #ifdef RSSI_TEST
            SciTxPacket(SCI_IRIDIUM_COM, strlen(STARTUP_MSGS[3]), (char *)STARTUP_MSGS[3]);
        #else
            SciTxPacket(SCI_IRIDIUM_COM, strlen(STARTUP_MSGS[msgCnt]), (char *)STARTUP_MSGS[msgCnt]);
        #endif
            
            msgCnt++;
            
            IridiumStateProcess(&IridStartup, 1); 
            break;
        case 1:
            if( IridiumAck )
            {                
                IridiumAck =FALSE;            
        
            #ifdef RSSI_TEST                
                IridiumStateProcess(&IridStartup, 0); 
            #else
                SciTxPacket(SCI_IRIDIUM_COM, strlen(STARTUP_MSGS[msgCnt]), (char *)STARTUP_MSGS[msgCnt]);
            
                if( ++msgCnt >= 4 )
                {
                    IridiumStateProcess(&IridStartup, 2); 
                }
            #endif
            }
            break;
        case 2:
            if( IridiumAck )
            {
                IridiumAck =FALSE;              
                status =TRUE;
            }
            break;
        case 3:
            /* in this state because of RSSI test */
            if( IridiumCheckTimeout(&IridStartup, 1000) )
            {
                IridiumStateProcess(&IridStartup, 0);               
            }
            break;
    }  
    
    if( IridiumCheckTimeout(&IridStartup, 10000) )
    {
        // ???
        msgCnt =0;
        IridiumStateProcess(&IridStartup, 0); 
    }
#endif    
    return status;
}


//#define IRIDIUM_TEST 
/*
*|----------------------------------------------------------------------------
*|  Routine: IridiumSBDTransfer
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL IridiumSBDTransfer(UINT16 nbrBytes, char *pTxData)
{
    char sbdStr[15];
    UINT16 j, chkSum =0;
    BOOL status =FALSE;
    
#if 0    
    memset(sbdStr, 0x00, sizeof(sbdStr));
            
    switch(IridSbdTransfer.machState)
    {
        case IRIDIUM_SBD_STATE0:
            IridiumAck =FALSE;
            
            /* write binary data to the ISU */
        #ifdef IRIDIUM_TEST
            sprintf(sbdStr, "AT+SBDWB=5\r");
        #else
            sprintf(sbdStr, "AT+SBDWB=%d\r\n", nbrBytes);                        
        #endif    
            
            SciTxPacket(SCI_IRIDIUM_COM, strlen(sbdStr), sbdStr);      
            
            IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE1); 
            break;
        case IRIDIUM_SBD_STATE1:
            /* wait for Iridium response/ack */
            if( IridiumAck )
            {
                IridiumAck =FALSE;                                         
            #ifdef IRIDIUM_TEST
                sprintf(sbdStr, "hello");                               
                SciTxPacket(SCI_IRIDIUM_COM, 5, sbdStr); 
                
                for(j=0; j<5; j++)
                {
                    chkSum +=sbdStr[j];
                }

                sbdStr[0] =((chkSum&0xff00)>>8);
                sbdStr[1] = (chkSum&0x00ff);
                
                SciTxPacket(SCI_IRIDIUM_COM, sizeof(chkSum), sbdStr); 
            #else                
                /* start binary transfer to ISU */
                for(j=0; j<nbrBytes; j++)
                {
                    chkSum +=pTxData[j];
                }
                sbdStr[0] =((chkSum&0xff00)>>8);
                sbdStr[1] = (chkSum&0x00ff);                
                
                SciTxPacket(SCI_IRIDIUM_COM, nbrBytes, pTxData); 
                SciTxPacket(SCI_IRIDIUM_COM, sizeof(chkSum), sbdStr); 
            #endif
                
                IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE2); 
            }
            break;
        case IRIDIUM_SBD_STATE2:
            /* wait for Iridium response/ack */
            if( IridiumAck )
            {
                IridiumAck =FALSE;
                              
                // check IridiumInfo.errorCode ???, 
                // should be '0' for pass
                
                /* instruct the ISU to initiate a SBD transfer */
                strcpy(sbdStr, "AT+SBDIX\r");
                SciTxPacket(SCI_IRIDIUM_COM, strlen(sbdStr), sbdStr);   
                
                IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE3); 
            }
            break;            
        case IRIDIUM_SBD_STATE3:
            /* wait for Iridium response/ack */
            if( IridiumAck )
            {
                IridiumAck =FALSE;
                
                /* instruct the ISU to clear the message from the send buffer */
                strcpy(sbdStr, "AT+SBDD0\r");
                SciTxPacket(SCI_IRIDIUM_COM, strlen(sbdStr), sbdStr);   
                
                IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE4); 
            }
            break;                        
        case IRIDIUM_SBD_STATE4:
            /* wait for Iridium response/ack */
            if( IridiumAck )
            {
                IridiumAck =FALSE;
                
               #if 0
                /* instruct the ISU to transfer the received message */
                strcpy(sbdStr, "AT+SBDRB\r");
                SciTxPacket(SCI_IRIDIUM_COM, strlen(sbdStr), sbdStr);   
                
                IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE5); 
               #else
                status =TRUE;

                IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE0); 
               #endif
            }
            break;            
        case IRIDIUM_SBD_STATE5:
            /* wait for Iridium response/ack */
            if( IridiumAck )
            {
                IridiumAck =FALSE;
                status =TRUE;
            
                IridiumStateProcess(&IridSbdTransfer, IRIDIUM_SBD_STATE0); 
            }            
            break;
    }
    
    if( IridiumCheckTimeout(&IridSbdTransfer, 1000) )
    {
        // ???
    }
#endif    
    return status;
}



/*
*|----------------------------------------------------------------------------
*|  Routine: IridiumMachine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL IridiumMachine(void)
{
    static IRIDIUM_PACKET_STRUCT iridiumPacket;
    
#if 0    
    switch( IridiumData.machState )
    {
        case IRIDIUM_INIT_STATE:
            if( IridiumStartup() )
            {
                /* startup successful */
                IridiumAck =FALSE;
                          
                IridiumData.machState =IRIDIUM_IDLE_STATE;
            }
            break;
        case IRIDIUM_IDLE_STATE:                 
            /* monitor RSSI */
            SciTxPacket(SCI_IRIDIUM_COM, strlen(STARTUP_MSGS[3]), (char *)STARTUP_MSGS[3]);
            IridiumData.machState =IRIDIUM_RSSI_STATE;
            break;
        case IRIDIUM_RSSI_STATE:
            /* wait for Iridium response/ack */
            if( IridiumAck )
            {
                IridiumAck =FALSE;
                
                if( IridiumInfo.rssi >0  /* && 
                     time to send message ??? */
                  )
                {
                    /*
                    if we need to send message and RSSI is OK
                    do it now, else go back to RSSI state 
                    */
                    IridiumData.machState =IRIDIUM_INIT_SBD_STATE; 
                }
                else
                    IridiumData.machState =IRIDIUM_IDLE_STATE; 
            }
            break;              
        case IRIDIUM_INIT_SBD_STATE: 
            /* build our data packet */
            BuildIridiumServerPacket(&iridiumPacket);      
            IridiumData.machState =IRIDIUM_TX_SBD_STATE;
            break;       
        case IRIDIUM_TX_SBD_STATE:            
            if( IridiumSBDTransfer(sizeof(iridiumPacket), (char *)&iridiumPacket) )
            {
                /* transfer to ISU successful, we are done */
                IridiumData.machState =IRIDIUM_IDLE_STATE;
            }
            break;                         
    }
#endif    
    return TRUE;
}

/*
*|------------------------------------------------------------------------------
*|  Routine: IridiumProcessCommands
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
void IridiumProcessCommands
(
    int cmd,
    char *pCmd
)
{  
    char *ptr;
   
    if( strstr(pCmd,"OK") ==0 &&
        strstr(pCmd,"READY") ==0
      )
    {
        return;
    }
      
    if( strstr(pCmd,"+CSQ:") !=0 )
    {
        /* get rssi value */
        ptr =strstr( pCmd, ":" );
						
        IridiumInfo.rssi =( *(ptr+1)-'0');      
    }
    else if( strstr(pCmd,"AT+CPIN:") !=0 )
    {
//        *pCmd =AT_CMD_CPIN;      
    }    
    else if( strstr(pCmd,"READY") !=0 )
    {
    }     
    else if( strstr(pCmd, "+SBDI:") !=0 )
    {
    }     
    else if( strstr(pCmd, "+CGMI") !=0 )
    {
        /* manufacturer identification */
    }         
    else if( strstr(pCmd, "+CGMM") !=0 )
    {
        /* model identification */
    }         
    else if( strstr(pCmd, "+CGMR:") !=0 )
    {
        /* revision */
    }             
    else if( strstr(pCmd, "+CGSN") !=0 )
    {
        /* serial number (IMEI) */
        ptr =strstr( pCmd, "N" );
        memcpy( (char *)IridiumInfo.imei,  (char *)(ptr+4), sizeof(IridiumInfo.imei));
        
    }                 
    else if( strstr(pCmd, "0") !=0 )
    {
        IridiumInfo.errorCode = *(pCmd+2);
    }       
          
    IridiumAck =TRUE;    
    /* echo for debugging */    
  //  SciTxPacket(SCI_IRIDIUM_COM, strlen(pCmd), pCmd);
} 


/*
*|------------------------------------------------------------------------------
*|  Routine: IridiumGetRssi
*|  Description:
*|  Retval:
*|------------------------------------------------------------------------------
*/
UINT8 IridiumGetRssi(void)
{
    return IridiumInfo.rssi;    
}


