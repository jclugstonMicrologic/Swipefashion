/* serialFd.h file modified for Visual C++ to use the built in serial object */
//---------------------------------------------------------------------------
#ifndef serialFdH
#define serialFdH
//---------------------------------------------------------------------------

#include <time.h>
#include <stdio.h>
#include "mk_std.h"
#include "serialProtocol.h"

using namespace System;
using namespace System::IO::Ports; // added by Randy for serial object

enum
{
   MSG_IGNORED,                /* message ignored, incorrect address */
   ACK1,                       /* a reply: indicating un-corrupted message rx'ed */
   ACK2,                       /* a reply: msg rx'ed and successfully processed*/
   NACK1,                      /* a reply indicating corrupted message detected */
   NACK2,                      /* a reply indicating good message, processing failed */
   LAST_LINK_LAYER             /* marker for last enum value used for link-layer,as these */
                                 /* must not be used by application command codes  */

};

//---------------------------------------------------------------------------
ref class TSerialCom
{

private:	// User declarations

public:		// User declarations

   /*****************************************************************************/
   /* D E F I N I T I O N S                                                     */
   /*****************************************************************************/
   /* stuff to enable debug printing for this module */
   #define MAX_SERIAL_PORTS ((UINT8)2)
   #define DEBUG_SERIAL_TX(message)  /**/ //(Application->MessageBox(message, "Debug", MB_OK ))
   #define DEBUG_SERIAL_RX(message)  /**/ //(Application->MessageBox(message, "Debug", MB_OK ))
   #define DEBUG_SERIAL(message)     /**/ //(Application->MessageBox(message, "Debug", MB_OK ))
   
   /* byte positions of the fields in all messages */
   #define SERIAL_HEADER_INDEX ((UINT8)0)
   #define SERIAL_TYPE_INDEX ((UINT8)1)
   #define SERIAL_BYTES_TO_FOLLOW_MSB_INDEX ((UINT8)2)
   #define SERIAL_BYTES_TO_FOLLOW_LSB_INDEX ((UINT8)3)

   /* the size of fields in BPS messages */
   #define SIZE_OF_HEADER ((UINT8)1)
   #define SIZE_OF_MSG_TYPE ((UINT8)1)
   #define SIZE_OF_BYTES_TO_FOLLOW ((UINT8)2)
   #define SIZE_OF_CRC_VALUE ((UINT8)2)
   #define SIZE_OF_OVER_HEAD (SIZE_OF_HEADER+SIZE_OF_MSG_TYPE+SIZE_OF_BYTES_TO_FOLLOW)

   /* The structure of BPS messages */
   #define HEADER_BYTE_INDEX         (SERIAL_HEADER_INDEX)
   #define MESSAGE_TYPE_INDEX        (SERIAL_TYPE_INDEX)
   #define BYTES_TO_FOLLOW_MSB_INDEX (SERIAL_BYTES_TO_FOLLOW_MSB_INDEX))
   #define BYTES_TO_FOLLOW_LSB_INDEX (SERIAL_BYTES_TO_FOLLOW_LSB_INDEX))

   #define ADAPTER_ADDRESS_INDEX     (SERIAL_BYTES_TO_FOLLOW_LSB_INDEX+1)
   #define COMMAND_CODE_INDEX        (SERIAL_BYTES_TO_FOLLOW_LSB_INDEX+2)
   /* ...databytes associated with each command code is defined in */
   /* serialProtocol.h */

   /* Message Protocol: Used for setting up the physical layer's */
   /* activity, fullDuplex, and physicalLayer characteristics */
   #define SERIAL_RX_BIT_MASK ((UINT8)0x01)
   #define SERIAL_TX_BIT_MASK ((UINT8)0x02)
   #define RS232 ((UINT8)0x00)
   #define RS485 ((UINT8)0x01)
   #define R485_TX_ENABLE  serialPorts[portIndex_]->RtsEnable = true //(ls->Rts(GCPP_ON ))
   #define R485_TX_DISABLE serialPorts[portIndex_]->RtsEnable = false //(ls->Rts(GCPP_OFF))
   #define R485_RX_ENABLE /* */
   #define R485_RX_DISABLE /* */

   /* types of messages supported: 8, 16, or 32 bit data */
   #define SERIAL_MSG_TYPE8  ((UINT8)8)
   #define SERIAL_MSG_TYPE16 ((UINT8)16)
   #define SERIAL_MSG_TYPE32 ((UINT8)32)


   /* an rx sequence is terminated if this amount of time passes by without */
   /* next character being received; units are milliseconds */
   #define MAX_MSG_TIME ((UINT32) 1000)

   /* the number of bytes to receive per iternation of the state machine */
   #define RX_BYTES_PER_CYCLE ((UINT16)10)

   /* define the Tx and Rx message structures used */
   #define SIZE_OF_HEADER ((UINT8)1)
   #define SIZE_OF_MSG_TYPE ((UINT8)1)
   #define SIZE_OF_BYTES_TO_FOLLOW ((UINT8)2)
   #define SIZE_OF_CRC_VALUE ((UINT8)2)
   #define SIZE_OF_OVER_HEAD (SIZE_OF_HEADER+SIZE_OF_MSG_TYPE+SIZE_OF_BYTES_TO_FOLLOW)


   /* Rx message characteristics */
//   #define MAX_RX_MSG_LENGTH ((UINT16)100) /* length excludes all overhead bytes */
   #define MAX_RX_BUF_BYTES ((MAX_RX_MSG_LENGTH + SIZE_OF_OVER_HEAD) * 2) /* size of buffers to store rx msgs */
   #define RX_MSG_HEADER ((char)'#') /* header byte value for rx messages */

   /* Tx message characteristics */
//   #define MAX_TX_MSG_LENGTH (MAX_RX_MSG_LENGTH) /* length excludes all overhead bytes */
   #define MAX_TX_BUF_BYTES ((MAX_TX_MSG_LENGTH + SIZE_OF_OVER_HEAD) * 2)
   #define TX_MSG_HEADER ((char)'$') /* header byte value added to tx messages */


   /* Hooks to the HardwareInterface routines */
   #define SERIAL_PORT_GOT(portNumber, portSettings) (InitSerialHw(8000000, portSettings))
   #define SERIAL_PORT_FLUSH(portNumber) /**/
   #define SERIAL_DATA_RECEIVED(portNumber, rxBytePtr) (GetByte(rxBytePtr))
   #define SERIAL_PORT_RELEASE(portNumber)  ((unsigned char)0)
   #define SERIAL_DATA_SENT(portNumber, aByte) (SendByte(aByte))
   #define CURRENT_SYSTEM_TIME  (TicksLeft)

   /*****************************************************************************/
   /* T Y P E    D E F I N I T I O N S                                          */
   /*****************************************************************************/
   /* structure used to hold tx state machine context */
   
   //* Structure for serial com port settings */             
   //typedef struct
   ref class COM_SETTINGS
   {
   public:
      bool fullDuplex;    /* full or half duplex                */
      UINT8 physicalLayer;/* RS232 or RS485                     */
      UINT8 activity;     /* bitfields for TX, RX...            */
   }; //COM_SETTINGS;

   //typedef ref struct
   ref class TX_CONTEXT
   {
   public:
      void (* txFailNotifyPtr)(void);
      //UINT8 txBuf[MAX_TX_BUF_BYTES];
      UINT8* txBuf;
      int SerialTxMachState;
   };//TX_CONTEXT;
      /* used to keeping track of serial module's performance */

#ifdef randy
   ref struct
   {
      UINT32 txCount; /* counts number of message successfully queued for tx */
      UINT32 rxCount; /* counts number of messages rx'ed with good CRC */
      UINT32 rxCrcErrors; /* counts num of msgs that had CRC failures */
      UINT32 rxDiscardedMsgs; /* num of msgs that had ok header but then subsequently failed */
      UINT32 rxHwOverRuns; /* this comes from HI layer, not implemented yet */
      UINT32 rxBufferOverRuns; /* this comes from HI layer, not implemented yet */
   }SerialComStats;
#endif

   typedef UINT16 CRC_VALUE;

   /***** Original Implementation *****/

   //COM_SETTINGS * ComPortPtr[MAX_SERIAL_PORTS ];
   //int TxCheckSum[MAX_SERIAL_PORTS ];
   //int RxCheckSum[MAX_SERIAL_PORTS ];
   //bool fPortActive[MAX_SERIAL_PORTS];

   /* structures to hold context of each serial port. */
   //TX_CONTEXT TxContext[MAX_SERIAL_PORTS];

   /***** modified by Randy, had to do this to get around the managed/unmanaged BS   *****/
   int* TxCheckSum;
   int* RxCheckSum;
   bool* fPortActive;

   array<COM_SETTINGS^>^ ComPortPtr;
   array<TX_CONTEXT^>^ TxContext;
   array<SerialPort^>^ serialPorts; // Visual C++ Serial Port object

   /***************************************************************************/
   /* F U N C T I O N    P R O T O T Y P E S                                  */
   /***************************************************************************/
   TSerialCom(); // constructor
   ~TSerialCom(); // destructor
   bool  OpenPort  ( unsigned char portIndex_, SerialPort^ serialPort_ );
   bool  ClosePort ( unsigned char portIndex_ );
   bool  SetPort   ( unsigned char portIndex_, int baudRate_ );
   bool  FlushPort ( unsigned char portIndex_ );
   bool  SendByte  ( unsigned char portIndex_, char aByte_, bool internal_ );
   bool  GetByte   ( UINT8 portIndex_, int timeOut_, UINT8 *rxBytePtr_ );
   bool  SendEcho  ( unsigned char portIndex_, char aByte_ );
   //void  commError ( int e_level, int sender );

   /*
   *|----------------------------------------------------------------------------
   *|  Module: Serial Communications
   *|  Routine: SendMessage
   *|  Description:
   *|   Serial communications transmission request.  This routine will
   *|   send ALL bytes in one shot (ie its blocking!).  If half-duplex
   *|   operation is configured, this routine will not transmit if data
   *|   reception is currently under way.  The protocol requires that all the
   *|   data fields be of the same length: 8, 16, or 32 bits in length.
   *|---------------------------------------------------------------------------
   */
   bool SendMessage                   /*| true if tx succeeded, else false    */
   (                                  /*|                                     */
      UINT8 msgType_,                 /*| SERIAL_MSG_TYPE8,16,32              */
      UINT8 portIndex_,               /*| index to identify serial port       */
      UINT16 numFields_,              /*| number of data fields be tx'ed      */
      UINT8 * txBufPtr_,              /*| pointer to buffer with data         */
      void(*txFailNotifyPtr_)(void)   /*| optional callback if tx fails       */
   );


   /*
   *|----------------------------------------------------------------------------
   *|  Module: Serial Communications
   *|  Routine: ReceiveMessage
   *|  Description:
   *|   Poll serial port for incoming data bytes and build up a message.
   *|   This routine will receive all bytes in one shot (ie the longer
   *|   the reception, the greater the amount of time this routine BLOCKS!).
   *|   If half-duplex
   *|   operation is configured, this routine will not receive if data
   *|   transmission is currently under way.
   *|
   *|   The protocol requires that all data fields in a message be of the
   *|   same length: 8, 16, or 32 bits.  When data is received, the data fields
   *|   must be extracted from the receive buffer using the GetField().
   *|----------------------------------------------------------------------------
   */
   bool ReceiveMessage    /*| 'true' if message avail, 'false' otherwise      */
   (                      /*|                                                 */
      UINT8 portIndex_,   /*| the serial port to poll                         */
      UINT8 * rxBufPtr_   /*| the location to store entire rx'ed bytes of msg */
   );                     /*|                                                 */



   /*
   *|----------------------------------------------------------------------------
   *|  Module: Serial Communications
   *|  Routine: CalcCrc
   *|  Description:
   *|   Calculates CRC on a buffer of data.
   *|----------------------------------------------------------------------------
   */
   CRC_VALUE CalcCrc                  /*| returns CRC value            */
   (                                  /*|                              */
      UINT8 *dataBufPtr_,             /*|                              */
      //array<UINT8>^ dataBufPtr_,
      UINT8 numBytes_                 /*|                              */
   );                                 /*|                              */


   /*
   *|----------------------------------------------------------------------------
   *|  Module: serial com
   *|  Routine: AppendTxBuf
   *|  Description:
   *|   Converts a 8, 16, or 32 bit data into fixed format structure of 1, 2, or
   *|   or 4 bytes.  Called sequentially, the return value can be used as the
   *|   sinkOffSet_ in order to append data to the sink buffer
   *|----------------------------------------------------------------------------
   */
   UINT8 AppendTxBuf
   (
      UINT8 *sinkPtr_,
      UINT8 sinkOffSet_,
      UINT32 sourcePtr_,
      UINT8 messageType_
   );



   /*
   *|----------------------------------------------------------------------------
   *|  Module: Application
   *|  Routine: GetField
   *|  Description:
   *|   Description:
   *|   Gets the specified BPS field from a buffer transmitted and
   *|   received using the SendMessage() format
   *|----------------------------------------------------------------------------
   */
   UINT32 GetField         /*| return the requested value as a UINT32 */
   (                       /*|                                        */
      UINT8 *dataBufPtr_,  /*| complete rx buffer                     */
      UINT16 fieldIndex_   /*| specify the desired field              */
   );                      /*|                                        */
}; // end of Class delcaration of TSerialCom
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#endif
