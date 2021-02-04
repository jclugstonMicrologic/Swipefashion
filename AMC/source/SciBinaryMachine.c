/** C Source ******************************************************************
*
* NAME      SciBinaryMachine.c
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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "crc.h"

#include "gpioHi.h"
#include "SciBinaryMachine.h"

#include "SciAsciiMachine.h"

/** Local Variable Declarations ***********************************************/
#define SERIAL_TIMEOUT_500MSEC (500)  //500msec
#define SERIAL_TIMEOUT_100MSEC (100)  //100msec
#define SERIAL_TIMEOUT_1SEC    (1000) //500msec


/* states of the serial receive machine */
typedef enum
{
    SCI_BINARY_RX_IDLE_STATE =0,
    SCI_BINARY_RX_STX_STATE,
    SCI_BINARY_RX_HEADER_STATE,
    SCI_BINARY_RX_PAYLOAD_STATE,

    SCI_BINARY_RX_PASS_THRU,
  	       
    SCI_BINARY_LAST_STATE   
      
}SciBinaryStatesTypeEnum;


sci_data_t PcDataCom;

sci_data_t BleDataCom;

UINT8 AsciiBuf[256];
/** Local Function Prototypes *************************************************/
void SciBinaryTaskComx(void * pvParameters);
int SciBinaryRxMachine(sci_data_t *pSerialData,char sciPort);


/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: SciBinaryStartReceiver
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SciBinaryStartReceiver(void)
{
    BaseType_t xReturn;
    
    /* spawn the serial receiver task */
    xReturn =xTaskCreate( SciBinaryTaskComx, 
                          "SciBinaryRxMachine", 
                          1024, //configMINIMAL_STACK_SIZE,  
                          NULL, 
                          (tskIDLE_PRIORITY + 2UL), 
                          ( TaskHandle_t * ) NULL);
    
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
*|  Routine: SciBinaryReceiverInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL SciBinaryReceiverInit
(	
    UINT8 sciPort,
    UINT32 baudRate,
    sci_data_t *pDataCom,
    void (*pCallBack)(int, char *) 
)
{
    sci_data_t *pSerialData;
    UINT8 parity =0;
    
    switch(sciPort)
    {
         case SCI_PC_COM:
            pSerialData =&PcDataCom;
            pSerialData->pCheckSumFunction =CrcCalc16;
            break;
         case SCI_BLUETOOTH_COM:
            pSerialData =&BleDataCom;
            pSerialData->pCheckSumFunction =CrcCalc16;
            break;            
    }    

    pSerialData->pCmdFunction = pCallBack;
      
    pSerialData->common.machState =SCI_BINARY_RX_IDLE_STATE;
    pSerialData->common.timeoutTimer =0;	  
       
    memset( pSerialData->rxBuffer, 0x00, sizeof(pSerialData->rxBuffer) );
    
    /* initialize sciPort serial port */
    SciSerialPortInit( (COMTypeDef)sciPort, baudRate, parity );
   
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciBinaryTaskComx
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciBinaryTaskComx(void * pvParameters)
{
    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();
    
//char SerialTxBuffer[16];

    for( ;; )
    {        
        SciBinaryRxMachine(&PcDataCom, SCI_PC_COM);
        
        SciBinaryRxMachine(&BleDataCom, SCI_BLUETOOTH_COM);
        
        /* place this task in the blocked state until it is time to run again */
        vTaskDelayUntil( &xNextWakeTime, 1 );         
    }
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciPopulateHeader
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciPopulateHeader(sci_data_t *pSciHdr, char *pRxBuf )
{
    pSciHdr->nbrBytes =(*pRxBuf<<8);
    *pRxBuf ++;
    
    pSciHdr->nbrBytes |= *pRxBuf;
    *pRxBuf ++;
          
    pSciHdr->command =(*pRxBuf<<8);
    *pRxBuf ++;
    
    pSciHdr->command |= *pRxBuf;
    *pRxBuf ++;    
    
    //pSciHdr->rxPacketNbr =(*pRxBuf<<8);
    //*pRxBuf ++;
    
    //pSciHdr->rxPacketNbr |= *pRxBuf;
    //*pRxBuf ++;        
    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciBinaryRxMachine
*|  Description:  <DLE><STX><NbrBytesMSB><NbrBytesLSB><CommandMSB><CommandLSB>
*|                <PAYLOAD><crcMSB><crcLSB>
*|  Retval:
*|----------------------------------------------------------------------------
*/
int SciBinaryRxMachine
(
    sci_data_t *pSerialData,
    char sciPort
)
{
    char rxByte =NULL;         
    INT8 status =0; 
  
    UINT16 calculatedCrc, rxCrc;    

    if( pSerialData ==NULL )
        return 0;
    
    switch( pSerialData->common.machState )
    {
        case SCI_BINARY_RX_IDLE_STATE:            
            if( SciGetByte( sciPort,&rxByte ) )	
	    {
                if( rxByte ==SCI_DLE )
                {
		    pSerialData->byteCnt =0;		
                    memset( pSerialData->rxBuffer, 0x00, sizeof(pSerialData->rxBuffer) );
                    
		    pSerialData->rxBuffer[pSerialData->byteCnt++] =rxByte;
		    SciStateProcess( &pSerialData->common, SCI_BINARY_RX_STX_STATE);                                         
		}   
                else
                {
		    pSerialData->rxBuffer[pSerialData->byteCnt++] =rxByte;
                    
                    if(pSerialData->byteCnt> 250)
                        pSerialData->byteCnt =0;
                }
	    }
    	    break;
        case SCI_BINARY_RX_STX_STATE:
            if( SciGetByte( sciPort,&rxByte ) )	
            {
                if( rxByte ==SCI_STX )  
                {      
                    pSerialData->rxBuffer[pSerialData->byteCnt++] =rxByte;                
                    
                    SciStateProcess( &pSerialData->common, SCI_BINARY_RX_HEADER_STATE);
                }            
            }
            if( SciCheckTimeout( &pSerialData->common, SERIAL_TIMEOUT_100MSEC) )
            {
                status = -1;
                break;
            }               
            break;            
	case SCI_BINARY_RX_HEADER_STATE:
    	    while( pSerialData->byteCnt <SIZEOF_HEAD )
	    {
	        while( SciGetByte( sciPort,&rxByte ) )
	        {                     
                    /* restart timeout timer */
                    pSerialData->common.timeoutTimer =xTaskGetTickCount();
                    
		    pSerialData->rxBuffer[pSerialData->byteCnt++] =rxByte;
				
                    if(pSerialData->byteCnt ==SIZEOF_HEAD )
                    {
                        SciPopulateHeader( pSerialData, &pSerialData->rxBuffer[SCI_START_OF_HEADER_INFO] );
                    
                        SciStateProcess( &pSerialData->common, SCI_BINARY_RX_PAYLOAD_STATE);

                        break;
                    }                                        
                }

                if( SciCheckTimeout( &pSerialData->common, SERIAL_TIMEOUT_100MSEC) )
                {
                    status = -1;
                    break;
                }   
	    }
            break;
	case SCI_BINARY_RX_PAYLOAD_STATE: /* get remaining bytes */
            while( SciGetByte( sciPort,&rxByte ) )
            {
                /* restart timeout timer */
                pSerialData->common.timeoutTimer =xTaskGetTickCount();
                
                pSerialData->rxBuffer[pSerialData->byteCnt++] =rxByte;

                if( pSerialData->byteCnt ==pSerialData->nbrBytes)
                {
                    /* termination bytes found */
                    /* check rx CRC with calculated CRC */
				
                    /* calculate the CRC on received data */
                    //calculatedCrc =CrcCalc16( (unsigned char *)pSerialData->rxBuffer, (pSerialData->byteCnt-2));
                    calculatedCrc =pSerialData->pCheckSumFunction( (unsigned char *)pSerialData->rxBuffer, (pSerialData->byteCnt-2));                     
                      
                    /* get the received CRC */
                    rxCrc =(pSerialData->rxBuffer[pSerialData->byteCnt-2]<<8) | pSerialData->rxBuffer[pSerialData->byteCnt-1];

                    /* remove always pass condition once rxCrc working */
                    //rxCrc =calculatedCrc;
                    if( rxCrc ==calculatedCrc )					
                    {
                        /* pass, message is valid, process receiver specific commands */
                        if( pSerialData->pCmdFunction !=NULL )
                            pSerialData->pCmdFunction( pSerialData->command, &pSerialData->rxBuffer[SIZEOF_HEAD]);
                                            
                    }
                                       
                    SciStateProcess( &pSerialData->common, SCI_BINARY_RX_IDLE_STATE); 
                    return -1;
                } 				
            }   
	    	
            if( SciCheckTimeout( &pSerialData->common, SERIAL_TIMEOUT_500MSEC) )
            {
                status =-1;
                break;
            }	    	       			       			
            break;																		
    }		
	
    if( status == -1 )
    {
        SciStateProcess( &pSerialData->common, SCI_BINARY_RX_IDLE_STATE); 
    }
    
    return status;
    
} 




/*
*|----------------------------------------------------------------------------
*|  Routine: SciSendPacket
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciSendPacket(UINT8 sciPort, UINT16 cmd, UINT16 nbrBytes, char *pPayload)
{   
    static UINT8 txBuf[256];
    UINT16 j =0;
    UINT16 calculatedCrc;
    UINT16 payloadSize;
    
    payloadSize =nbrBytes +SIZEOF_HEAD_TAIL;
    
    txBuf[j++] =SCI_DLE;
    txBuf[j++] =SCI_STX;
    txBuf[j++] =((payloadSize &0xff00)>>8);
    txBuf[j++] =((payloadSize &0x00ff)>>0);
    txBuf[j++] =((cmd &0xff00)>>8);
    txBuf[j++] =((cmd &0x00ff)>>0);    
    
    for(j=j; j<(nbrBytes+SIZEOF_HEAD); j++)
    {
        txBuf[j] = *pPayload++;
    }

    /* calculate the CRC of the packet */
    calculatedCrc =CrcCalc16( (unsigned char *)txBuf, (nbrBytes+SIZEOF_HEAD) );
        
    for(j=0; j<(nbrBytes+SIZEOF_HEAD); j++)
    {
        SciSendByte(sciPort, txBuf[j]);
    }    
        
    SciSendByte(sciPort, ((calculatedCrc&0xff00)>>8) );
    SciSendByte(sciPort, ((calculatedCrc&0x00ff)>>0) );    
}



/*
*|----------------------------------------------------------------------------
*|  Routine: Sci_GetAsciiString
*|  Description:
*|   Checks serial rx register for ASCII data. 
*|  Retval: UINT8
*|----------------------------------------------------------------------------
*/
UINT8 Sci_GetAsciiString
(
   char sciPort,
   char *pString
)
{    
    int k=0;
    
    memset( AsciiBuf, 0x00, sizeof(AsciiBuf));
    
    for(int j=0; j<256; j++)
    {
        //if(RxBuff[sciPort].q[j]<0x7f && RxBuff[sciPort].q[j]>=0x20)          
        if(BleDataCom.rxBuffer[j]<0x7f && BleDataCom.rxBuffer[j]>=0x20)          
            AsciiBuf[k++]= BleDataCom.rxBuffer[j];
    }
       
    //if( strstr( (char *)RxBuff[sciPort].q, pString) !=0 )
    if( strstr( (char *)AsciiBuf, pString) !=0 )
    {
        BleDataCom.byteCnt =0;
        memset( &BleDataCom.rxBuffer, 0x00, sizeof(BleDataCom.rxBuffer));
        return 1;
    }
    else
        return 0;
}