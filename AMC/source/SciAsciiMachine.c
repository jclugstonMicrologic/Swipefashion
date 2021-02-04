/** C Source ******************************************************************
*
* NAME      SciAsciiMachine.c
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
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "SciAsciiMachine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define SCI_TIMEOUT_500MSEC (500)  //500msec
#define SCI_TIMEOUT_100MSEC (100)  //100msec
#define SCI_TIMEOUT_1SEC    (1000) //500msec

// states of the serial receive machine
typedef enum
{
    SCI_ASCII_RX_IDLE_STATE =0,
    SCI_ASCII_RX_CMD_STATE,
  	       
    SCI_ASCII_RX_ERROR_STATE, 
    
    SCI_AT_LAST_STATE   
      
}SciAsciiStatesTypeEnum;


sci_data_t BluetoothDataCom;


void SciAsciiTaskComx(void * pvParameters);
int SciAsciiRxMachine(sci_data_t *pSerialData,char sciPort);


/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: SciAsciiStartReceiver
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SciAsciiStartReceiver(void)
{
    BaseType_t xReturn;
    
    /* spawn the serial receiver task */
    xReturn =xTaskCreate( SciAsciiTaskComx, 
                          "SciAsciiMachine", 
                          1024, //(configMINIMAL_STACK_SIZE+128),
                          NULL, 
                          (tskIDLE_PRIORITY + 2UL), 
                          ( TaskHandle_t * ) NULL );

    
    if(xReturn !=pdPASS)
    {
        //!!!
        return FALSE;
    } 
    else
        return TRUE;  
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciAsciiReceiverInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SciAsciiReceiverInit
(	
    UINT8 sciPort,
    UINT32 baudRate,
    void (*pCallBack)(int, char *),
    void (*pAltCallBack)(int, char *)
)
{
    sci_data_t *pSerialData;
    UINT8 parity =0;
        
    switch(sciPort)
    {
        case SCI_BLUETOOTH_COM:
            pSerialData =&BluetoothDataCom;
            pSerialData->termChar =0xff;
            break;                                   
    }    

    pSerialData->pCmdFunction = pCallBack;
    pSerialData->pCmdAltFunction = pAltCallBack;
      
    pSerialData->common.machState =SCI_ASCII_RX_IDLE_STATE;
    pSerialData->common.timeoutTimer =0;	  
       
    memset( pSerialData->rxBuffer, 0x00, sizeof(pSerialData->rxBuffer) );
    
    /* initialize sciPort serial port */
    SciSerialPortInit( (COMTypeDef)sciPort, baudRate, parity );
   
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: SciAsciiTaskComx
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciAsciiTaskComx(void * pvParameters)
{
    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();
    
    for( ;; )
    {       
        SciAsciiRxMachine(&BluetoothDataCom, SCI_BLUETOOTH_COM);
               
        /* place this task in the blocked state until it is time to run again */
        vTaskDelayUntil( &xNextWakeTime, 1 );  
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciAsciiRxMachine
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
int SciAsciiRxMachine
(
    sci_data_t *pSerialData,
    char sciPort
)
{
    char rxByte =NULL;         
    INT8 status =0; 

    TickType_t xTicks;
                       
    switch( pSerialData->common.machState )
    {
        case SCI_ASCII_RX_IDLE_STATE:                        
            if( SciGetByte( sciPort,&rxByte ) )	
	    {
                //if( rxByte <=0x7f ) 
                {       
                    /* ASCII character, potential NMEA, or AT command response */ 
		    pSerialData->byteCnt =0;				
                    memset( pSerialData->rxBuffer, 0x00, sizeof(pSerialData->rxBuffer) );
		    pSerialData->rxBuffer[pSerialData->byteCnt++] =rxByte;
		    SciStateProcess( &pSerialData->common, SCI_ASCII_RX_CMD_STATE);
		}
	    }
    	    break;
	case SCI_ASCII_RX_CMD_STATE:
            while( SciGetByte( sciPort,&rxByte ) )
	    {
                xTicks=xTaskGetTickCount();
                
            #if 0
                if( rxByte >0x7f )
                {
                    /* not ASCII/printable character, quit */
                    status =-1;
                    break;
                }
            #endif    
                
                pSerialData->rxBuffer[pSerialData->byteCnt] =rxByte;
			
                /* remove this to make the routine more generic */
         	if( pSerialData->rxBuffer[pSerialData->byteCnt] ==pSerialData->termChar &&
                    pSerialData->termChar !=0xff 
                   ) 
	        {                   
                    /* pass, message is valid, process project specific commands */
                    if( pSerialData->pCmdFunction !=NULL_PTR )
                        pSerialData->pCmdFunction(NULL, pSerialData->rxBuffer);                      
                                        
                    status = -1;
                    
                    break;
	        }              
                if(  pSerialData->byteCnt >= (sizeof(pSerialData->rxBuffer)) )
                {
                    /* rx buffer has been overrun, message is too long
                       or termination character(s) were missed ('\n', "OK", etc)
                       goto error condition for now, application should probably just go idle 
                    */                  
                    //SciStateProcess( &pSerialData->common, pSerialData->common.machState, SCI_ASCII_RX_ERROR_STATE);                    
                    
                    /* rx buffer is half full, lets parse any messages that may be present at this time
                       process project specific command(s) 
                    */
                    if( pSerialData->pCmdFunction !=NULL_PTR )
                        pSerialData->pCmdFunction(NULL, pSerialData->rxBuffer);      
                    
                    status = -1;
                    break;
                }
            
                pSerialData->byteCnt ++;
                
                /* restart timeout timer */
                pSerialData->common.timeoutTimer =xTicks;	 
            }
            
            if( SciCheckTimeout( &pSerialData->common, 25) )//SCI_TIMEOUT_100MSEC) )//SCI_TIMEOUT_500MSEC) )
            {
                /* no more bytes coming in, lets parse any messages that may be present at this time
                   process project specific command(s) 
                */
                if( pSerialData->pCmdAltFunction !=NULL_PTR )
                    pSerialData->pCmdAltFunction(pSerialData->byteCnt, pSerialData->rxBuffer);              
                                   
            #if 1              
                for(int j=0; j<sizeof(pSerialData->rxBuffer); j++)
                {
                    /* clear any null characters from the buffer */
                    if(pSerialData->rxBuffer[j] ==0x00 )
                        pSerialData->rxBuffer[j] =0x7e;
                }
            #endif
                
                if( pSerialData->pCmdFunction !=NULL_PTR )
                    pSerialData->pCmdFunction(NULL, pSerialData->rxBuffer);     
                                                                  
                status = -1;
                break;               
	    }
            break;
        case SCI_ASCII_RX_ERROR_STATE:
            /* ERROR */
            break;
    }		
	
    if( status == -1 )
    {
        /* done for whatever reason, go to idle state */
        SciStateProcess( &pSerialData->common, SCI_ASCII_RX_IDLE_STATE);
    }

    
    return status;   
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: SendString
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciAsciiSendString(UINT8 port, char *pBuf)
{
#ifdef TERMINAL_DEBUG_ENABLED
    SciTxPacket(port, strlen(pBuf), pBuf);      
#endif    
}

