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

#include "rtcHi.h"
#include "gpioHi.h"


#include "AmcConfig.h"
#include "PCMachine.h"
#include "PressureTdrHi.h"
#include "MainControlTask.h"
#include "BluetoothMachine.h"

#include "sysTimers.h"

/** Local Constants and Types *************************************************/
//#define TERMINAL_DEBUG_ENABLE

typedef enum
{
    BLUETOOTH_INIT_STATE =0,

    BLUETOOTH_WAIT_RESPONSE_STATE,
    BLUETOOTH_IDLE_STATE,
    BLUETOOTH_START_NOTIFY_STATE,
    BLUETOOTH_WAIT_FOR_CLIENT_STATE,
    
    BLUETOOTH_LAST_STATE
      
}BluetoothStatesTypeEnum;


typedef struct
{
    BluetoothStatesTypeEnum machState;
    BluetoothStatesTypeEnum prevMachState;
        
    UINT32 timer;
    UINT32 sleepTimer;    
          
}ble_t;



/** Local Variable Declarations ***********************************************/
ble_t BleData;

char BleSerialTxBuffer[64]; /* transmit buffer */   

/** Local Function Prototypes *************************************************/
void Ble_ProcessCommands(int cmd,char *pCmd);
void Ble_SendString(char *pBuf);

/** Functions *****************************************************************/


/*
*|----------------------------------------------------------------------------
*|  Routine: Ble_MachineInit
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL Ble_MachineInit(void)
{
    //if( !SciAsciiReceiverInit(SCI_BLUETOOTH_COM, SCI_BLUETOOTH_BAUD_RATE, Ble_ProcessCommands, NULL_PTR) )
    if( !SciBinaryReceiverInit(SCI_BLUETOOTH_COM, SCI_BLUETOOTH_BAUD_RATE, NULL_PTR, Ble_ProcessCommands) )
    {
        //!!!
        return FALSE;
    }         
     
    memset( &BleData, 0x00, sizeof(BleData) );

    return TRUE;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Ble_Notify_Timer_Callback
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
static void Ble_Notify_Timer_Callback (void * pvParameter)
{ 
#if 1  
    //uint16_t length =0;
    char data_array[16];

static uint16_t var=300;
var -=1;

    sprintf(data_array, "count: %d", var);
    //length =strlen(data_array);
    
    Ble_SendString(data_array); 
#else      
    /* send pressure readings periodically */
    Ble_ProcessCommands(CMD_GET_PRESS, NULL_PTR);
#endif
}

/*
*|----------------------------------------------------------------------------
*|  Routine: Ble_StartPeriodicNotify
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Ble_StartPeriodicNotify(void)
{
    #define TIMER_PERIOD      1000          /**< Timer period (msec) */
    /* Start timer for LED1 blinking */
    TimerHandle_t notify_timer_handle; 
    notify_timer_handle =xTimerCreate( "BLENotify", TIMER_PERIOD, pdTRUE, NULL, Ble_Notify_Timer_Callback);
    xTimerStart(notify_timer_handle, 0);
}


/*
*|----------------------------------------------------------------------------
*|  Routine: BluetoothSendString
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Ble_SendString(char *pBuf)
{
    SciTxPacket(SCI_BLUETOOTH_COM, strlen(pBuf), pBuf);      
}


/*
*|----------------------------------------------------------------------------
*|  Routine: Ble_ProcessCommands
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Ble_ProcessCommands
(
    int cmd,
    char *pRxBuf  /* pointer to the receive buffer */      
)
{          
    RTC_TimeTypeDef RtcTimeStruct;  
    RTC_DateTypeDef RtcDateStruct; 

    AMC_CONFIG_STRUCT amcConfig;
    AMC_SETUP_STRUCT amcSetup;
       
    press_sensor_data_t pressSensorData[8];
    
    UINT16 pressSensorPsi[8];

    UINT8 valveNbr =0;
    UINT8 boardId =0;
    UINT16 nbrTxBytes =0;
    
    //ErrorStatus errStatus =SUCCESS;
    
    /* skip over packet number, only needed for messages that are receiving 
       data from PC */
    //pRxBuf +=2;
    
    memset( BleSerialTxBuffer, 0x00, sizeof(BleSerialTxBuffer) );
    
    switch(cmd)
    {
        case CMD_GW_RESET:
            NVIC_SystemReset();
            break;
        case CMD_GW_GET_FW_VERSION:
            strcpy(BleSerialTxBuffer, FW_VERSION);
            strcat(BleSerialTxBuffer, __DATE__);
         //   strcat(SerialTxBuffer, __TIME__);
            
            nbrTxBytes =strlen(BleSerialTxBuffer);
            break;
        case CMD_GW_GET_RTC:
            memset(&RtcTimeStruct, 0x00, sizeof(RtcTimeStruct));
            RtcGetTime( RTC_Format_BIN, &RtcTimeStruct);
          
            memset(&RtcDateStruct, 0x00, sizeof(RtcDateStruct));
            RtcGetDate( RTC_Format_BIN, &RtcDateStruct);  
            
            memcpy(BleSerialTxBuffer, &RtcTimeStruct, sizeof(RtcTimeStruct));
            memcpy(&BleSerialTxBuffer[sizeof(RtcTimeStruct)], &RtcDateStruct, sizeof(RtcDateStruct));  
            
            nbrTxBytes =sizeof(RtcTimeStruct) +sizeof(RtcTimeStruct);           
            break;
        case CMD_GW_SET_RTC:          
            /* set the date */
            RtcDateStruct.RTC_WeekDay =0; /* make sure this is 0 */
            
            RtcDateStruct.RTC_Year  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcDateStruct.RTC_Year += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcDateStruct.RTC_Month  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcDateStruct.RTC_Month += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcDateStruct.RTC_Date  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcDateStruct.RTC_Date += (*pRxBuf &0x0f);    *pRxBuf++;            
 
            //errStatus =RtcSetDate(RTC_Format_BIN, &RtcDateStruct);            

            /* read back */
            memset(&RtcDateStruct, 0x00, sizeof(RtcDateStruct));
            RTC_GetDate( RTC_Format_BIN, &RtcDateStruct);  
            
            /* set the time */
            RtcTimeStruct.RTC_Hours  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcTimeStruct.RTC_Hours += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcTimeStruct.RTC_Minutes  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcTimeStruct.RTC_Minutes += (*pRxBuf &0x0f);    *pRxBuf++;
            
            RtcTimeStruct.RTC_Seconds  = (*pRxBuf &0x0f)*10; *pRxBuf++;
            RtcTimeStruct.RTC_Seconds += (*pRxBuf &0x0f);    *pRxBuf++; 
            
            RtcSetTime(RTC_Format_BIN, &RtcTimeStruct);                                   
            break;            
        case CMD_GW_GET_CONFIG:
            AmcGetConfig(&amcConfig);
                         
            memcpy(BleSerialTxBuffer, &amcConfig, sizeof(amcConfig));
            
            nbrTxBytes =sizeof(amcConfig);            
            break;
        case CMD_GW_SET_CONFIG:
            memcpy(&amcConfig, pRxBuf, sizeof(amcConfig));
            break;              
        case CMD_GW_GET_SETUP:
            AmcGetSetup(&amcSetup);
                         
            memcpy(BleSerialTxBuffer, &amcSetup, sizeof(amcSetup));
            
            nbrTxBytes =sizeof(amcSetup);
            break;
        case CMD_GW_SET_SETUP:
            memcpy(&amcSetup, pRxBuf, sizeof(amcSetup));                   
            break;
        case CMD_GW_GET_ANALOG:
            break;   
        case CMD_OPEN_VALUE:
            valveNbr  =*pRxBuf++;
            
            for(int j=0;j<8; j++)
            {
                if( valveNbr&(0x01<<j) )
                    OpenValve(j+1);
            }
            break;
        case CMD_CLOSE_VALUE:
            valveNbr  =*pRxBuf++;

            for(int j=0;j<8; j++)
            {
                if( valveNbr& (0x01<<j) )
                    CloseValve(j+1);
            }                       
            break;            
        case CMD_GET_PRESS_TEMP:            
            PressureTdr_GetPressTemp(pressSensorData);
            
            memcpy(BleSerialTxBuffer, &pressSensorData, sizeof(BleSerialTxBuffer));            
            nbrTxBytes =sizeof(pressSensorData);         
            break;
        case CMD_GET_PRESS:
            PressureTdr_GetPressTemp(pressSensorData);
      
            for(int j=0; j<NBR_TRANSDUCERS; j++)
            {
                pressSensorPsi[j] =(UINT16)(pressSensorData[j].press/0.00689476);
            }
            
            memcpy(BleSerialTxBuffer, &pressSensorPsi, sizeof(BleSerialTxBuffer));            
            nbrTxBytes =sizeof(pressSensorPsi);     
            break;            
        case CMD_GET_BRD_ID:
            boardId =BOARD_ID;

            memcpy(BleSerialTxBuffer, &boardId, sizeof(boardId));            
            nbrTxBytes =sizeof(boardId);                
            break;
        default:            
            cmd =0x7fff;
            break;
    }  
    
    /* use NACK to indicate command received, but request failed */ 
    cmd |=ACK;
    
    /* send a packet */
    SciSendPacket(SCI_BLUETOOTH_COM, cmd, nbrTxBytes, BleSerialTxBuffer);     
} 


/*
*|----------------------------------------------------------------------------
*|  Routine: Ble_Machine
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL Ble_Machine(void)
{  
    TickType_t xTicks=xTaskGetTickCount();
    
    static UINT8 msgNbr =0;
    
    switch( BleData.machState )
    {
        case BLUETOOTH_INIT_STATE:
            //GPIO_ResetBits(BRD_ID_BIT1_PORT, BRD_ID_BIT1_PIN);
            TimerDelayUs(500000);
            GPIO_SetBits(BRD_ID_BIT1_PORT, BRD_ID_BIT1_PIN);
            
            msgNbr =0;
                                    
            BleData.timer =xTicks;            
            BleData.machState =BLUETOOTH_WAIT_RESPONSE_STATE;
            break;
        case BLUETOOTH_WAIT_RESPONSE_STATE:
            if( (xTicks -BleData.timer ) >1000 )
            {
                msgNbr ++;
                
                if( msgNbr ==1 )
                    Ble_SendString("$$$");  //enter command mode
                else if( msgNbr ==2 )
                    Ble_SendString("SS,C0\r\n"); // enable device information and UART transparent service
                else if( msgNbr ==3 )
                    Ble_SendString("S-,SF\r\n"); // enable device information and UART transparent service                
                else if(msgNbr ==4 )
                {
                    Ble_SendString("R,1\r\n"); // reboot the module                  
                    
                    /* done */
                    BleData.machState =BLUETOOTH_IDLE_STATE;
                }
                
                BleData.timer =xTicks;       
            }
            break;
        case BLUETOOTH_IDLE_STATE:
//            Ble_SendString("$$$");  //enter command mode
#if 0
//Ble_SendString("+\r\n"); //echo on
Ble_SendString("SS,C0\r\n"); // enable device information and UART transparent service

Ble_SendString("R,1\r\n"); // reboot the module
//Ble_SendString("D\r\n"); // display settings (ensure service is no C0)

//Ble_SendString("F\r\n"); // initiate scan
//Ble_SendString("C,<0,1><MAC address>\r\n"); 

/* kill connection */
//Ble_SendString("$$$");
//Ble_SendString("K,1\r\n");
#endif
            BleData.machState =BLUETOOTH_START_NOTIFY_STATE;
            break;       
        case BLUETOOTH_START_NOTIFY_STATE:
            Ble_StartPeriodicNotify();
            BleData.machState =BLUETOOTH_WAIT_FOR_CLIENT_STATE;
            break;          
        case BLUETOOTH_WAIT_FOR_CLIENT_STATE:
            /* GATT Client will subscribe to our notify characteristic */
            break;      
    }
    
    return TRUE;
}
