/** C Source *******************************************************************
*
* NAME      MQTTClient.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
********************************************************************************
* Copyright (c) 2018, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/

/** Include Files *************************************************************/
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

//#include "MQTTPacket.h"
#include "CellularMachine.h"
#include "MQTTClient.h"
#include "Transport.h"
#include "ServerPacket.h"

#define KEEPALIVE_INTERVAL 60

void MQTTDisconnect(void);

/* This is to get a timebase in seconds to test the sample */
#include <time.h>
time_t old_t;

void start_ping_timer(void)
{
    time(&old_t);
    old_t += KEEPALIVE_INTERVAL/2 + 1;
}

int time_to_ping(void)
{
    time_t t;

    time(&t);
    if(t >= old_t)
  	return 1;
   
    return 0;
}

/* This is in order to get an asynchronous signal to stop the sample,
as the code loops waiting for msgs on the subscribed topic.
Your actual code will depend on your hw and approach*/
#include <signal.h>

enum
{
    MQTT_INIT_STATE =0,
    MQTT_CONNECT_STATE,
    MQTT_SUBSCRIBE_STATE,
    MQTT_PUBLISH_STATE,
    MQTT_DISCONNECT_STATE,
    
    MQTT_WAIT_RESPONSE_STATE,
    MQTT_WAIT_NACK_STATE,

    MQTT_LAST_STATE
};

unsigned char buf[200];  

int toStop = 0;

int MQTTNextMachState;
int MQTTMachState;


int MQTTMessageHandler(int msg, int res_qos);

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTClientInit
*|  Description:
*|----------------------------------------------------------------------------
*/
void MQTTClientInit(void)
{
    MQTTMachState =MQTT_INIT_STATE;
    MQTTNextMachState =MQTTMachState;    
}

void MQTTClientStart(void)
{
    MQTTMachState =MQTT_CONNECT_STATE;
    MQTTNextMachState =MQTTMachState;
}



/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTClientShutdown
*|  Description:
*|----------------------------------------------------------------------------
*/
void MQTTClientShutdown(void)
{
    MQTTDisconnect();
    MQTTMachState =MQTT_INIT_STATE;   
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTClientMachine
*|  Description:
*|----------------------------------------------------------------------------
*/
int MQTTClientMachine(UINT8 msg)
{
    int status =0;   
    static int req_qos =0;
    static unsigned char payloadMsg[200];       
    static TickType_t timeoutTimer =0;
    static int msgResponse =0;
      
    MQTTPacket_connectData client = MQTTPacket_connectData_initializer;     
    
    TickType_t xTicks=xTaskGetTickCount();
               
    switch(MQTTMachState)
    {
        case MQTT_INIT_STATE:
            break;
        case MQTT_CONNECT_STATE:
            client.clientID.cstring = "me";
            client.keepAliveInterval = KEEPALIVE_INTERVAL;
            client.cleansession = 1;
            client.username.cstring ="FreeRTOS_sample";// "smeho4dXG0opDe6P0fNS";
            client.password.cstring = "testpassword";

            status =MQTTConnect(&client);    

            msgResponse =CONNACK;
            timeoutTimer =xTicks;                
            MQTTNextMachState =MQTT_SUBSCRIBE_STATE; //MQTT_PUBLISH_STATE;//MQTT_SUBSCRIBE_STATE;
            MQTTMachState =MQTT_WAIT_RESPONSE_STATE;
            break;
            /* purposely fall into next state */
        case MQTT_SUBSCRIBE_STATE: 
            req_qos =0;
            
            //status =MQTTSubscribe("v1/devices/me/attributes/response/+", req_qos);
            //status =MQTTSubscribe("v1/devices/me/telemetry/response/+");
            status =MQTTSubscribe("FreeRTOS/sample/#", req_qos);
            
            msgResponse =SUBACK;
            timeoutTimer =xTicks;      
            MQTTNextMachState =MQTT_WAIT_NACK_STATE;//MQTT_PUBLISH_STATE;            
            MQTTMachState =MQTT_WAIT_RESPONSE_STATE;             
            break;
            /* purposely fall into next state */
        case MQTT_PUBLISH_STATE:
            /* we have a message to send */
            ServerBuildMessage( (char *)payloadMsg, msg); 
            
            /* publish to this topic to receive data from the server */
            //status =MQTTPublish("v1/devices/me/attributes/request/1", (char *)payloadMsg);            
            //status =MQTTPublish("v1/devices/me/attributes", (char *)payloadMsg);            
            //status =MQTTPublish("v1/devices/me/telemetry", (char *)payloadMsg);            
            status =MQTTPublish("FreeRTOS/sample/a", (char *)payloadMsg);            
                        
#if 1            
            /* telemetry has no response/ack ??? 
               we need to hold up here to allow time for our 
               message to cross the network
            */                       
            timeoutTimer =xTicks;      
            MQTTNextMachState =MQTT_PUBLISH_STATE;            
            MQTTMachState =MQTT_WAIT_NACK_STATE;             
#else
            msgResponse =PUBLISH;
            timeoutTimer =xTicks;      
            MQTTNextMachState =MQTT_WAIT_NACK_STATE;//MQTT_PUBLISH_STATE;            
            MQTTMachState =MQTT_WAIT_RESPONSE_STATE;             
#endif                       
            break;
        case MQTT_DISCONNECT_STATE:
            MQTTDisconnect();
            MQTTMachState =MQTT_INIT_STATE;            
            break;
        case MQTT_WAIT_RESPONSE_STATE:
            if( MQTTMessageHandler(msgResponse, req_qos) ==1 )
            {
                MQTTMachState =MQTTNextMachState;
            }
            
            if( (xTicks -timeoutTimer) >10000 )
            {
                timeoutTimer =xTicks;                
                status =2;
            }            
            break;   
        case MQTT_WAIT_NACK_STATE:         
            if( (xTicks -timeoutTimer) >10000 )
            {
//                timeoutTimer =xTicks;                
//                status =1;                
                MQTTMachState =MQTTNextMachState;
            }            
            break;               
    }
    
    return status;
}


/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTConnect
*|  Description:
*|----------------------------------------------------------------------------
*/
int MQTTConnect(MQTTPacket_connectData* client)
{
    int rc = 0;
    int status =0;
    int mysock = 0;
    int buflen = sizeof(buf);
    int len = 0;
    
    mysock = SCI_CELLULAR_COM;
        
    len = MQTTSerialize_connect(buf, buflen, client);    
    rc =transport_sendPacketBuffer(mysock, (char *)buf, len);   
    
    return status;
}


/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTPublish
*|  Description:
*|----------------------------------------------------------------------------
*/
int MQTTPublish(char *pTopic, char *pMsg)
{
    MQTTString topicString = MQTTString_initializer;
    MQTTString receivedTopic;
    unsigned char *payload_in;        
    
    int req_qos = 0;
    
    int rc = 0;
    int mysock = 0;

    int buflen = sizeof(buf);
    int len = 0;
    mysock =SCI_CELLULAR_COM;
     
    memset(buf, 0x00, buflen );
      
    /* loop getting msgs on subscribed topic */    
    topicString.cstring =pTopic;
    //topicString.cstring = "FreeRTOS/sample/a";
    //topicString.cstring = "v1/devices/me/atributes/request/1";
    //topicString.cstring = "v1/devices/me/attributes";
    
    char *payload;        
    //payload = "{\"messages\":\"telemetry message change\"}";
    //payload = "messages:what up ";
    //payload ="{\"temperature\":35.0,\"humidity\":65.0,\"active\":true,\"isCool\":true}"; 
    payload =pMsg;
    int payloadlen = strlen(payload);
               
    /* publishing reading */
    req_qos =0;
    len = MQTTSerialize_publish(buf, buflen, 0, req_qos, 0, 0, topicString, (unsigned char*)payload, payloadlen);
    rc = transport_sendPacketBuffer(mysock, ( char *)buf, len);
            
#if 0    
    /* transport_getdata() has a built-in 1 second timeout,
       your mileage will vary */
    if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
    {
        unsigned char dup;
        int qos;
        unsigned char retained;
        unsigned short msgid;
        int payloadlen_in;
                   
        rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                                     &payload_in, &payloadlen_in, buf, buflen);
         
        
        //MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
        /* message acknowledged */ 
    }
    else
        return 0;
    
    if( strstr( (char *)buf, "NO CARRIER") !=0 )
        return 0;
#endif
    
    //else
        //MQTTInit();                        

#if 0
    /* ping */
    while (!toStop)
    {         
        xTicks=xTaskGetTickCount();
        if( (xTicks - PingTimer) >15000 )      
        {
            Go =1;
            PingTimer =xTicks;
        }
        
        if( Go ==1)
        {
            len = MQTTSerialize_pingreq(buf, buflen);
            transport_sendPacketBuffer(SCI_CELLULAR_COM, buf, len);
            //SciTxPacket(SCI_CELLULAR_COM, len, (char *)buf);

            /* ping */
            if (MQTTPacket_read(buf, buflen, transport_getdata) == PINGRESP){
                /* pong */
                start_ping_timer();
            }
            else {
                /* OOPS */
                goto exit;
            }	
            
            Go =0;        
        }
    }
#endif
            
    return 0;
}


/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTSubscribe
*|  Description:
*|----------------------------------------------------------------------------
*/
int MQTTSubscribe(char *pTopic, int req_qos)
{
    MQTTString topicString = MQTTString_initializer;

    int msgid = 1;
    int status =0;
    
    int rc = 0;
    int mysock =0;  
    
    int buflen = sizeof(buf);
    int len = 0;

    TickType_t xTicks=xTaskGetTickCount();
    
    mysock =SCI_CELLULAR_COM;
    //req_qos =0;
    
    /* subscribe */
    //topicString.cstring = "FreeRTOS/sample/#";
    topicString.cstring = pTopic; //"v1/devices/me/attributes/#"; //"v1/devices/me/telemetry/response/+"; 
    len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

    rc =transport_sendPacketBuffer(mysock, (char *)buf, len);
      
#if 0    
    if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
    {
        unsigned short submsgid;
	int subcount;
	int granted_qos =0;

	rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
   
	if(granted_qos != req_qos)
	{
            /* granted qos !=req_qos, failure */
            status =0;
	}
        else 
            status =1;
    }
    else
        status =0;
#endif
    
    return status;
}

/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTMessageHandler
*|  Description:
*|----------------------------------------------------------------------------
*/
int MQTTMessageHandler(int msg, int req_qos)
{
    int rc =0;       
    int buflen = sizeof(buf);  
    
    switch(msg)
    {
        case CONNACK:
            /* wait for connack */           
            if (MQTTPacket_read(buf, buflen, transport_getdata) == msg)
            {
                unsigned char sessionPresent, connack_rc;

                if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
                {
                    /* unable to connect */
                    rc =2;
                }
                else 
                {
                    rc =1;
                }
            }
            // else nothing yet
            break;
        case SUBACK:
            if (MQTTPacket_read(buf, buflen, transport_getdata) == msg) 	/* wait for suback */
            {
                unsigned short submsgid;
                int subcount;
                int granted_qos =0;

                MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
           
                if(granted_qos != req_qos)
                {
                    /* granted qos !=req_qos, failure */
                    rc =2;
                }
                else 
                    rc =1;
            }
            break;
        case PUBLISH:
            if (MQTTPacket_read(buf, buflen, transport_getdata) == msg)
            {
                unsigned char dup;
                int qos;
                unsigned char retained;
                unsigned short msgid;
                int payloadlen_in;
                 
                MQTTString receivedTopic;
                unsigned char *payload_in;  
    
                if( MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                                             &payload_in, &payloadlen_in, buf, buflen) != SUCCESS
                  )
                {
                    rc =2;
                }
                else
                    rc =1;
                                 
                //MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
                /* message acknowledged */ 
            }          
            break;
    }
    
    /* !!!TO DO!!!
       hardware socket disconnect, note this is cellular response, this needs 
       fixing if using other hardware
    */
    if( strstr( (char *)buf, "NO CARRIER") !=0 )
        rc =2;
    
    return rc;
}


/*
*|----------------------------------------------------------------------------
*|  Module: 
*|  Routine: MQTTDisconnect
*|  Description:
*|----------------------------------------------------------------------------
*/
void MQTTDisconnect(void)
{
    int len = 0;
    int buflen = sizeof(buf);
    int mysock =0;
    
    mysock =SCI_CELLULAR_COM;
    /* disconnecting */
    len = MQTTSerialize_disconnect(buf, buflen);
    transport_sendPacketBuffer(mysock, (char *)buf, len);  
}

