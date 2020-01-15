/** C Source ******************************************************************
*
* NAME      CellularMachine.c
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
* Copyright (c) 2019, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
******************************************************************************/


/** Include Files *************************************************************/

#include "GatewayConfig.h"

#include "PowerManagement.h"
#include "CellularMachine.h"
#include "ServerPacket.h"

#include "WsMachine.h"

#include "FreeRTOS.h"
#include "task.h"

#include "timers.h"

#include "sysTimers.h"

#include "rtcHi.h"
#include "gpioHi.h"

#include "stdio.h"

#include "Transport.h"

#include "WSClient.h"

//#define DEBUG_CELL_COMMS
//#define TEST_GPRS

#define FW_FILENAME "Gateway_v0.04.hex" 

/* all timeouts below are in msec */
#define CELL_POWER_UP_DELAY      5000
#define CELL_INIT_DELAY          15000

/* max time to wait for cell module to reply */
#define CELL_RESPONSE_TIMEOUT    15000

/* how long to wait before trying again if cell connection procedure failed */
#define WAIT_TRY_AGAIN_TIMEOUT   5000   

/* max time wait for server acknowledgement */
#define WAIT_SERVER_ACK_TIMEOUT  10000

/* how long to stay in failure power down mode (server connection could not be established or other continuous cellular errors */ 
#define POWER_DOWN_TIMEOUT      3600000

#define CELL_RSSI_UNKNOWN_UNDETECTABLE 99

typedef enum
{
    CELL_INIT_GSM_START =0,
    CELL_INIT_GSM_RESET,
    
    CELL_INIT_GSM_COMPLETE =100
      
}CellInitGsmStayepEnum;

typedef enum
{
    CELL_INIT_GPRS_START =0,    
     
    CELL_INIT_GPRS_GET_RSSI =6,
    
    CELL_INIT_GPRS_COMPLETE =100
      
}CellInitGprsStatesTyepEnum;

typedef enum
{
    CELL_INIT_STATE =0,
    CELL_IDLE_STATE,    
    CELL_WAIT_TX_STATE,
    CELL_TURN_ON_STATE,    
    CELL_INIT_GSM_STATE,
    CELL_INIT_GPRS_STATE,  
    CELL_WAIT_CELL_ACK_STATE,      
    CELL_READY_STATE,    
    CELL_FAIL_STATE,
    CELL_DISABLE_STATE,
    CELL_RSSI_WAIT_STATE,
    CELL_POWER_DOWN_STATE,
    CELL_POWER_DOWN_STATE2,
    CELL_WAIT_SERVER_ACK,
      
    CELL_LAST_STATE
      
}CellStatesTypeEnum;

enum
{
    FTP_ENTER_CMD_MODE =0,    
    FTP_CFG_STATE,    
    FTP_OPEN_STATE,
    FTP_CD_STATE,
    FTP_SET_TYPE_STATE,
    FTP_PUT_FILE_STATE,
    FTP_GET_FILE_STATE,

    FTP_GET_FILE_SIZE_STATE,
    
    FTP_FTPS_CFG_STATE,
    FTP_SECDATA_STATE,
    FTP_SAVE_CERT_STATE,
    
    FTP_COMPLETE_STATE,
      
    FTP_LAST_STATE
    
};

typedef struct
{
    CellStatesTypeEnum machState;
    CellStatesTypeEnum prevMachState;
    
    UINT32 timer;
    UINT32 sleepTimer;
          
}CELLULAR_STRUCT;


typedef struct
{
    UINT8 prevState;
    UINT8 machState;
    UINT32 timeoutTimer;
}STATE_INFO;


typedef struct
{
    UINT8 rssi;
    UINT8 simPresent;
    UINT8 socketConnect;
    UINT8 type;	    /* LE910, HE910, GSM865, etc */
    UINT8 regStatus;
    
    UINT32 simNbr;  /* last 5 digits */
    
    UINT8 phNbr[12];

}CELLULAR_RADIO_PARAMETERS;

typedef struct
{       
    BOOL ack;
    UINT8 reply;
    
}CELL_RESPONSE;


typedef struct
{       
    UINT32 bytesDownloaded;
    UINT32 fileSize;
    
}CELL_FTP_DATA;

CELLULAR_RADIO_PARAMETERS CellRadioParameters;

CELLULAR_STRUCT CellData;

CELL_RESPONSE CellResponse;

CELL_FTP_DATA CellFtpData;

UINT8 CellTurnonState =0;
UINT8 CellInitModuleState =0;
UINT8 CellInitGprsState =0;
UINT8 RssiState =0;

UINT8 FtpSocketState =0;

char FtpRecvArray[256];

void CellularProcessCommands(int cmd,char *pCcmd);
BOOL CellularSmsMessageTest( UINT8 *pSmsState );

void SaveFtpsCertificate(void);
  
/** Functions *****************************************************************/


/*
*|------------------------------------------------------------------------------
*|  Routine: CellularTurnOn
*|  Description: power up the cellular module
*|  Retval: bool
*|------------------------------------------------------------------------------
*/
BOOL CellularTurnOn(void)
{   
    static UINT32 cellTimer =0;
    
    BOOL status =FALSE;
    
    TickType_t xTicks;                
    
    xTicks=xTaskGetTickCount();
   
    switch( CellTurnonState )
    {
        case 0:
            PowerDownPeripheral(PWR_CELL_MODULE);
            
            if( !GPIO_ReadInputDataBit(CELL_PWR_MON_PORT, CELL_PWR_MON_PIN) )
            {
                CellTurnonState =1;
            }
            break;
        case 1:          
        //    PowerUpPeripheral(PWR_CELL_MODULE);
            
            if( GPIO_ReadInputDataBit(CELL_PWR_MON_PORT, CELL_PWR_MON_PIN) )
            {
                PowerDownPeripheral(PWR_CELL_MODULE);
                cellTimer =xTicks;
             
                CellTurnonState =2;
            }
            else
            {                              
                PowerUpPeripheral(PWR_CELL_MODULE);                
        
                CELL_OFF;       
                
                cellTimer =xTicks;
                
                CellTurnonState =3;
            }
            break;
        case 2:
            if( (xTicks -cellTimer) >10 )
            {               
                CellTurnonState =0;
            }
            break;
        case 3:
            /* wait 5 seconds before proceeding */
            if( xTicks -cellTimer > CELL_POWER_UP_DELAY )
            {
                CELL_ON;
                               
                TimerDelayUs(100000);
                                
                if( GPIO_ReadInputDataBit(CELL_PWR_MON_PORT, CELL_PWR_MON_PIN) )
                {
                    cellTimer =xTicks;
                
                    CellTurnonState =4;
                }
                else
                    CellTurnonState =0;
            }                   
            break;
        case 4:
            /* wait 15 seconds before proceeding, OR move on +PACSP1 response */
            if( xTicks -cellTimer >CELL_INIT_DELAY //||
                //CellResponse.reply ==CELL_REPLY_OK
              )
            {               
                status =TRUE;
                
                CellTurnonState =0;
            }
            break;
              
    } 
    
    return status;
}

/*
*|------------------------------------------------------------------------------
*|  Routine: CellularInitGsm
*|  Description: Init GSM 
*|  Retval:
*|------------------------------------------------------------------------------
*/
UINT8 CellularInitGsm()
{   
    char *pCmd =NULL_PTR;
                     
    switch( CellInitModuleState )
    {
        case CELL_INIT_GSM_START:
            //pCmd ="ATZ1\r\n";
            //pCmd ="AT&F0\r\n";
            pCmd ="AT\r\n";
            CellInitModuleState ++;
            break;
        case 1:
            pCmd ="AT+GMM\r\n";
           // pCmd ="AT+CGMR\r\n"; // get device software revision number (20.00.522, latest is 20.00.525)
            CellInitModuleState ++;
            break;            
        case 2:
            pCmd ="AT#SELINT=2\r\n";
            CellInitModuleState =4;
            break;                        
        case 3:
            pCmd ="AT#STIA=2,10\r\n";
            CellInitModuleState ++;
            break;              
        case 4:
            pCmd ="AT+COPS=0\r\n";
            CellInitModuleState ++;
            break;          
        case 5:
            pCmd ="AT#AUTOBND=2\r\n"; // enable automatic band selection in four bands (at 850/1900 and 900/1800);            
            CellInitModuleState =7;
            break;              
        case 6:
            pCmd ="AT#PLMNMODE=0\r\n";
            CellInitModuleState ++;
            break;              
        case 7:
            pCmd ="AT&P0\r\n"; // save profile
            CellInitModuleState ++;
            break;      
        case 8:
            pCmd ="AT&K3\r\n";  // hardware bi-directional flow control (both RTS/CTS active)
            CellInitModuleState ++;
            break;      
        case 9:
            pCmd ="AT&W0\r\n"; // save settings
            CellInitModuleState ++;
            break;              
        case 10:
            /* set the maximum time that the module awaits before sending
               a packet whose size is less than the default one 
            */
            //pCmd ="AT#DSTO=5\r\n";
          
            /* set the default packet size to be used by the TCP/UDP/IP stack
               for data sending
            */
            pCmd ="AT#PKTSZ=32\r\n"; //32 bytes (default is 300)
            CellInitModuleState ++;
            break;
        case 11:
            //pCmd ="AT+IPR=?\r\n"; 
            pCmd ="AT+IPR=921600\r\n"; /* switch to higher baudrate */
                        
            CellInitModuleState ++;
            break;            
        case 12:
            SciAsciiReceiverInit(SCI_CELLULAR_COM, 921600, CellularProcessCommands, NULL_PTR);
          
            CellInitModuleState =CELL_INIT_GSM_COMPLETE;
            break;
      
    }

    if( pCmd !=NULL_PTR )
    {
        SciTxPacket(SCI_CELLULAR_COM, strlen(pCmd), pCmd);      
    }
    
    return CellInitModuleState;    
}


/*
*|------------------------------------------------------------------------------
*|  Routine: CellularInitGprs
*|  Description: Init GPRS 
*|  Retval:
*|------------------------------------------------------------------------------
*/
UINT8 CellularInitGprs()
{  
    char *pCmd =NULL;
    char tempBuf[128];
    
    GW_CONFIG_STRUCT gwConfig;   
    
    switch( CellInitGprsState )
    {
        case CELL_INIT_GPRS_START:
            pCmd ="AT+CMEE=2\r\n"; // enable error responses           
            CellInitGprsState ++;
            break;
        case 1:
            pCmd ="AT+CPIN?\r\n"; // check for SIM
            CellInitGprsState ++;
            break;            
        case 2:
            pCmd ="AT+CGSN\r\n";  // get IMEI number
            CellInitGprsState ++;
            break;          
        case 3:
            pCmd ="AT+CNUM\r\n"; // get 10 digit phone number
            CellInitGprsState ++;
            break;              
        case 4:
            pCmd ="AT+COPS?\r\n";
            CellInitGprsState ++;
            break;              
        case 5:
            pCmd ="AT+CSQ\r\n";   // get RSSI, do not proceed if too low              
            CellInitGprsState ++;
            break;                                    
        case CELL_INIT_GPRS_GET_RSSI:
            /* rssi value received, if 99 try again */
            if( CellRadioParameters.rssi ==CELL_RSSI_UNKNOWN_UNDETECTABLE )
            { 
                /* RSSI no go, resend */
                pCmd ="AT+CSQ\r\n";   
            }
            else
            {
                pCmd ="AT#CCID\r\n";  // get ICCID number (SIM number)                
                CellInitGprsState ++;
            }           
            break;                          
        case 7:
            pCmd ="AT#SERVINFO\r\n";
            CellInitGprsState =9;
            break;                  
        case 8:
            pCmd ="AT+FLO=2\r\n"; // enable flow control (not working returns ERROR???)
            CellInitGprsState ++;
            break;                  
        case 9:
            GwGetConfig(&gwConfig);
                
            /* build message with apn setting */
            strcpy(tempBuf, "AT+CGDCONT=1,\"IP\",\"");
            strcat(tempBuf, gwConfig.network.apn);
            strcat(tempBuf, "\",\"0.0.0.0\",0,0\r\n");
                   
            pCmd =tempBuf;
            
            //pCmd ="AT+CGDCONT=1,\"IP\",\"m2minternet.apn\",\"0.0.0.0\",0,0\r\n";            
            CellInitGprsState ++;
            break;      
        case 10:
            if( CellRadioParameters.regStatus ==1 || /* registered, home network */
                CellRadioParameters.regStatus ==5    /* registered, roaming */
              )
            {
                pCmd ="AT+CGQMIN=1,0,0,0,0,0\r\n";                
                CellInitGprsState ++;     
            }
            else
            {
                pCmd ="AT+CREG?\r\n"; // get registration
            }
            break;              
        case 11:
            pCmd ="AT+CGQREQ=1,0,0,3,0,0\r\n";
            CellInitGprsState =14;//++;
            break;                        
        case 12:
            pCmd ="AT+CGATT=?\r\n";	            
            CellInitGprsState ++;
            break;
        case 13:
            pCmd ="AT+CGATT=1\r\n";	
            CellInitGprsState ++;
            break;           
        case 14:
            pCmd ="AT#SGACT=1,0,\"\",\"\"\r\n";	//deactivate PDP                                    
                        
            CellInitGprsState ++;
            break;
        case 15:
            pCmd ="AT#SGACT=1,1,\"\",\"\"\r\n";	//activate PDP                              
                                 
            CellInitGprsState ++;
            break;            
        case 16:
            pCmd ="AT+CGREG?\r\n";
            CellInitGprsState ++;
            break;
        case 17: 
            pCmd ="AT#CIMI\r\n"; // get IMSI number
            
            /* skip to state 17 if you want to send an SMS message instead */
            CellInitGprsState =33;//++;
            break;            
        case 18:
            /* <connId>,<txProt(TCP or UDP)><rPort><IPAddr><closureType>
               <IPort(UDP)><connMode> 
                <connMode> - 0 online mode connection (data mode)
                <connMode> - 1 command mode connection
            */            
            GwGetConfig(&gwConfig);
                       
        #if 0
            /* TRIG */
            gwConfig.network.ipAddr[0] =184;
            gwConfig.network.ipAddr[1] =64;
            gwConfig.network.ipAddr[2] =221;
            gwConfig.network.ipAddr[3] =28;

            /* my house*/            
            gwConfig.network.serverPort =55000;
            gwConfig.network.ipAddr[0] =184;
            gwConfig.network.ipAddr[1] =64;
            gwConfig.network.ipAddr[2] =106;
            gwConfig.network.ipAddr[3] =49;
           
            /* Thingsboard MQTT server */
            gwConfig.network.serverPort =1883;
            gwConfig.network.ipAddr[0] =34;
            gwConfig.network.ipAddr[1] =221;
            gwConfig.network.ipAddr[2] =203;
            gwConfig.network.ipAddr[3] =201;     
            
            /* Websocket server, internal */
            gwConfig.network.serverPort =8080;
            gwConfig.network.ipAddr[0] =192;
            gwConfig.network.ipAddr[1] =168;
            gwConfig.network.ipAddr[2] =20;
            gwConfig.network.ipAddr[3] =120;                          
        #endif           
                              
            sprintf(tempBuf, "AT#SD=1,0,%d,\"%d.%d.%d.%d\",0,0,0\r\n",gwConfig.network.serverPort,
                                                                      gwConfig.network.ipAddr[0],
                                                                      gwConfig.network.ipAddr[1],
                                                                      gwConfig.network.ipAddr[2],
                                                                      gwConfig.network.ipAddr[3]);            
                                    
            //sprintf(tempBuf, "AT#SD=1,0,%d,\"159.89.117.98\",0,0,0\r\n", 990);
            //sprintf(tempBuf, "AT#SD=1,0,%d,\"ws://trig-tload.herokuapp.com\",0,0,0\r\n", 80);
            //sprintf(tempBuf, "AT#SD=1,0,%d,\"ws://echo.websocket.org\",0,0,0\r\n", 80);
            //sprintf(tempBuf, "AT#SD=1,0,%d,\"m2m.eclipse.org\",0,0,0\r\n", 1883);                                    
            //sprintf(tempBuf, "AT#SD=1,0,%d,\"54.218.82.76\",0,0,0\r\n", 1883);
                                
            pCmd =tempBuf; 
                       
            //pCmd ="AT#SD=1,0,8888,\"184.64.221.30\",0,0,0\r\n";                        
            //pCmd ="AT#HTTPCFG=1,\"www.google.com\",80,0,0,0,0,0,0\r\n";
            //pCmd ="AT#SD=2,0,80,\"www.google.com\",0,0\r\n";
            CellInitGprsState ++;           
            break;               
        case 19:
            /* socket connection successful */
            CellInitGprsState =CELL_INIT_GPRS_COMPLETE;
            break;            
        case 33:
            /* get the network time from an NTP server */
            //pCmd ="AT#NTP=\"time.windows.com\",123,1,10,0\r\n";
            pCmd ="AT#NTP=\"time.google.com\",123,1,10,0\r\n";
            CellInitGprsState =18;
            break;            
    }
        
    if( pCmd !=NULL_PTR )
    {
        SciTxPacket(SCI_CELLULAR_COM, strlen(pCmd), pCmd);      
    }
    
    return CellInitGprsState;    
}


/*
*|------------------------------------------------------------------------------
*|  Routine: CellularTestGprs
*|  Description: Test GPRS
*|  Retval:
*|------------------------------------------------------------------------------
*/
UINT8 CellularTestGprs(void)
{
    static UINT8 testGprsState =0;
    char *pCmd =NULL;
        
    switch( testGprsState)
    {
        case 0:  
            pCmd ="AT+CMEE=2\r\n"; // enable error responses           
            testGprsState ++;
            break;
        case 1:  
            pCmd ="AT#CEERNETEXT=1\r\n"; // enable error responses           
            testGprsState ++;
            break;            
        case 2:  
            pCmd ="AT+CGMM\r\n"; // enable error responses           
            testGprsState ++;
            break;            
        case 3:  
            pCmd ="AT+CGMR\r\n"; // enable error responses           
            testGprsState ++;
            break;            
        case 4:  
            pCmd ="AT+CGSN\r\n"; // enable error responses           
            testGprsState =90;
            break;                       
        case 90:  
            pCmd ="AT#FWSWITCH?\r\n";          
            testGprsState ++;
            break;       
        case 91:  
            pCmd ="AT#FWSWITCH=?\r\n";          
            testGprsState =5;
            break;                               
        case 5:  
            pCmd ="AT+CREG?\r\n"; // enable error responses           
            testGprsState ++;
            break;            
        case 6:  
            pCmd ="AT+COPS?\r\n"; // enable error responses           
            testGprsState ++;
            break;                        
        case 7:  
            pCmd ="AT#RFSTS\r\n";
            testGprsState ++;
            break;                                    
        case 8:  
            pCmd ="AT+CGCONTRDP=1\r\n";
            testGprsState ++;
            break;                  
        case 9: 
            pCmd ="AT+CGDCONT?\r\n";
            testGprsState ++;
            break;   
        case 10:  
            pCmd ="AT#SGACT=1,1\r\n";
            testGprsState ++;
            break;   
        case 11:  
            pCmd ="AT+CEER\r\n";
            testGprsState ++;
            break;   
        case 12:  
            pCmd ="AT#CEER\r\n";
            testGprsState ++;
            break;               
        case 13:  
            pCmd ="AT#CEERNET\r\n";
            testGprsState ++;
            break;                       
    }
    
    if( pCmd !=NULL_PTR )
    {
        SciTxPacket(SCI_CELLULAR_COM, strlen(pCmd), pCmd);      
    }
    
    return testGprsState;    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: CellularInitFtpSocket
*|  Description: Init FTP socket
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
void CellularInitFtpSocket(void)
{
    FtpSocketState =0;
    
    CellFtpData.bytesDownloaded =0;
    CellFtpData.fileSize =0;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: CellularConnectFtpSocket
*|  Description: Establish an FTP socket connection
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
BOOL CellularConnectFtpSocket(UINT8 put)
{
    char *pCmd =NULL;
    char tempBuf[128];
  
    BOOL status  =FALSE;
        
    GW_CONFIG_STRUCT gwConfig;   

    char fwFilename[20];
    
//#define FTPS_SERVER
    
    switch(FtpSocketState)
    {
        case FTP_ENTER_CMD_MODE:
            strcpy(tempBuf,"+++");            
            pCmd =tempBuf;            
            FtpSocketState =FTP_CFG_STATE;
            break;
        case FTP_CFG_STATE:
        #ifdef FTPS_SERVER // secure TRIG server  
            strcpy(tempBuf,"AT#FTPCFG=100,0,1\r\n"); // choose FTPEn (0) or FTPSEn (1)            
            
            pCmd =tempBuf;
            
            FtpSocketState =FTP_FTPS_CFG_STATE;             
        #else // FTP server          
            strcpy(tempBuf,"AT#FTPCFG=100,0,0\r\n"); // choose FTPEn (0) or FTPSEn (1)
            
            pCmd =tempBuf;
            
            FtpSocketState =FTP_OPEN_STATE; //FTP_FTPS_CFG_STATE;                         
        #endif    
            break;  
        case FTP_FTPS_CFG_STATE:
            strcpy(tempBuf,"AT#SSLSECCFG=1,3,1\r\n");
            
            pCmd =tempBuf;            
            
            FtpSocketState =FTP_OPEN_STATE; //FTP_SECDATA_STATE;
            break;           
        case FTP_SECDATA_STATE:
            strcpy(tempBuf,"AT#SSLSECDATA=1,1,1,1290\r\n");
            
            pCmd =tempBuf;            
            
            FtpSocketState =FTP_SAVE_CERT_STATE;
            break;   
        case FTP_SAVE_CERT_STATE:            
            SaveFtpsCertificate();
           
            FtpSocketState =FTP_OPEN_STATE;
            break;            
        case FTP_OPEN_STATE:       
        #ifdef FTPS_SERVER 
            strcpy(tempBuf,"AT#FTPOPEN=\"159.89.117.98:990\",\"ftps\",\"transrail\",1\r\n");
        #else
            strcpy(tempBuf,"AT#FTPOPEN=\"ftp.drivehq.com\",\"jclugston\",\"Trig2017\",0\r\n");
        #endif            
            
            pCmd =tempBuf;                        
            
            FtpSocketState =FTP_CD_STATE; //FTP_GET_FILE_SIZE_STATE; FTP_CD_STATE; FTP_SET_TYPE_STATE;
            break;
        case FTP_CD_STATE:                      
            /* change working directory */
            //strcpy(tempBuf,"AT#FTPCWD=\"FwUpdate\"\r\n");
          
            if(put ==0)
                strcpy(tempBuf,"AT#FTPCWD=\"firmware\"\r\n");
            else
                strcpy(tempBuf,"AT#FTPCWD=\"SensorLog\"\r\n");                        
//                strcpy(tempBuf,"AT#FTPCWD=\"MeterData\"\r\n");                        
            
            pCmd =tempBuf;                        
            
            FtpSocketState =FTP_SET_TYPE_STATE;
            break;    
        case FTP_SET_TYPE_STATE:
            strcpy(tempBuf,"AT#FTPTYPE=0\r\n");
           
            pCmd =tempBuf;                        
            
            if( put ==0 )
                FtpSocketState =FTP_GET_FILE_SIZE_STATE;
            else
                FtpSocketState =FTP_PUT_FILE_STATE;
            break;    
        case FTP_PUT_FILE_STATE:
            GwGetConfig(&gwConfig);
            
            /* online mode */
            //sprintf(tempBuf,"AT#FTPPUT=\"metertckt_%d.txt\",0\r\n", (UINT32)RtcGetEpochTime());                                             
            sprintf(tempBuf,"AT#FTPPUT=\"gw%d_sensorlog_%d.txt\",0\r\n", gwConfig.network.gatewayId, (UINT32)RtcGetEpochTime());
            
            /* command mode*/
            //sprintf(tempBuf,"AT#FTPPUT=\"metertckt_%d.txt\",1\r\n", (UINT32)RtcGetEpochTime());  
            
            pCmd =tempBuf;                        
                                     
            FtpSocketState =FTP_COMPLETE_STATE;
            break;                                          
        case FTP_GET_FILE_STATE:
            //strcpy(tempBuf,"AT#FTPGETPKT=\"testfile.txt\",0\r\n");
            
            GetFwFilename(fwFilename);
            
            strcpy(tempBuf, "AT#FTPGETPKT=\"");
            strcat(tempBuf, fwFilename);
            strcat(tempBuf, "\"\r\n");
            
            pCmd =tempBuf;                        
                                     
            FtpSocketState =FTP_COMPLETE_STATE;
            break;    
        case FTP_GET_FILE_SIZE_STATE:
            //strcpy(tempBuf,"AT#FTPLIST\r\n");
            //strcpy(tempBuf,"AT#FTPDELE=\"Gateway.hex\"\r\n");            
            //strcpy(tempBuf,"AT#FTPFSIZE=\"Gateway.hex\"\r\n");

            GetFwFilename(fwFilename);
                      
            strcpy(tempBuf, "AT#FTPFSIZE=\"");
            strcat(tempBuf, fwFilename);
            strcat(tempBuf, "\"\r\n");
            
            pCmd =tempBuf;                        
                                     
            FtpSocketState =FTP_GET_FILE_STATE;          
            break;            
        case FTP_COMPLETE_STATE:
            status =TRUE;
            break;
#if 0            
        case 100:
            strcpy(tempBuf,"AT#SSLEN=1,1\r\n");          
            pCmd =tempBuf;                        
            FtpSocketState =101;
            break;                  
        case 104:
            strcpy(tempBuf,"AT#SSLEN=1,0\r\n");            
            pCmd =tempBuf;                        
            FtpSocketState =105;
            break;                    
#endif            
    }
    
    if( pCmd !=NULL_PTR )
    {
        SciTxPacket(SCI_CELLULAR_COM, strlen(pCmd), pCmd);      
    }
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Routine: CellularSendCmdMode
*|  Description: Switch cell mode to command mode
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
void CellularSendCmdMode(void)
{
    char tempBuf[32];
    
    CellResponse.reply =CELL_REPLY_NONE;
      
    strcpy( tempBuf,"+++"); /* switch to command mode (response is OK) */     
   
    SciTxPacket(SCI_CELLULAR_COM, 3, tempBuf);      
}

/*
*|----------------------------------------------------------------------------
*|  Routine: CellularSocketShutdown
*|  Description: 
*|  Retval: void
*|----------------------------------------------------------------------------
*/
void CellularSocketShutdown(void)
{
    char tempBuf[32];
    
    CellResponse.reply =CELL_REPLY_NONE;
      
    strcpy( tempBuf,"+++"); /* switch to command mode (response is OK) */        
    SciTxPacket(SCI_CELLULAR_COM, 3, tempBuf);      
    
    strcpy( tempBuf,"AT#SH=1\r\n"); /* close the socket */
    SciTxPacket(SCI_CELLULAR_COM, 3, tempBuf);     
}
   

/*
*|----------------------------------------------------------------------------
*|  Routine: CellularMachineInit
*|  Description: Init cell serial port and data structures
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
BOOL CellularMachineInit(void)
{
    if( !SciAsciiReceiverInit(SCI_CELLULAR_COM, SCI_CELLULAR_BAUD_RATE, CellularProcessCommands, NULL_PTR) )
    {
        //!!!
        return FALSE;
    }         
    
    memset( &CellData, 0x00, sizeof(CellData) );
    memset( &CellRadioParameters, 0x00, sizeof(CellRadioParameters) );
    
    //SystemStartup =FALSE;
    
    return TRUE;
}



/*
*|----------------------------------------------------------------------------
*|  Routine: CellularSocketState
*|  Description:
*|  Retval: BOOL
*|----------------------------------------------------------------------------
*/
UINT8 CellularSocketState(void)
{
    if( CellData.machState ==CELL_READY_STATE )
    {
        return 1;
    }
    else if( CellData.machState ==CELL_POWER_DOWN_STATE )
        return 2;
    else
        return 0;    
}

/*
*|----------------------------------------------------------------------------
*|  Routine: CellularCheckTimeout
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
BOOL CellularCheckTimeout
(
    STATE_INFO *stateInfo,
    UINT32 timeout  /* msec */
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
*|  Routine: CellularStateProcess
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void CellularStateProcess
(
    CELLULAR_STRUCT *structInfo,
//    CellStatesTypeEnum prevState,
    CellStatesTypeEnum nextState
)
{
    structInfo->prevMachState =structInfo->machState;//prevState;
    structInfo->machState =nextState;
    
    structInfo->timer =xTaskGetTickCount();
}


/*
*|----------------------------------------------------------------------------
*|  Routine: CellularMachineStart
*|  Description: 
*|  Retval:
*|----------------------------------------------------------------------------
*/
void CellularMachineStart(void)
{
    CellularStateProcess( &CellData, CELL_IDLE_STATE);
}


/*
*|----------------------------------------------------------------------------
*|  Routine: CellularMachine
*|  Description: Main state machine of the cell module
*|  Retval:
*|----------------------------------------------------------------------------
*/
int CellularMachine(void) //void * pvParameters)//UINT8 msg)
{  
    static ServerMsgsTypeEnum GoTcpMsg =SERVER_MSG_NONE;
         
    /* static to take this off the stack */
    //static char tempBuf[256];
    
    static UINT8 GoSMSMsg =0;
            
    static UINT8 initGprsState =0;
    static UINT8 rssiNa =0;
    static UINT8 cellFailCnt =0;

    int status =1;
#if 0
//GoTcpMsg =SERVER_MSG_GPS;
//ServerBuildMessage(tempBuf, GoTcpMsg);

//GoTcpMsg =SERVER_MSG_SENSOR;
//ServerBuildMessage(tempBuf, GoTcpMsg);

GoTcpMsg =SERVER_MSG_METER_TCKT;
ServerBuildMessage(tempBuf, GoTcpMsg);
#endif

#if 0
    if( msg ==SERVER_MSG_SD )
    { 
        CellularStateProcess( &CellData, CELL_POWER_DOWN_STATE); 
    }
    else if(msg ==SERVER_MSG_START )
    {
        CellularStateProcess( &CellData, CELL_IDLE_STATE);
    }
#endif
    
//for(;;)
{
    switch( CellData.machState )
    {
        case CELL_INIT_STATE:  
            //SystemStartup =TRUE;
            //CellularStateProcess( &CellData, CELL_IDLE_STATE);            
            break;
        case CELL_IDLE_STATE:        
            GoSMSMsg =SERVER_MSG_NONE;
            GoTcpMsg =SERVER_MSG_NONE;
            CellTurnonState =0;
                       
            CellInitModuleState =CELL_INIT_GSM_START;
            CellInitGprsState =CELL_INIT_GPRS_START;          
            
            //CellularStateProcess( &CellData, CellData.machState, CELL_WAIT_TX_STATE);
            
            SciAsciiReceiverInit(SCI_CELLULAR_COM, SCI_CELLULAR_BAUD_RATE, CellularProcessCommands, NULL_PTR);
            
            CellularStateProcess( &CellData, CELL_TURN_ON_STATE);
            break;
        case CELL_WAIT_TX_STATE:      
            GoTcpMsg =ServerCheckMessageToSend();
            
            if( GoTcpMsg !=SERVER_MSG_NONE )
            {
                /* power up cell and send message(s) */
                CellularStateProcess( &CellData, CELL_TURN_ON_STATE);
            }
            break;
        case CELL_TURN_ON_STATE:       
            if( CellularTurnOn() )
            {                          
                CellularStateProcess( &CellData, CELL_INIT_GSM_STATE);                
            }
            break;
        case CELL_INIT_GSM_STATE:                           
            if( CellularInitGsm() ==CELL_INIT_GSM_COMPLETE )
            {              
                rssiNa =0;
                
                memset(&CellRadioParameters, 0x00, sizeof(CellRadioParameters) );
                /* delay before moving to next state */
                TimerDelayUs(10000);
                CellularStateProcess( &CellData, CELL_INIT_GPRS_STATE);
            }
            else
                CellularStateProcess( &CellData, CELL_WAIT_CELL_ACK_STATE);                                      
            break;
        case CELL_INIT_GPRS_STATE:           
        #ifdef TEST_GPRS
            initGprsState =CellularTestGprs();
        #else            
            initGprsState =CellularInitGprs();
        #endif           
            if( initGprsState ==CELL_INIT_GPRS_COMPLETE ) 
            {
                /* connected to network/server */
                /* init server transport layer */    
                transport_init();
                CellularStateProcess( &CellData, CELL_READY_STATE);
            }
            else
            {
                CellularStateProcess( &CellData, CELL_WAIT_CELL_ACK_STATE);                                                    
            }            
            break;
        case CELL_WAIT_CELL_ACK_STATE:
            if( CellResponse.reply ==CELL_REPLY_OK  ||
                CellResponse.reply ==CELL_REPLY_CONNECT
              ) 
            {
                TimerDelayUs(10000); /* 10msec */
                
                CellResponse.ack =FALSE;
                
                CellResponse.reply =CELL_REPLY_NONE;
                    
                CellularStateProcess( &CellData, CellData.prevMachState);
                
                if(initGprsState ==CELL_INIT_GPRS_GET_RSSI) 
                {
                    if( CellRadioParameters.rssi == CELL_RSSI_UNKNOWN_UNDETECTABLE )
                    {
                        /* delay a bit, then try again */
                        CellularStateProcess( &CellData, CELL_RSSI_WAIT_STATE);
                    }
                }                                    
            }
            else if( (xTaskGetTickCount() -CellData.timer) > CELL_RESPONSE_TIMEOUT ||
                      CellResponse.reply ==CELL_REPLY_ERROR
                   )
            {
                /* no response, ERROR response, or unknown response */                
                CellularStateProcess( &CellData, CELL_FAIL_STATE);
            }
            break;
        case CELL_READY_STATE:           
#if 0            
            if( GoTcpMsg )
            {
                /* we have a message to send */
                ServerBuildMessage(tempBuf, GoTcpMsg); 
                                             
                CellularStateProcess( &CellData, CELL_WAIT_SERVER_ACK);   
            }
            else
            {
                /* no more messages, power down cell */
                CellularStateProcess( &CellData, CELL_POWER_DOWN_STATE);       
            }
            
            if( CellularSmsMessageTest(&GoSMSMsg) )
            {
                CellularStateProcess( &CellData, CELL_WAIT_CELL_ACK_STATE);
            }
#endif
            
            if( CellResponse.reply ==CELL_REPLY_NO_CARRIER )
            {
                status =0;
                CellularStateProcess( &CellData, CELL_INIT_STATE); //CELL_FAIL_STATE);
            }                                
            break;     
        case CELL_WAIT_SERVER_ACK:
            if( (xTaskGetTickCount() -CellData.timer) > WAIT_SERVER_ACK_TIMEOUT )
            {
                /* timeout */
                ServerClearMessageToSend(GoTcpMsg); 
                               
                /* check for any new messages */
                GoTcpMsg =ServerCheckMessageToSend();
                
                CellularStateProcess( &CellData, CELL_READY_STATE);    
            }
            break;
        case CELL_FAIL_STATE:
            if( (xTaskGetTickCount() -CellData.timer) > WAIT_TRY_AGAIN_TIMEOUT )
            {                
                if( ++cellFailCnt >3 )
                {
                    /* lets give up for awhile */
                    //CellularStateProcess( &CellData, CELL_POWER_DOWN_STATE2);
                    /* lets give up */
                    CellularStateProcess( &CellData, CELL_POWER_DOWN_STATE);
                }
                else
                {
                    /* lets start over */
                    CellularStateProcess( &CellData, CELL_IDLE_STATE);
                }
            }
            break;
        case CELL_RSSI_WAIT_STATE:
            if( (xTaskGetTickCount() -CellData.timer) > 2000 )
            {                                 
                if( rssiNa ++ >45 )  /* this will be about 90 seconds of no signal strength, quit!!! */
                {
                    /* cannot get a good cell signal */
                    CellularStateProcess( &CellData, CELL_FAIL_STATE);
                }                      
                else
                    CellularStateProcess( &CellData, CELL_INIT_GPRS_STATE);
            }
            break;
        case CELL_DISABLE_STATE: 
            if( SOL_PGOOD )
            {
                CellularStateProcess( &CellData, CELL_IDLE_STATE);
            }
            break;
        case CELL_POWER_DOWN_STATE:
            //PowerDownPeripheral(PWR_ALL_PERIPH);                                                              
            CellularStateProcess( &CellData, CELL_INIT_STATE);              
            break;
        case CELL_POWER_DOWN_STATE2:
            //PowerDownPeripheral(PWR_ALL_PERIPH);                                  
            if( (xTaskGetTickCount() -CellData.timer) > POWER_DOWN_TIMEOUT )
            {
                cellFailCnt =0;
                CellularStateProcess( &CellData, CELL_INIT_STATE);              
            }
            break;            
    }
    
#if 0
    if( !SOL_PGOOD )
    {
        int x=5;
        //CellularStateProcess( &CellData, CELL_DISABLE_STATE);    
    }
#endif    

}    
    return status;
}

/*
*|------------------------------------------------------------------------------
*|  Routine: CellularProcessCommands
*|  Description: process ASCII data received from the cell
*|  Retval:
*|------------------------------------------------------------------------------
*/
void CellularProcessCommands
(
    int cmd,
    char *pRxBuf
)
{  
    char *ptr;
    
    UINT16 ftpRecvSize =0;
    UINT32 ftpFileSize =0;
    
    RTC_TimeTypeDef rtcTimeStruct;  
    RTC_DateTypeDef rtcDateStruct;
    
    CellResponse.ack =TRUE;        
    CellResponse.reply =CELL_REPLY_NONE; 
        
    if( strstr(pRxBuf,"+CSQ:") !=0 )
    {
        /* get rssi value, dBm =(rssi * 2) –113 */
        ptr =strstr( pRxBuf, " " );
                          
        CellRadioParameters.rssi =( *(ptr+1)-'0' );
        
        if( *(ptr+2) !=',' )
	{
            /* there is another digit available (rssi>9) */
            CellRadioParameters.rssi *=10;
            CellRadioParameters.rssi += ( *(ptr+2) -'0');
        }       
    }
    else if( strstr(pRxBuf,"+CPIN: READY") !=0 )
    {
        CellRadioParameters.simPresent =1;              
    }      
    else if( strstr(pRxBuf, "+CGMI:") !=0 )
    {
        /* manufacturer identification */
    }         
    else if( strstr(pRxBuf, "+CGMM:") !=0 )
    {
        /* model identification */
    }         
    else if( strstr(pRxBuf, "+CGMR:") !=0 )
    {
        /* revision */
    }             
    else if( strstr(pRxBuf, "+CGSN:") !=0 )
    {
        /* serial number (IMEI) */
    }                 
    else if( strstr(pRxBuf, "#CCID:") !=0 )
    {
        /* ICCID number (SIM number) */
	ptr =strstr( pRxBuf, " " );

        CellRadioParameters.simNbr = ( ( (UINT32)*(ptr+16) -'0')*10000);
	CellRadioParameters.simNbr +=( ( (UINT32)*(ptr+17) -'0')*1000);
	CellRadioParameters.simNbr +=( ( (UINT32)*(ptr+18) -'0')*100);
	CellRadioParameters.simNbr +=( ( (UINT32)*(ptr+19) -'0')*10);
	CellRadioParameters.simNbr +=( ( (UINT32)*(ptr+20) -'0'));           
    }
    else if( strstr(pRxBuf, "+GMM") !=0 )
    {
        /* radio type GE865, HE910, LE910-NA1, etc */
    }
    else if( strstr(pRxBuf, "+CREG:") !=0 )
    {
        /* */
	ptr =strstr( pRxBuf, "," );
        
        CellRadioParameters.regStatus =( *(ptr+1)-'0' );
    }
    else if( strstr(pRxBuf, "+GMM") !=0 )
    {        
        /* radio type */
    }
    else if( strstr(pRxBuf, "#FTPRECV:") !=0 )
    {        
        /* FTP get response */
        ptr =strstr( pRxBuf, " " );
        *ptr++;
        
        ftpRecvSize =( *ptr -'0');

        for(int j=0; j<3; j++)
        {
            *ptr ++;
            
            if( *ptr >='0' && *ptr <='9' )
            {
                ftpRecvSize *=10;
                
                ftpRecvSize += *ptr -'0';
            }
            else
                break;
        }
        
        memset( &FtpRecvArray, 0x00, sizeof(FtpRecvArray));
        memcpy( &FtpRecvArray, (ptr + 2), ftpRecvSize);
        
        CellFtpData.bytesDownloaded +=ftpRecvSize;
          
        //CellResponse.reply =CELL_REPLY_OK;
    }    
    else if( strstr(pRxBuf, "#FTPFSIZE:") !=0 )
    {
        ptr =strstr( pRxBuf, " " );
        *ptr++;
        
        ftpFileSize =( *ptr -'0');

        for(int j=0; j<6; j++)
        {
            *ptr ++;
            
            if( *ptr >='0' && *ptr <='9' )
            {
                ftpFileSize *=10;

                ftpFileSize += *ptr -'0';
            }
            else
                break;
        }      
        
        CellFtpData.fileSize =ftpFileSize;        
    }
    else if( strstr(pRxBuf, "#NTP:") !=0 )
    {
        /* message format #NTP: yy/mm/dd,hh:mm:ss+xx 
           note the 'OK' seems to have a bit of a delay from this response, 
           so if you break here you will miss it
        */
        ptr =strstr( pRxBuf, " " );
        *ptr++;
        
        rtcDateStruct.RTC_WeekDay =0; /* make sure this is 0 */
        
        rtcDateStruct.RTC_Year =( *ptr -'0') *10;
        *ptr++;
        rtcDateStruct.RTC_Year +=( *ptr -'0');
        *ptr++;
        
        /* skip the '/' */
        *ptr++;
        
        rtcDateStruct.RTC_Month =( *ptr -'0') *10;
        *ptr++;
        rtcDateStruct.RTC_Month +=( *ptr -'0');        
        *ptr++;        
       
        /* skip the '/' */
        *ptr++;
        
        rtcDateStruct.RTC_Date =( *ptr -'0') *10;
        *ptr++;
        rtcDateStruct.RTC_Date +=( *ptr -'0');        
        *ptr++;        

        /* skip the ',' */
        *ptr++;
        
        rtcTimeStruct.RTC_Hours =( *ptr -'0') *10;
        *ptr++;
        rtcTimeStruct.RTC_Hours +=( *ptr -'0');        
        *ptr++;        

        /* skip the ':' */
        *ptr++;        
        
        rtcTimeStruct.RTC_Minutes =( *ptr -'0') *10;
        *ptr++;
        rtcTimeStruct.RTC_Minutes +=( *ptr -'0');        
        *ptr++;        

        /* skip the ':' */
        *ptr++;        

        rtcTimeStruct.RTC_Seconds =( *ptr -'0') *10;
        *ptr++;
        rtcTimeStruct.RTC_Seconds +=( *ptr -'0');        
        *ptr++;        

        /* set date and time */
        if( !RtcSetDate(RTC_Format_BIN, &rtcDateStruct) )
            RtcSetDate(RTC_Format_BIN, &rtcDateStruct);
                
        if( !RtcSetTime(RTC_Format_BIN, &rtcTimeStruct) )
            RtcSetTime(RTC_Format_BIN, &rtcTimeStruct);
    }
    
    /* check for these specific replies */
    if( strstr(pRxBuf, "OK") !=0 )
    {
        CellResponse.reply =CELL_REPLY_OK;      
    }    
    else if( strstr(pRxBuf, "CONNECT") !=0 )
    {
        CellResponse.reply =CELL_REPLY_CONNECT;
    }    
    else if( strstr(pRxBuf, "NO CARRIER") !=0 )
    {
        /* socket disconnect */
        CellResponse.reply =CELL_REPLY_NO_CARRIER;        
    }      
    else if( strstr(pRxBuf, "+++") !=0 )
    {
        CellResponse.reply =CELL_REPLY_OK;            
    } 
    else if( strstr(pRxBuf, "ERROR") !=0 )
    {        
        CellResponse.reply =CELL_REPLY_ERROR;
    #ifdef TEST_GPRS        
        CellResponse.reply =CELL_REPLY_OK;
    #endif
    }
    else if( strstr(pRxBuf,"404 Not Found") !=0 )
    {
        CellResponse.reply =CELL_REPLY_ERROR;      
    }   
    else if( strstr(pRxBuf,"+PACSP1") !=0 )
    {
        CellResponse.reply =CELL_REPLY_OK;            
    }
    else if( strstr(pRxBuf,"SIM not inserted") !=0 )
    {
       /* note the ERROR message above will catch this, however we may want to 
          do something different if it is a SIM not inserted error,
          so this code is for future consideration
       */
       CellResponse.reply =CELL_REPLY_ERROR;      
    }        
    //else if( strstr(pRxBuf,"Connection: Upgrade") !=0 )
    else if( strstr(pRxBuf,"Upgrade: websocket") !=0 ||
             strstr(pRxBuf,"Upgrade: WebSocket") !=0
           )
    {
       /* Websocket open, handshake/request accepted, switch our processing
          callback 
       */       
       //SciAsciiReceiverInit(SCI_CELLULAR_COM, SCI_CELLULAR_BAUD_RATE, CellularProcessCommands, WSClient_recv_message);
       SciAsciiReceiverInit(SCI_CELLULAR_COM, 921600, CellularProcessCommands, WSClient_recv_message);
       CellResponse.reply =CELL_REPLY_WS_OPEN; 
    }        
    else if( strstr(pRxBuf,"> ") !=0 )    
    {
        /* SMS init response */
        CellResponse.reply =CELL_REPLY_OK;
    }
    else
    {
        /* unrecognized, NACK */
        //CellResponse.ack =FALSE;             
    }
    
 #ifdef DEBUG_CELL_COMMS
    /* echo for debugging */    
    for(int j=0; j< strlen(pRxBuf); j++)
    {
        /* clear any null characters from the buffer */
        if(pRxBuf[j] ==0x7e )
            pRxBuf[j] =0x00;
    }
    
    SciTxPacket(SCI_PC_COM, strlen(pRxBuf), pRxBuf);
 #endif    
} 


/*
*|------------------------------------------------------------------------------
*|  Routine: CellularSmsMessageTest
*|  Description: Test SMS messaging
*|  Retval: BOOL
*|------------------------------------------------------------------------------
*/
BOOL CellularSmsMessageTest( UINT8 *pSmsState )
{
    UINT8 static MsgCnt =0;
    char txBuf[128];
    
    BOOL smsWaitAck =TRUE;
    
    memset(txBuf, 0x00, sizeof(txBuf) );
      
    if( MsgCnt <50 )
    {                              
        switch( *pSmsState)          
        {
            case 1:
                strcpy( txBuf, "AT+CMGF=1\r\n" );                                    
                break;        
            case 2:        
                strcpy( txBuf, "AT+CMGS=\"+14039732584\"\r\n" ); 
                //strcpy( txBuf, "AT+CMGS=\"+14035618095\"\r\n" ); // N.Forbes
                //strcpy( txBuf, "AT+CMGS=\"+12147698048\"\r\n" ); // Trinity contact
                break;
            case 3:
                MsgCnt ++;
                
                //strcpy( txBuf, "Good afternoom\nHow are you today\nThis message has been sent from your newest BFF TRIG Gateway :)\x1A\r\n");//, Counter);
                strcpy( txBuf, "Do not reply\nThis is a SMS message from TRIG gateway\x1A\r\n");//, Counter);
                break;
            case 4:
            default:
                *pSmsState =SERVER_MSG_NONE;
                smsWaitAck =FALSE;
                break;
        }
    }

    if( *pSmsState !=0 )
    {
        SciTxPacket(SCI_CELLULAR_COM, strlen(txBuf), txBuf);    
        
        *pSmsState +=1;
    }
    
    if( (xTaskGetTickCount() -CellData.timer) > 7200000 )
    {
        /* lets send another text message */
        *pSmsState =SERVER_MSG_NONE;
    }

    return smsWaitAck;
}


void CellularInitRequestRssi(void)
{
    RssiState =0;
}

/*
*|------------------------------------------------------------------------------
*|  Routine: CellularRequestRssi
*|  Description: Switch to command mode and get RSSI value
*|  Retval: UINT8
*|------------------------------------------------------------------------------
*/
BOOL CellularRequestRssi(void)
{
    char tempBuf[32];
    UINT8 cellResponse;
    static UINT32 rssiTimer =0;
    BOOL status =FALSE;
    
    switch( RssiState ) 
    {
        case 0:          
            CellularSendCmdMode();      
            
            rssiTimer =xTaskGetTickCount();
            RssiState =1;
            break;
        case 1:
            if( CellularGetResponse() ==CELL_REPLY_NONE )
            {
                if( (xTaskGetTickCount() -rssiTimer )>5000 )
                {
                    RssiState =0;                                      
                }                
                
                return status;
            }

            CellResponse.reply =CELL_REPLY_NONE;            
            
            RssiState =2;            
            break;
        case 2:
            strcpy( tempBuf,"AT+CSQ\r\n");   
            SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), tempBuf); 
            
            CellResponse.reply =CELL_REPLY_NONE;            
                            
            rssiTimer =xTaskGetTickCount();
            RssiState =3;
            break;
        case 3:
            cellResponse =CellularGetResponse();
            if( cellResponse !=CELL_REPLY_NONE ||
                (xTaskGetTickCount() -rssiTimer) >5000
              )
            {
            #define RESTORE_SOCKET
            #ifdef RESTORE_SOCKET
                /* restore suspended socket, does not work for
                   upgraded websocket
                */
                strcpy( tempBuf,"AT#SO=1\r\n");   
                SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), tempBuf); 
                
                RssiState =0;                
            #else
                RssiState =2;     
            #endif
                
                status =TRUE;
            }
            break;
    }              
    
    return status;
}


/*
*|------------------------------------------------------------------------------
*|  Routine: CellularGetRssi
*|  Description: 
*|  Retval: UINT8
*|------------------------------------------------------------------------------
*/
UINT8 CellularGetRssi(void)
{
    return CellRadioParameters.rssi;
}

int CellularGetResponse(void)
{
    return CellResponse.reply; 
}

void CellularClearResponse(void)
{
    CellResponse.reply =CELL_REPLY_NONE; 
}

BOOL GetFtpStatus(void)
{
    if( CellFtpData.bytesDownloaded ==CellFtpData.fileSize )
        return TRUE;
    else 
        return FALSE;
}

void SaveFtpsCertificate(void)
{
    char tempBuf[128];
    int nbrByte =0;
    
    strcpy(tempBuf,"-----BEGIN CERTIFICATE-----\r\n");            
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                 
    nbrByte=strlen(tempBuf);            
            
    strcpy(tempBuf,"MIIDfDCCAmSgAwIBAgIJAIKde8Pk7+0zMA0GCSqGSIb3DQEBCwUAMFMxCzAJBgNV\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                 
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"BAYTAkNBMQswCQYDVQQIDAJBQjEQMA4GA1UEBwwHQ2FsZ2FyeTENMAsGA1UECgwE\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                             
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"VFJJRzEWMBQGA1UEAwwNMTU5Ljg5LjExNy45ODAeFw0xOTAxMTAwMDMwMzVaFw0y\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                                         
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"MDAxMTAwMDMwMzVaMFMxCzAJBgNVBAYTAkNBMQswCQYDVQQIDAJBQjEQMA4GA1UE\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);  
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"BwwHQ2FsZ2FyeTENMAsGA1UECgwEVFJJRzEWMBQGA1UEAwwNMTU5Ljg5LjExNy45\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);              
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"ODCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKuwUYIn7fi0BYhfWaof\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                          
    nbrByte +=strlen(tempBuf);            
          
    strcpy(tempBuf,"vaNsSitghDoOGvhsdUM7SkTs4D7zb3ThkkwXNqWwngfNx9EaQT5qYDtPvpqNN3oh\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                                                             
    nbrByte +=strlen(tempBuf);            

    strcpy(tempBuf,"qb4OSrSoJN3LDUFjQOMMZhIO8URGS0L+drfxVe6Y2FClVygQsnNo12JbBRpPO7nm\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"v6H7WGa86UViYWDmX41wpf/w3vJf6V7zCgphMbQu6+ygAVgQGP6hSaD5Mb1AwUT5\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
           
    strcpy(tempBuf,"HC3DLDviOcj21tZPwfRPGIaRGsnuHrVcSoHkWeQgayvaeKHr8b/saYBJ4pWjQhRS\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"H5hseJDIuv6vumN1MyMzW+iEoTdJhrJW813XBXHHgF/2N80dGGRqgRbBJktXP0OW\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"Fe0CAwEAAaNTMFEwHQYDVR0OBBYEFLYKHz7gMmC70eExdQ3ETuU9G3QtMB8GA1Ud\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"IwQYMBaAFLYKHz7gMmC70eExdQ3ETuU9G3QtMA8GA1UdEwEB/wQFMAMBAf8wDQYJ\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"KoZIhvcNAQELBQADggEBABprkXCS0x0+FEg5RCozIVuXBZY6wH2Ie8qq4ouFJy06\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"iBLxoQWeO7qSBLilnXqoTPIj/Eb5m42r4NeyoGqYE9NLtf8BJAjKdUmns3Xj4k00\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"pn2+1h21YkfqPiPDTwQKHHrR187djT4iFEeESlvkbD1H2z9Qo+3D+2MS4Izr2V8/\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"iYQuqxojKOZLRF8glOTVIHFNZXu0cT61HUaJWggPpAGiKnnNG5tYJxS+eFMO+uOG\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"oodfJqz35dLCSlUl4mvUgbVWF7IgPRDbXO4c4FJ2X5uLRwWINwhnCziLFlU/y4/u\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"w/09OueFVbmvc26RhqE/VCoPe+XoSZcd7tS4sIsT7uk=\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            
            
    strcpy(tempBuf,"-----END CERTIFICATE-----\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);
    nbrByte +=strlen(tempBuf);            

    /* send CTRL-Z to save */
    strcpy(tempBuf,"\x1A\r\n");
    SciTxPacket(SCI_CELLULAR_COM, strlen(tempBuf), (char *)tempBuf);                   
}


/* end CellularMachine.c */

