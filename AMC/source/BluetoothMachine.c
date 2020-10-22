/** C Source ******************************************************************
*
* NAME      BluetoothMachine.c
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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "stdio.h"

#include "BluetoothMachine.h"

/** Local Constants and Types *************************************************/
//#define TERMINAL_DEBUG_ENABLE

typedef enum
{
    BLUETOOTH_INIT_STATE =0,
    
    BLUETOOTH_IDLE_STATE,
     
    BLUETOOTH_LAST_STATE
      
}BluetoothStatesTypeEnum;

typedef struct
{
    BluetoothStatesTypeEnum machState;
    
    UINT32 sleepTimer;
          
}ble_t;


/** Local Variable Declarations ***********************************************/
ble_t BluetoothData;


/** Local Function Prototypes *************************************************/
void BluetoothProcessCommands(int cmd,char *pCmd);
void BluetoothSendString(char *pBuf);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: BluetoothMachine_Init
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL BluetoothMachine_Init(void)
{
    if( !SciAsciiReceiverInit(SCI_BLUETOOTH_COM, SCI_BLUETOOTH_BAUD_RATE, BluetoothProcessCommands, NULL_PTR) )
    {
        //!!!
        return FALSE;
    }         
     
    memset( &BluetoothData, 0x00, sizeof(BluetoothData) );
    
    return TRUE;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: BluetoothMachine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL BluetoothMachine(void)
{  
    switch( BluetoothData.machState )
    {
        case BLUETOOTH_INIT_STATE:
            BluetoothData.machState =BLUETOOTH_IDLE_STATE;
            break;
        case BLUETOOTH_IDLE_STATE:
          //while(1)
          {
          BluetoothSendString("$$$");  //enter command mode        
          }
#if 0
BluetoothSendString("+\r\n"); //echo on
BluetoothSendString("SS,C0\r\n"); // enable device information and UART transparent service

BluetoothSendString("R,1\r\n"); // reboot the module
BluetoothSendString("D\r\n"); // display settings (ensure serive is no C0

//BluetoothSendString("F\r\n"); // initiate scan
//BluetoothSendString("C,<0,1><MAC address>\r\n"); 

/* send some info */
BluetoothSendString("Hello RN4871");

/* kill connection */
BluetoothSendString("$$$");
BluetoothSendString("K,1\r\n");
#endif
            BluetoothData.machState =BLUETOOTH_LAST_STATE;
            break;       
    }
    
    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: BluetoothProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void BluetoothProcessCommands
(
    int cmd,
    char *pCmd
)
{
    
    switch(*pCmd)
    {
        case '0':
            NVIC_SystemReset();
            break;
        case '1':
        #ifdef TERMINAL_DEBUG_ENABLE
            sprintf(printStr, "TIME: %x %x %x\n", RtcTimeStruct.RTC_Hours, RtcTimeStruct.RTC_Minutes, RtcTimeStruct.RTC_Seconds);        
            printf(printStr);
        #endif                                 
//            data =RtcReadBackupData(0);
            break;
        case '2':
//            RtcWriteBackupData(0, 0x12345678);
            break;            
        default:          
            break;
    }
           
    /* echo for debugging */    
    SciTxPacket(SCI_BLUETOOTH_COM, strlen(pCmd), pCmd);         
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: BluetoothSendString
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void BluetoothSendString(char *pBuf)
{
    SciTxPacket(SCI_BLUETOOTH_COM, strlen(pBuf), pBuf);      
}



