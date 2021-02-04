/** C Source ******************************************************************
*
* NAME      sciHi.c
*
* SUMMARY   
*
* TARGET    
*
* TOOLS     IAR Embedded workbench for ARM v7.4
*
* REVISION LOG
*
*******************************************************************************
* Copyright (c) 2017, TRIG
* Calgary, Alberta, Canada, www.webpage.ca
*******************************************************************************/


#include "sciHi.h"


const uint32_t COM_USART_CLK[COMn] =    {SERIAL_COM1_CLK, SERIAL_COM2_CLK};//, SERIAL_COM3_CLK, SERIAL_COM4_CLK, SERIAL_COM5_CLK, SERIAL_COM6_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] =  {SERIAL_COM1_TX_GPIO_CLK,SERIAL_COM2_TX_GPIO_CLK};//,SERIAL_COM3_TX_GPIO_CLK,SERIAL_COM4_TX_GPIO_CLK,SERIAL_COM5_TX_GPIO_CLK, SERIAL_COM6_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] =  {SERIAL_COM1_RX_GPIO_CLK, SERIAL_COM2_RX_GPIO_CLK};//, SERIAL_COM3_RX_GPIO_CLK, SERIAL_COM4_RX_GPIO_CLK,SERIAL_COM5_RX_GPIO_CLK, SERIAL_COM6_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] =       {SERIAL_COM1_TX_PIN, SERIAL_COM2_TX_PIN};//, SERIAL_COM3_TX_PIN, SERIAL_COM4_TX_PIN,SERIAL_COM5_TX_PIN,SERIAL_COM6_TX_PIN};

const uint16_t COM_RX_PIN[COMn] =       {SERIAL_COM1_RX_PIN, SERIAL_COM2_RX_PIN};//, SERIAL_COM3_RX_PIN, SERIAL_COM4_RX_PIN,SERIAL_COM5_RX_PIN,SERIAL_COM6_RX_PIN};
 
const uint16_t COM_TX_PIN_SOURCE[COMn] ={SERIAL_COM1_TX_SOURCE,SERIAL_COM2_TX_SOURCE};//,SERIAL_COM3_TX_SOURCE,SERIAL_COM4_TX_SOURCE,SERIAL_COM5_TX_SOURCE,SERIAL_COM6_TX_SOURCE};

const uint16_t COM_RX_PIN_SOURCE[COMn] ={SERIAL_COM1_RX_SOURCE, SERIAL_COM2_RX_SOURCE};//,SERIAL_COM3_RX_SOURCE,SERIAL_COM4_RX_SOURCE,SERIAL_COM5_RX_SOURCE,SERIAL_COM6_RX_SOURCE};
 
const uint16_t COM_TX_AF[COMn] =        {SERIAL_COM1_TX_AF, SERIAL_COM2_TX_AF};//, SERIAL_COM3_TX_AF, SERIAL_COM4_TX_AF,SERIAL_COM5_TX_AF,SERIAL_COM6_TX_AF};
 
const uint16_t COM_RX_AF[COMn] =        {SERIAL_COM1_RX_AF, SERIAL_COM2_RX_AF};//, SERIAL_COM3_RX_AF, SERIAL_COM4_RX_AF,SERIAL_COM5_RX_AF,SERIAL_COM6_RX_AF};

USART_TypeDef* COM_USART[COMn] =        {SERIAL_COM1,SERIAL_COM2};//,SERIAL_COM3,SERIAL_COM4,SERIAL_COM5,SERIAL_COM6};
GPIO_TypeDef* COM_TX_PORT[COMn] =       {SERIAL_COM1_TX_GPIO_PORT, SERIAL_COM2_TX_GPIO_PORT};//, SERIAL_COM3_TX_GPIO_PORT, SERIAL_COM4_TX_GPIO_PORT,SERIAL_COM5_TX_GPIO_PORT,SERIAL_COM6_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] =       {SERIAL_COM1_RX_GPIO_PORT, SERIAL_COM2_RX_GPIO_PORT};//, SERIAL_COM3_RX_GPIO_PORT, SERIAL_COM4_RX_GPIO_PORT,SERIAL_COM5_RX_GPIO_PORT,SERIAL_COM6_RX_GPIO_PORT};


/* receive queue structures */
QUE_STRUCT RxBuff[NBR_PORTS];

/* We need this que for the MQTT application */
//QUE_STRUCT SocketQue;

void SciComInit(COMTypeDef sciPort, USART_InitTypeDef* USART_InitStruct);
                
                
/*
*|----------------------------------------------------------------------------
*|  Routine: SciQueInit
*|  Description:
*|  retval:
*|----------------------------------------------------------------------------
*/
void SciQueInit
(
   QUE_STRUCT *quePtr
)
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */


   /*
   *************************
    E R R O R   C H E C K S
   *************************
   */


   /*
   *************************
    C O D E
   *************************
   */
   quePtr->tail = 0x0002; /* Set in=out, and full=0, ie. buffer empty */
   quePtr->full = 0x0000;
   quePtr->head = 0x0002;
   
   memset(quePtr->q, 0x00, sizeof(quePtr->q) );
   
}// SciQueInit()

#if 0
void SciSocketInit(void)
{
    SciQueInit( &SocketQue );
}
#endif


/*
*|----------------------------------------------------------------------------
*|  Routine: SciInitSerialPort
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciSerialPortInit(COMTypeDef sciPort, UINT32 baudRate, UINT8 parity)
{    
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    SciQueInit( &RxBuff[sciPort] );
 
   /* USARTx configured as follow:
          - BaudRate = 115200 baud  
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = baudRate;    
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
    
    if( parity ==0x00 )
    {
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_Parity =USART_Parity_No;
    }
    else if(parity ==0x01 )
    {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        USART_InitStructure.USART_Parity =USART_Parity_Odd;      
    }
    else
    {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        USART_InitStructure.USART_Parity =USART_Parity_Even;            
    }
      
    SciComInit(sciPort, &USART_InitStructure); 

    /* NVIC configuration */
    /* Configure the Priority Group to 2 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//NVIC_PriorityGroup_2);
    
    switch( sciPort )
    {
        case USART_1:
            /* Enable the USARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
    
            /* enable USART interrupt */
            USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          
            break;
        case USART_2:
            /* Enable the USARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn ;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
    
            USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
            break;     
        case USART_3:
            /* Enable the USARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn ;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
    
            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
            break;   
        case UART_4:
            /* Enable the UARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn ;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
    
            USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
            break;            
        case UART_5:
            /* Enable the UARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn ;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
    
            USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
            break;  
        case LPUART_1:
            /* Enable the LPUARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = LPUART1_IRQn ;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
    
            USART_ITConfig(LPUART1, USART_IT_RXNE, ENABLE);
            break;   
#if 0            
        case USART_6:
            /* Enable the USARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
        
            USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
            break;        
        case UART_8:
            /* Enable the USARTx Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = UART8_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
        
            USART_ITConfig(UART8, USART_IT_RXNE, ENABLE);
            break;                    
#endif            
    }   
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciComInit
*|  Description: Configures COM port.
*|  Arguments: COM: Specifies the COM port to be configured
*|             USART_InitStruct: pointer to a USART_InitTypeDef structure that
*|             contains the configuration information for the specified USART peripheral
*|  Retval: none
*|----------------------------------------------------------------------------
*/
void SciComInit(COMTypeDef sciPort, USART_InitTypeDef* USART_InitStruct)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* enable GPIO clock */
    RCC_AHB2PeriphClockCmd(COM_TX_PORT_CLK[sciPort] | COM_RX_PORT_CLK[sciPort], ENABLE);
    
    if( sciPort == USART_1 )// || sciPort == USART_6 )
    {
        /* APB2 is used for USART1 & USART6 */
        RCC_APB2PeriphClockCmd(COM_USART_CLK[sciPort], ENABLE);
    }
    else if(sciPort == LPUART_1 )
    {
        /* APB1ENR2 is used for LPUART1 */
        RCC_APB1ENR2PeriphClockCmd(COM_USART_CLK[sciPort], ENABLE);
    }
    else
    {
        /* APB1 is used for USART2, USART3, UART4, UART5, and LPUART1*/
        RCC_APB1PeriphClockCmd(COM_USART_CLK[sciPort], ENABLE);
    }
    
    /* Connect Pin to USARTx_Tx*/
    GPIO_PinAFConfig(COM_TX_PORT[sciPort], COM_TX_PIN_SOURCE[sciPort], COM_TX_AF[sciPort]);

    /* Connect Pin to USARTx_Rx*/
    GPIO_PinAFConfig(COM_RX_PORT[sciPort], COM_RX_PIN_SOURCE[sciPort], COM_RX_AF[sciPort]);    
 
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[sciPort];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COM_TX_PORT[sciPort], &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[sciPort];
    GPIO_Init(COM_RX_PORT[sciPort], &GPIO_InitStructure);
 
    /* USART configuration */
    USART_Init(COM_USART[sciPort], USART_InitStruct);
      
    /* Enable USART */
    USART_Cmd(COM_USART[sciPort], ENABLE);
}

/*
*|----------------------------------------------------------------------------
*|  Routine: SciSetPort
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciSetPort(UINT16 port)
{
//    SciPort =port;
}


/*
*|----------------------------------------------------------------------------
*|  Routine: SciQue
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciQue(char rxByte, QUE_STRUCT *que ) //char sciPort)
{
    UINT16 qTail, qFull, qHead;
    
    /* Read rx buffer variables into locals */
    qTail =que->tail;// RxBuff[sciPort].tail;
    qFull =que->full;// RxBuff[sciPort].full;
    qHead =que->head;// RxBuff[sciPort].head;
	
    if( (qTail!=qHead) || !qFull )
    {
    	// rx buffer is not full and data is available
    	//RxBuff[sciPort].q[qTail] = (char)rxByte;
        que->q[qTail] = (char)rxByte;

	qTail++;
        if( qTail>(UART_MAX_RX_QUE_SIZE-1) )
        {
            qTail = 0; /* check for wraparound */
        }

#if 0
        if( qTail==qHead )
        {
            // Buffer now full, so set FULL flag coherently with update of qin
            // Also generate warning that buffer is now full
        }
        else
#endif
        {
            // Buffer not full yet, so don't set FULL
            //RxBuff[sciPort].tail = qTail;
             que->tail = qTail;
        }
    }    
}


/*
*|----------------------------------------------------------------------------
*|  Routine: SciGetByte
*|  Description:
*|   Checks serial rx register for data. This checking occurs for a specified
*|   duration or until data is found, whichever comes first.
*|  Retval:
*|----------------------------------------------------------------------------
*/
UINT16 SciGetByte
(
   char sciPort,
   char *pRxByte
)
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */
   UINT16 rxTail, rxFull, rxHead;


   /*
   *************************
    E R R O R   C H E C K S
   *************************
   */

   /* read rx buffer variables into locals */
   rxTail = RxBuff[sciPort].tail;
   rxFull = RxBuff[sciPort].full;
   rxHead = RxBuff[sciPort].head;
   if( (rxTail!=rxHead)||rxFull )
   {
      *pRxByte = RxBuff[sciPort].q[rxHead]; /* read data byte */
      rxHead++; /* update pointer..*/
      if( rxHead>(UART_MAX_RX_QUE_SIZE-1) ) rxHead = 0; /* check for wraparound */

      RxBuff[sciPort].full =0x00;
      RxBuff[sciPort].head =rxHead;

      return 1; /* return 1 for successful read */
   }
   else
   {
      return 0; /* return 0 as no byte available */
   }

}/* SciGetByte() */


/*
*|----------------------------------------------------------------------------
*|  Routine: SciSendByte
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void SciSendByte
(
   char sciPort,
   char txByte
)
{
    switch( sciPort )
    {
        case USART_1:
            /* transmit byte */
            while (!(USART1->ISR & USART_FLAG_TXE));
            
            USART1->TDR = txByte;            
            break;
        case USART_2:
            /* transmit byte */
            while (!(USART2->ISR & USART_FLAG_TXE));
            
            USART2->TDR = txByte;            
            break;            
        case USART_3:
            /* transmit byte */
            while (!(USART3->ISR & USART_FLAG_TXE));
            
            USART3->TDR = txByte;            
            break;
        case UART_4:
            /* transmit byte */
            while (!(UART4->ISR & USART_FLAG_TXE));
            
            UART4->TDR = txByte;            
            break;            
        case UART_5:
            /* transmit byte */
            while (!(UART5->ISR & USART_FLAG_TXE));
            
            UART5->TDR = txByte;            
            break;        
        case LPUART_1:
            /* transmit byte */
            while (!(LPUART1->ISR & USART_FLAG_TXE));
            
            LPUART1->TDR = txByte;            
            break;                
#if 0            
        case USART_6:
            /* transmit byte */
            while (!(USART6->ISR & USART_FLAG_TXE));
            
            USART6->TDR =txByte;
            break;                
        case UART_8:
            /* transmit byte */
            while (!(UART8->ISR & USART_FLAG_TXE));
            
            UART8->TDR =txByte;
            break;        
#endif            
    }   
}
 
/*
*|----------------------------------------------------------------------------
*|  Routine: USART1_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void USART1_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(USART1->RDR &0x1ff);
        
        SciQue(rxByte, &RxBuff[USART_1]);
        
        //SciQue(rxByte, &SocketQue);
    }               
    else if( USART_GetFlagStatus(USART1, USART_FLAG_ORE) ==SET )
    {         
        USART1->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(USART1->RDR &0x1ff);   
    }           
}

/*
*|----------------------------------------------------------------------------
*|  Routine: UART4_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void UART4_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(UART4->RDR );
        
        SciQue(rxByte, &RxBuff[UART_4]);
    }
    else if( USART_GetFlagStatus(UART4, USART_FLAG_ORE) ==SET )
    {         
        UART4->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(UART4->RDR &0x1ff);   
    }     
}

#if 0
/*
*|----------------------------------------------------------------------------
*|  Routine: USART2_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void USART2_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(USART2->RDR );
        
        SciQue(rxByte, &RxBuff[USART_2]);
    }
    else if (USART_GetITStatus(USART2, USART_IT_FE) == SET)
    {
        rxByte =(uint16_t)(USART2->RDR );
        
        SciQue(rxByte, &RxBuff[USART_2]);
    }    
    else if (USART_GetITStatus(USART2, USART_IT_PE) == SET)
    {
        rxByte =(uint16_t)(USART2->RDR );
        
        SciQue(rxByte, &RxBuff[USART_2]);
    }    
        
    else if( USART_GetFlagStatus(USART2, USART_FLAG_ORE) ==SET )
    {         
        USART2->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(USART2->RDR &0x1ff);   
    }           
    
    else if( USART_GetFlagStatus(USART2, USART_FLAG_PE) ==SET )
    {         
        USART2->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(USART2->RDR &0x1ff);   
    }  

    else if( USART_GetFlagStatus(USART2, USART_FLAG_FE) ==SET )
    {         
        USART2->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(USART2->RDR &0x1ff);   
    }  
}


/*
*|----------------------------------------------------------------------------
*|  Routine: USART3_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void USART3_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(USART3->RDR );
        
        SciQue(rxByte, &RxBuff[USART_3]);              
    }
    else if( USART_GetFlagStatus(USART3, USART_FLAG_ORE) ==SET )
    {         
        USART3->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(USART3->RDR &0x1ff);   
    }       
}


/*
*|----------------------------------------------------------------------------
*|  Routine: UART5_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void UART5_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if (USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(UART5->RDR );
        
        SciQue(rxByte, &RxBuff[UART_5]);
    }
    else if( USART_GetFlagStatus(UART5, USART_FLAG_ORE) ==SET )
    {         
        UART5->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(UART5->RDR &0x1ff);   
    }     
}

/*
*|----------------------------------------------------------------------------
*|  Routine: LPUART1_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void LPUART1_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if (USART_GetITStatus(LPUART1, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(LPUART1->RDR );
        
        SciQue(rxByte, &RxBuff[LPUART_1]);
    }
    else if( USART_GetFlagStatus(LPUART1, USART_FLAG_ORE) ==SET )
    {         
        LPUART1->ICR =USART_FLAG_ORE;
        
        rxByte =(uint16_t)(LPUART1->RDR &0x1ff);   
    }     
}

/*
*|----------------------------------------------------------------------------
*|  Routine: USART6_IRQHandler
*|  Description:
*|  Retval:
*|----------------------------------------------------------------------------
*/
void USART6_IRQHandler(void)  
{
    uint8_t rxByte;
  
    if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
    {
        rxByte =(uint16_t)(USART6->RDR );
        
        SciQue(rxByte, USART_6);
    }
}
#endif

#if MQTT_ENABLED
int SciSocketCheckQue(void)
{
    UINT16 rxTail, rxHead;
    UINT16 bytesInQue =0;

    /* read rx buffer variables into locals */
    rxTail = SocketQue.tail;
    //rxFull = SocketQue.full;
    rxHead = SocketQue.head;
          
    bytesInQue =(rxTail -rxHead);  
    
    return bytesInQue;
}


int SciSocketRecv( unsigned char *buf, int count )
{
    /*
    ***********************
     L O C A L   D A T A
    ***********************
    */
    UINT16 rxTail, rxHead;
    UINT16 bytesInQue =0;
    int j;
    /*
     *************************
     E R R O R   C H E C K S
     *************************
    */
    /* read rx buffer variables into locals */
    rxTail = SocketQue.tail;
    //rxFull = SocketQue.full;
    rxHead = SocketQue.head;
          
    bytesInQue =(rxTail -rxHead);
    
    if( bytesInQue >=count )
    {
        for( j=0; j<count; j++ )
        {
            *buf ++ = SocketQue.q[rxHead]; /* read data byte */
            rxHead++; /* update pointer..*/
            if( rxHead>(UART_MAX_RX_QUE_SIZE-1) ) rxHead = 0; /* check for wraparound */

            SocketQue.full =0x00;
            SocketQue.head =rxHead;
        }
        
        return 1;
    }
    else
        return 0; /* return 0 as no bytes available */
   
}
#endif


