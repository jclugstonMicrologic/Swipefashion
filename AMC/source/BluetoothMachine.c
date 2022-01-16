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
* Copyright (c) 2021, MICROLOGIC
* Calgary, Alberta, Canada, www.micrologic.ab.ca
*******************************************************************************/


/** Include Files *************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "stdio.h"

#include "rtcHi.h"
#include "solenoidHi.h" 
#include "gpioHi.h"

#include "SciBinaryMachine.h"
#include "SciAsciiMachine.h"

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
    BLUETOOTH_CONNECTED_STATE,
    
    BLUETOOTH_LAST_STATE
      
}BluetoothStatesTypeEnum;


typedef struct
{
    BluetoothStatesTypeEnum machState;
    BluetoothStatesTypeEnum prevMachState;
        
    UINT32 timer;
    UINT32 waitTimer;    
          
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
#if 0 
    //uint16_t length =0;
    char data_array[16];

static uint16_t var=300;
var -=1;

    sprintf(data_array, "count: %d", var);
    //length =strlen(data_array);
    
    Ble_SendString(data_array); 
#else      
    /* send pressure readings periodically */
    //Ble_ProcessCommands(CMD_GET_PRESS_TEMP, NULL_PTR);
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
    UINT8 comprNbr =0;
    
    UINT16 nbrTxBytes =0;

    UINT32 FlashPgmAddress =0;
    
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
        case CMD_OPEN_VALVE:
            valveNbr  =*pRxBuf++;
            
            for(int j=0;j<NBR_VALVES; j++)
            {
                if( valveNbr&(0x01<<j) )
                    Solenoid_OpenValve(j+1);
            }
            break;
        case CMD_CLOSE_VALVE:
            valveNbr  =*pRxBuf++;

            for(int j=0;j<NBR_VALVES; j++)
            {
                if( valveNbr& (0x01<<j) )
                    Solenoid_CloseValve(j+1);
            }                       
            break;
        case CMD_SET_VALVE:
            /* open and close all valves */
            valveNbr  =*pRxBuf++;
            
            for(int j=0;j<NBR_VALVES; j++)
            {
                if( valveNbr&(0x01<<j) )
                    Solenoid_OpenValve(j+1);
                else
                    Solenoid_CloseValve(j+1);
            }
            break; 
        case CMD_START_COMR:
            comprNbr  =*pRxBuf++;
            
            Solenoid_StartCompr(comprNbr);
            break;
        case CMD_STOP_COMR:
            comprNbr  =*pRxBuf++;
            
            Solenoid_StopCompr(comprNbr);
            break;            
        case CMD_SET_COMR:                      
            /* turn on/off the compresssor */
            comprNbr  =*pRxBuf++;
            
            Solenoid_SetCompr( comprNbr );
            break;
        case CMD_GET_PRESS_TEMP:            
            /* pressure and temperature, and they are floats, 
               large packet
            */           
            PressureTdr_GetPressTemp(pressSensorData);
            
            memcpy(BleSerialTxBuffer, &pressSensorData, sizeof(pressSensorData));            
            nbrTxBytes =sizeof(pressSensorData);         
            break;
        case CMD_GET_PRESS:
            /* just pressure, and UINT16 instead of float, much smaller packet */
            PressureTdr_GetPressTemp(pressSensorData);
      
            for(int j=0; j<NBR_TRANSDUCERS; j++)
            {
                if( pressSensorData[j].press >=0 )
                    pressSensorPsi[j] =(UINT16)(pressSensorData[j].press/0.00689476);
                else
                    pressSensorPsi[j] =0;
            }
            
            /* send the board status along with this message */
            memcpy(BleSerialTxBuffer, &BoardStatus, sizeof(BoardStatus));            
            nbrTxBytes =sizeof(BoardStatus);     
            
            memcpy(&BleSerialTxBuffer[nbrTxBytes], &pressSensorPsi, sizeof(pressSensorPsi));            
            nbrTxBytes +=sizeof(pressSensorPsi);     
            break;            
        case CMD_GET_BRD_ID:
            memcpy(BleSerialTxBuffer, &BoardStatus, sizeof(BoardStatus));            
            nbrTxBytes =sizeof(BoardStatus);                
            break;
        case CMD_ERASE_FLASH:                      
            dfu_EraseFlash();
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
*|  Routine: Ble_StateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void Ble_StateProcess
(
    ble_t *pStructInfo,
    BluetoothStatesTypeEnum nextState
)
{
    pStructInfo->prevMachState =pStructInfo->machState;
    pStructInfo->machState =nextState;
    
    pStructInfo->timer =xTaskGetTickCount();
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
    BOOL msgPass =FALSE;
    
    switch( BleData.machState )
    {
        case BLUETOOTH_INIT_STATE:
            /* dual purpose pin, was input on startup, now needs to be output */
            GpioSetOutput();  

            BLE_RESET_NEGATE;
            TimerDelayUs(500000);
            BLE_RESET_ASSERT;
            
            msgNbr =1;
               
            BleData.waitTimer =xTicks;
            
            Ble_StateProcess(&BleData,BLUETOOTH_WAIT_RESPONSE_STATE);
            break;
        case BLUETOOTH_WAIT_RESPONSE_STATE:
            if( (xTicks -BleData.waitTimer ) >500 )
            {                
                if( msgNbr ==1 && Sci_GetAsciiString(0, "%REBOOT") ==1 )
                {
                    msgPass =TRUE;
                    Ble_SendString("$$$");  //enter command mode
                }
                else if( msgNbr ==2 && Sci_GetAsciiString(0, "CMD>") ==1 )
                {
                    msgPass =TRUE;
                    Ble_SendString("SS,C0\r\n"); // enable device information and UART transparent service
                }
                else if( msgNbr ==3 && Sci_GetAsciiString(0, "AOK") ==1 )
                {
                    msgPass =TRUE;
                    Ble_SendString("S-,SwipeFashion\r\n"); // enable device information and UART transparent service                
                }
                else if(msgNbr ==4 && Sci_GetAsciiString(0, "CMD>") ==1 )
                {
                    msgPass =TRUE;
                    Ble_SendString("R,1\r\n"); // reboot the module                                      
                }
                else if(msgNbr ==5 && Sci_GetAsciiString(0, "%REBOOT") ==1 )
                {                
                    /* done */
                    SciAsciiSendString(SCI_PC_COM, "BLE INIT PASS\r\n");
                    msgPass =TRUE;
                    Ble_StateProcess(&BleData,BLUETOOTH_START_NOTIFY_STATE);;                  
                }
                
                if(msgPass)
                {                  
                    msgPass =FALSE;
                    msgNbr++;
                    
                    BleData.waitTimer =xTicks;    
                }
                
                if( (xTicks -BleData.waitTimer ) >5000 )
                {
                    SciAsciiSendString(SCI_PC_COM, "ERROR BLE INIT FAIL\r\n");
                    Ble_StateProcess(&BleData,BLUETOOTH_INIT_STATE);
                    
                    BleData.waitTimer =xTicks;       
                }
            }
            break;
        case BLUETOOTH_IDLE_STATE:
            //BleData.machState =BLUETOOTH_START_NOTIFY_STATE;
            break;       
        case BLUETOOTH_START_NOTIFY_STATE:
            Ble_StartPeriodicNotify();
            Ble_StateProcess(&BleData,BLUETOOTH_WAIT_FOR_CLIENT_STATE);
            break;          
        case BLUETOOTH_WAIT_FOR_CLIENT_STATE:
            /* GATT Client will subscribe to our notify characteristic */
            if( Sci_GetAsciiString(0, "%CONNECT") ==1 )
            {
                Ble_StateProcess(&BleData,BLUETOOTH_CONNECTED_STATE);
            }
            break;      
        case BLUETOOTH_CONNECTED_STATE:
            if( Sci_GetAsciiString(0, "%DISCONNECT") ==1 )
            {
                /* lost connection with PC, close all valves for safety */
                SciAsciiSendString(SCI_PC_COM, "BLE DISCONNECT\r\n");
                
                Solenoid_CloseAllValves();               
                /* stop the pumps if this is the pump controller card */
                Solenoid_StopCompr( (COMPR_LO | COMPR_HI) );
                Solenoid_StopCompr(0x00);
                
                Ble_StateProcess(&BleData,BLUETOOTH_WAIT_FOR_CLIENT_STATE);
            }           
            break;
    }
    
    return TRUE;
}
