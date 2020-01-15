/*|***************************************************************************/
/*|PROJECT:  Visual C++ Serial Communications Implementation                 */
/*|Module:   Serial communications: Function Driver layer                    */
/*|Description:                                                              */
/*|   This module implements serial communications routines at the function  */
/*|   driver layer.
/*|   Public members:
/*|   ComPortPtr - pointers to a COM_SETTINGS structure that defines the
/*|      characteristics for this port.
/*|   TxCheckSum - stores the checksum of data transmitted through the object.
/*|      This checksum must be initialised external to these routines.
/*|   RxCheckSum - stores the checsum of data received thru the object
/*|      This checksum must be initialised external to these routines.
/*|
/*|
/*|   The underlying routines are from the Greenleaf library package GFCOMM++
/*|   so their "cpfw.lib" file and "compplib.h" file must be added to the
/*|   Borland C++Builder project file list!
/*|****************************************************************************
*/

/*
Tools:
   Compiler: Visual C++ Express Edition 2010
   ***************************************************************************
   Copyright (C) 2010, Micrologic Limited.
   Any information contained in this document which was hitherto unknown by
   the recipient or in which there subsists a proprietary right must not be
   disclosed to any other person, nor used for manufacturing purposes,
   without the permission of Micrologic Limited and the acceptance of this
   document will be construed as acceptance of the foregoing.
   ***************************************************************************
*/
//---------------------------------------------------------------------------

#include "StdAfx.h"
#include "serialFd.h"

//#define TYPE_RS485 1

using namespace msclr::interop;; // added by Randy (needed for Marshal, not sure what that is though...)
using namespace System::Windows::Forms; // Needed for message box pop ups

//---------------------------------------------------------------------------
//TSerialCom ^SerialCom;
//---------------------------------------------------------------------------
TSerialCom::TSerialCom()
{
   // The constructor was added to dynamically create the necessary objects in the GC Heap
   serialPorts = gcnew array<SerialPort^>(MAX_SERIAL_PORTS);
   ComPortPtr = gcnew array<COM_SETTINGS^>(MAX_SERIAL_PORTS);
   TxContext = gcnew array<TX_CONTEXT^>(MAX_SERIAL_PORTS);

   TxCheckSum = new int[MAX_SERIAL_PORTS];
   RxCheckSum = new int[MAX_SERIAL_PORTS];
   fPortActive = new bool[MAX_SERIAL_PORTS];

   for(int portIndex=0; portIndex<MAX_SERIAL_PORTS; portIndex++)
   {
      fPortActive[portIndex] = false;

      // This is kind of weird how Visual C++ does this. You have to dynamically create the array
      // and then each element you have to dynamically create a new element
      ComPortPtr[portIndex] = gcnew COM_SETTINGS;
      serialPorts[portIndex] = gcnew SerialPort;
      TxContext[portIndex] = gcnew TX_CONTEXT;

      TxContext[portIndex]->txBuf = new UINT8[MAX_TX_BUF_BYTES];
   }
}
//---------------------------------------------------------------------------
TSerialCom::~TSerialCom()
{
   // Destructor is added to deallocate the dynamic memory
   delete[] TxCheckSum;
   delete[] RxCheckSum;
   delete[] fPortActive;

   for(int portIndex=0; portIndex<MAX_SERIAL_PORTS; portIndex++)
   {
      delete[] TxContext[portIndex]->txBuf;
   }
}
//---------------------------------------------------------------------------
/*
*****************************************************************************
 P R O C E D U R E S
*****************************************************************************
*/

/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialBase                                                      */
/*|  Routine: OpenPort                                                       */
/*|  Description:  Creates the serial port object (only one is permitted, if */
/*|  one already exists, it is destroyed) using the public member ComPortPtr */
/*|  for setting up the port characteristics                                 */
/*|--------------------------------------------------------------------------*/
bool TSerialCom::OpenPort  /*| true if successful */
(
   unsigned char portIndex_,   /*| identifies which port object to open */
   SerialPort^ serialPort_
)
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/

   /***********************/
   /* C O D E             */
   /***********************/
   /* If the structure has not been defined use the following default values  */
   if ( fPortActive[ portIndex_ ] )
   {  /* port already exists, shut it down before redefining it */
      ClosePort( portIndex_);
   }

   serialPorts[portIndex_] = serialPort_;

   /* try to get the serial port */
   try
   {
      serialPorts[portIndex_]->Open();
   }
   catch(UnauthorizedAccessException^)
   {
      MessageBox::Show( "Error", "COM Port Could Not Be Opened" );

      return false;
   }

   /* set initial state of DTR and RTS lines */
   /* set init state of RS485 TX_EN and RX_EN: default to RX always */
   /* as tranmissions will control the TX_EN as needed */
   #ifdef TYPE_RS485
      R485_TX_DISABLE;
      R485_RX_ENABLE;
   #else
      serialPorts[portIndex_]->DtrEnable = false;
      serialPorts[portIndex_]->RtsEnable = false;
   #endif

   /* mark port as existent */
   fPortActive[ portIndex_ ] = true;
   Sleep(200);

   return true;
   
}
/* OpenPort() */
//---------------------------------------------------------------------------


/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialComm                                                      */
/*|  Routine: SetPort                                                        */
/*|  Description:  Changes the baudrate of the serial port object without any*/
/*|  changing of the DTR and RTS control line state.  The other port         */
/*|  characteristics are set up as pointed to by the class member ComPortPtr */
/*|                                                                          */
/*|--------------------------------------------------------------------------*/
bool TSerialCom:: SetPort     //*| true if successful
(                 
   unsigned char portIndex_, //* identifies the port to set up */
   int baudRate_  //*| the desired baudrate of the serial port
)
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/
   

   /***********************/
   /* C O D E             */
   /***********************/
 
   serialPorts[portIndex_]->BaudRate = baudRate_;

   return true;

}//* end setCommunications() */
//---------------------------------------------------------------------------



/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialBase                                                      */
/*|  Routine: ClosePort                                                      */
/*|  Description:  Closes the communications port object                     */
/*|                                                                          */
/*|--------------------------------------------------------------------------*/
bool TSerialCom:: ClosePort              /*| true if successful */
(                            /*|                    */
   unsigned char portIndex_  /*|                    */
)                            /*|                    */
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/

   /***********************/
   /* C O D E             */
   /***********************/
   if ( fPortActive[ portIndex_ ] )
   {
      /* the serial port object exists, shut it down */
      fPortActive[ portIndex_ ] = false;
      serialPorts[ portIndex_ ]->Close();
   }
   return true;
} /* ClosePort */

/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialComm                                                      */
/*|  Routine: SendByte                                                       */
/*|  Description: Sends a single byte out the comport                        */
/*|   If RS485 operation is selected, this routine allows user to specify    */
/*|   whether or not the tx and rx buffers should be enabled inside this     */
/*|   routine or if they are set already.
/*|   If "send" was ok, the transmit checksum (TxCheckSum) is updated        */
/*|                                                                          */
/*|--------------------------------------------------------------------------*/
bool TSerialCom:: SendByte   //*| false if error occurred during send, else true
(                            //*|
   unsigned char portIndex_, //*| identifies the port
   char aByte_,              //*| the character to be sent out
   bool internal_            //*| used only for rs485 mode, allows the tx and
)                            //*| rx buffers to be twiddled in here or not
                             //*| 'true' allows means twiddling is done here
                             //*| 'false' means tx/rx buffers is to be assumed
                             //*|  as already set up correctly
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/

   /***********************/
   /* C O D E             */
   /***********************/
   if(fPortActive[ portIndex_ ] == true)
   {
      #ifdef TYPE_RS485
         if( physicalLayer == RS485 ) && (internal_) )
         {
            R485_TX_ENABLE;
            R485_RX_DISABLE;
         }
      #endif
      //* serial port object exists, proceed with transmission */
      try
      {
         array<Byte>^ sendBuff = gcnew array<Byte>(2);

         #ifdef TYPE_RS485
            if( physicalLayer == RS485 ) && (internal_) )
            {
               R485_TX_DISABLE;
               R485_RX_ENABLE;
            }
         #endif
         sendBuff[0] = (Byte)aByte_;
         serialPorts[ portIndex_ ]->Write(sendBuff, 0 , 1);
         TxCheckSum[ portIndex_ ] += aByte_;
      }
      catch (InvalidOperationException ^)
      {
         // transmission failed
         #ifdef TYPE_RS485
            if( physicalLayer == RS485 ) && (internal_) )
            {
               R485_TX_DISABLE;
               R485_RX_ENABLE;
            }
         #endif

         DEBUG_SERIAL_TX("SendByte: transmission error!");
         return false;
      }

      // transmission is successful
      return true;
   }
   else
   {
      DEBUG_SERIAL_TX("SendByte: Com Port not activated!");
      return false;
   }
}/* SendByte() */
//---------------------------------------------------------------------------



/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialComm                                                      */
/*|  Routine: GetByte                                                        */
/*|  Description: Wait on the serial port for a specified duration for an    */
/*|   incoming character. The received data checksum, RxChecksum, is updated */
/*|   if a character is found during this window.                            */
/*|--------------------------------------------------------------------------*/
bool TSerialCom:: GetByte    /*| 'true' if char rx'ed, 'false' otherwise         */
(                        /*|                                                 */
   UINT8 portIndex_ ,    /*| identifies the port                             */
   int timeOut_,         /*| duration to wait, in milliseconds               */
   UINT8 *rxBytePtr_     /*| the data received is stored here                */
)                        /*|                                                 */
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/

   Byte status;
   int rxTimeOutBackup;

//   struct  time t;


   /***********************/
   /* C O D E             */
   /***********************/
   if( rxBytePtr_ == NULL_PTR )
   {
      return false;
   }

   if(fPortActive[ portIndex_ ] == true)
   {
      /* serial port object exists, proceed with rx */
      rxTimeOutBackup = serialPorts[portIndex_]->ReadTimeout;
      serialPorts[portIndex_]->ReadTimeout = timeOut_;

      try
      {
         // NOTE: ReadByte is a blocking call!
         status = serialPorts[ portIndex_ ]->ReadByte();
      }
      catch (TimeoutException ^)
      {
         // Timed out
         *rxBytePtr_  = 0;
         serialPorts[portIndex_]->ReadTimeout = rxTimeOutBackup;
         return false;
      }

      *rxBytePtr_ = (UINT8)status;
      RxCheckSum[ portIndex_ ] += *rxBytePtr_;
      serialPorts[portIndex_]->ReadTimeout = rxTimeOutBackup;

      /* return the received byte */
      return true;
   }
   else
   {
      /* the serial port object does not exist */
     *rxBytePtr_ = 0;
     return false;
   }
}/* end GetByte */
//---------------------------------------------------------------------------


/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialBase                                                      */
/*|  Routine: FlushPort                                                       */
/*|  Description:  Empty the Tx and Rx buffers of the serial port object     */
/*|                                                                          */
/*|--------------------------------------------------------------------------*/
bool TSerialCom:: FlushPort
(
   unsigned char portIndex_  //|* identifies the port
)
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/

   /***********************/
   /* C O D E             */
   /***********************/

   // Not sure if there is a flush port type routine available in the serial object


   return true;
} /* FlushPort() */
//---------------------------------------------------------------------------



/*|--------------------------------------------------------------------------*/
/*|                                                                          */
/*|  Module: serialComm                                                      */
/*|  Routine: SendEcho                                                       */
/*|  Description:  Sends a byte and waits a limited time for the byte to be  */
/*|     echoed back by the device on the other end of the serial port        */
/*|                                                                          */
/*|--------------------------------------------------------------------------*/
bool TSerialCom:: SendEcho    /*| true if valid echo received  */
(             
   unsigned char portIndex_ , /*| identifies the port         */
   char aByte_                /*| the byte to be transmitted   */
)              
{
   /***********************/
   /* L O C A L   D A T A */
   /***********************/
   UINT8 rxByte;

   /***********************/
   /* C O D E             */
   /***********************/
   /* try to send the character */
   if( SendByte(portIndex_, aByte_, false) )
   {
      /* tx suceeded, wait 100 ms to see if the other end of serial port */
      /* sends back the echoed character */
      GetByte(portIndex_, 100, &rxByte);
      if( rxByte== aByte_)
      {
         /* valid echo received */
         return true;
      }
      else
      {
         /* invalid echo or no echo received in time allotted */
         return false;
      }
   }
   else
   {
      /* could not send byte */
      return false;
   }
} /* SendEcho() */
//---------------------------------------------------------------------------


/*
*|----------------------------------------------------------------------------
*|  Module: Serial Communications
*|  Routine: SendMessage
*|  Description:
*|   Serial communications transmission request.
*|----------------------------------------------------------------------------
*/
bool TSerialCom:: SendMessage
(
   UINT8 msgType_,
   UINT8 portIndex_,
   UINT16 numFields_,
   UINT8 * txBufPtr_,
   void(*txFailNotifyPtr_)(void)
)
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */
   int i, j;
   CRC_VALUE tempCrc;
   UINT16 numBytes;
   UINT8 tempBuf[MAX_RX_MSG_LENGTH];

   /*
   *************************
    E R R O R   C H E C K S
   *************************
   */                             

   if( (!ComPortPtr[ portIndex_ ]->fullDuplex ) )
   {
      /* half-duplex operation: */
      if( CHECK(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK) )
      {
         /* reception is in-progress, don't  allow transmission */
         DEBUG_SERIAL_TX("Can't transmit while receiving in half-duplex");
         return false;
      }
      else
      {
         /* get activity 'semaphore' */
         SET(ComPortPtr[ portIndex_ ]->activity, SERIAL_TX_BIT_MASK);
      }
   }

   switch( msgType_ )
   {
      case SERIAL_MSG_TYPE8:
         numBytes = numFields_;
         break;

      case SERIAL_MSG_TYPE16:
         numBytes = numFields_ <<1;
         break;

      case SERIAL_MSG_TYPE32:
         numBytes = numFields_<<2;
         break;

      default:
         /* not allowed */
         DEBUG_SERIAL_TX("SendMessage: Invalid arguments");
         UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_TX_BIT_MASK);
         return false;
   }

   if( numBytes > MAX_TX_MSG_LENGTH )
   {
      /* not allowed */
      DEBUG_SERIAL_TX("SendMessage: too many bytes to send");
      UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_TX_BIT_MASK);
      return false;
   }

   /*
   *************************
    C O D E
   *************************
   */
   {
      /* remember the tx failed callback routine */
      TxContext[portIndex_]->txFailNotifyPtr = txFailNotifyPtr_;

      /* build up the actual tx bytes into Tx Buffer... */
      /* set the header bytes */
      TxContext[portIndex_]->txBuf[SERIAL_HEADER_INDEX] = TX_MSG_HEADER;

      /* set the message type */
      TxContext[portIndex_]->txBuf[SERIAL_TYPE_INDEX] = msgType_;

      /* set the MSbyte of bytesToFollow of buffer*/
      TxContext[portIndex_]->txBuf[SERIAL_BYTES_TO_FOLLOW_MSB_INDEX] =
         (UINT8)((numBytes + SIZE_OF_CRC_VALUE)>>8);

      /* set the LSbyte of bytesToFollow of buffer*/
      TxContext[portIndex_]->txBuf[SERIAL_BYTES_TO_FOLLOW_LSB_INDEX] =
         (UINT8)((numBytes + SIZE_OF_CRC_VALUE) & 0x00FF );

      /* get data bytes of the real message */
      for( (i=SIZE_OF_HEADER + SIZE_OF_BYTES_TO_FOLLOW +1) && (j=0);
           j < numBytes;
           i++ && j++
         )
      {
         TxContext[portIndex_]->txBuf[i] = txBufPtr_[j];
      }

      /* get the CRC of entire buffer except the CRC bytes*/
      tempCrc = CalcCrc( &TxContext[portIndex_]->txBuf[SERIAL_HEADER_INDEX],
                          numBytes + SIZE_OF_HEADER + SIZE_OF_MSG_TYPE + SIZE_OF_BYTES_TO_FOLLOW
                       );

      TxContext[portIndex_]->txBuf[i] = (UINT8)(tempCrc>>8); /* MSbyte */
      TxContext[portIndex_]->txBuf[i+1]=(UINT8)(tempCrc & 0x00FF); /*LSbyte */
      numBytes = i+2;

      /* indicate that task was accepted */
      DEBUG_SERIAL_TX("SendMessage: Tx message queued for send!");

      /* count messages transmitted for statistics */
      //SerialComStats.txCount++;

      /* transmit the buffer now */
      R485_TX_ENABLE;

      for( j=SERIAL_HEADER_INDEX; j<numBytes; j++)
      {
         if( !SendByte( portIndex_, TxContext[portIndex_]->txBuf[j], false ) )
         {
            /* error sending byte */
            UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_TX_BIT_MASK);
            DEBUG_SERIAL_TX("SendMessage: Transmission error!");
            R485_TX_DISABLE;

            return false;
         }
      }

      SendByte( portIndex_, 0xaa, false );

      R485_TX_DISABLE;

      UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_TX_BIT_MASK);

      if( ComPortPtr[portIndex_]->physicalLayer == RS485 )
      {
         /* get echo and discard it (or verify it?) */
         ReceiveMessage( 0, tempBuf );
      }
      return true;
   }

}/* SendMessage() */



/*
*|----------------------------------------------------------------------------
*|  Module: Serial Communications
*|  Routine: ReceiveMessage
*|  Description:
*|   Serial communications receive.  In order to use this routine, first
*|   open the portIndex with the desired COM_SETTINGS.  If half-duplex
*|   operation is configured, data reception is prevented when transmission
*|   is under way.
*|----------------------------------------------------------------------------
*/
bool TSerialCom:: ReceiveMessage
(
   UINT8 portIndex_,
   UINT8 * rxBufPtr_
)
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */
   int i;
   CRC_VALUE tempCrc;
   UINT8 rxByte;
   UINT16 numBytes;
   UINT8 * tempPtr;
   UINT8 status;

   /*
   *************************
    E R R O R   C H E C K S
   *************************
   */


   if( rxBufPtr_ == NULL_PTR )
   {
      return false;
   }
   status = 0;

   if( (!ComPortPtr[ portIndex_ ]->fullDuplex ) )
   {
      /* half-duplex operation: */
      if( CHECK(ComPortPtr[ portIndex_ ]->activity, SERIAL_TX_BIT_MASK) )
      {
         /* transmission is in-progress, don't  allow reception */
         DEBUG_SERIAL_RX("Can't receive while transmitting in half-duplex");
         return false;
      }
      else
      {
         /* get activity 'semaphore' */
         SET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
      }
   }

   tempPtr = rxBufPtr_;

   /*
   *************************
    C O D E
   *************************
   */

   /* wait up to 100 milliseconds for header byte */
   /* changed to 25mSec July 6/01                 */
   //if( !SerialCom->GetByte(portIndex_, 25, &rxByte)) // original
   if( !GetByte(portIndex_, 50, &rxByte)) // original
   {
      /* no data found at all */
      UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
      DEBUG_SERIAL_RX("No incoming data detected in time!");
      return false;
   }

   /* search for header bytes */
   i= 0;
   while( (rxByte != RX_MSG_HEADER ) && (rxByte != TX_MSG_HEADER ) )
   {
      /* keep searching until no rx bytes or hard-limit reached */
      i++;
      if( (i>MAX_RX_MSG_LENGTH)  || (!GetByte(portIndex_, 100, &rxByte)) //100, &rxByte))
        )
      {
         /* can't find header byte */
         DEBUG_SERIAL_RX("Can't Find Header Byte!");
         UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
         return false;
      }
      // sleep slows down this tight while loop, releasing some CPU usage
      Sleep(1);
   }

   if( (rxByte != RX_MSG_HEADER ) && (rxByte != TX_MSG_HEADER )
     )
   {
      /* corruption, can retry but I'll  just quit */
      DEBUG_SERIAL_RX("Corrupted header byte!");
      UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
      return false;
   }


   /* save header byte to rx buffer */
   *(rxBufPtr_++) = rxByte;
   status = rxByte;

   /* get the msg-type byte and the 2 bytes-to-follow bytes */
   for( i=0; i<3; i++ )
   {
      /* wait up to 10 milliseconds for next byte */
      if( !(GetByte(portIndex_, 10, &rxByte)) )
      {
         /* no data found at all */
         UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
         DEBUG_SERIAL_RX("Waited too long for msg header data!");
         return false;
      }

      /* save byte to rx buffer */
      *(rxBufPtr_++) = rxByte;
   }

   /* calculate the bytes-to-follow, verify it and use it to get bytes */
   numBytes = ((*(rxBufPtr_-2))<<8) +  *(rxBufPtr_-1);

   if( numBytes > MAX_RX_MSG_LENGTH )
   {
      UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
      DEBUG_SERIAL_RX("Incoming message's bytecount field is too high!");
      return false;
   }

   for( i= 0; i<numBytes; i++)
   {
      /* wait up to 30 milliseconds for next byte */
      /* If serial CRC errors are happening too often, it may be */
      /* caused by delays in Windows: increase this value to fix  it*/
      //if( !(SerialCom->GetByte(portIndex_, 30, &rxByte)) ) original commented out by Randy
      if( !(GetByte(portIndex_, 60, &rxByte)) ) 
      {
         /* no more data found  */
         UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
         DEBUG_SERIAL_RX("Waited too long for msg data!");
         return false;
      }

      /* save byte to rx buffer */
      *(rxBufPtr_++) = rxByte;
   }

   /*! verify CRC */
   UNSET(ComPortPtr[ portIndex_ ]->activity, SERIAL_RX_BIT_MASK);
   tempCrc = CalcCrc( tempPtr, SIZE_OF_OVER_HEAD+numBytes-SIZE_OF_CRC_VALUE );
   if( tempCrc == ( (UINT16)(*(rxBufPtr_-2))<<8 ) + (*(rxBufPtr_-1) & 0x00FF) )
   {
      /* crc Ok */
      DEBUG_SERIAL_RX("CRC verified");
      if( status == RX_MSG_HEADER)
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      DEBUG_SERIAL_RX("CRC failed");
      return false;
   }


   return true;


}/* ReceiveMessage() */


/*
*|----------------------------------------------------------------------------
*|  Module: Serial Communications
*|  Routine: CalcCrc
*|  Description:
*|   Calculates CRC on a buffer of data.
*|----------------------------------------------------------------------------
*/
UINT16 TSerialCom::CalcCrc                  /*| returns CRC value            */
(                                  /*|                              */
   UINT8 *dataBufPtr_,             /*|                              */
   UINT8 numBytes_                 /*|                              */
)                                  /*|                              */
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */
   int i;
   int x;
   unsigned int crc, feed;
   unsigned char *tempPtr;


   /*
   *************************
    E R R O R   C H E C K S
   *************************
   */
   /* Check for uninitialized pointers, out of bounds inputs, etc */


   /*
   *************************
    C O D E
   *************************
   */

   crc = 0;
   for ( tempPtr = dataBufPtr_;
         tempPtr < (dataBufPtr_ + numBytes_);
         tempPtr++
       )
   {
      feed = (unsigned int)(*tempPtr);
      for ( i=0; i<8; i++)
      {
         x = crc & 0x0001;
         crc= crc>>1;
         if ( feed & 0x0001 )
         {
            crc = crc | 0x8000;
         }
         feed = feed>>1;
         if ( x )
         {
            crc = crc ^ 0x1021;
         }
      }
   }
   return crc;

}/* end CalcCrc() */

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
UINT8 TSerialCom::AppendTxBuf
(
   UINT8 *sinkPtr_,
   UINT8 sinkOffSet_,
   UINT32 sourcePtr_,
   UINT8 messageType_
)
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */
   UINT8 i;


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
   switch( messageType_ )
   {
      case SERIAL_MSG_TYPE8:
         *(sinkPtr_+ sinkOffSet_) = (UINT8)sourcePtr_;
         i=1;
         break;

      case SERIAL_MSG_TYPE16:
         *(sinkPtr_+ sinkOffSet_+ 0) = (UINT8)(sourcePtr_>>8);
         *(sinkPtr_+ sinkOffSet_+ 1) = (UINT8)(sourcePtr_>>0);
         i=2;
         break;
               
      case SERIAL_MSG_TYPE32:
         *(sinkPtr_+ sinkOffSet_+ 0) = (UINT8)(sourcePtr_>>24);
         *(sinkPtr_+ sinkOffSet_+ 1) = (UINT8)(sourcePtr_>>16);
         *(sinkPtr_+ sinkOffSet_+ 2) = (UINT8)(sourcePtr_>> 8);
         *(sinkPtr_+ sinkOffSet_+ 3) = (UINT8)(sourcePtr_>> 0);
         i=4;
         break;
   }

   return sinkOffSet_ + i;
}/* AppendTxBuf() */

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
UINT32 TSerialCom::GetField         /*| return the requested value as a UINT32 */
(                       /*|                                        */
   UINT8 *dataBufPtr_,  /*| complete rx buffer                     */
   UINT16 fieldIndex_   /*| specify the desired field              */
)                       /*|                                        */
{
   /*
   ***********************
    L O C A L   D A T A
   ***********************
   */
   UINT32 temp32;
   UINT8 *tempPtr;



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
   switch( dataBufPtr_[SERIAL_TYPE_INDEX] )
   {
      case SERIAL_MSG_TYPE8:
         /* pointer is already correct for this format */
         
         /* get the byte */ 
         temp32 = (UINT32)((UINT8 *)dataBufPtr_)[fieldIndex_];
         break;

      case SERIAL_MSG_TYPE16:
         /* point to first (MSB) of required field */
         tempPtr = (UINT8 *)
                   &( (UINT16 *)(dataBufPtr_+SIZE_OF_OVER_HEAD))[fieldIndex_-SIZE_OF_OVER_HEAD];

         /* build MSB */
         temp32 = (*tempPtr)<<8;

         /* build LSB */
         temp32 += *(tempPtr+1);
         break;

      case SERIAL_MSG_TYPE32:
         /* point to first (MSB) of required field */
         tempPtr = (UINT8 *)
                   &( (UINT32 *)(dataBufPtr_+SIZE_OF_OVER_HEAD))[fieldIndex_-SIZE_OF_OVER_HEAD];

         /* build MSB */
         temp32 =  (*tempPtr+0)<<24;

         /* build next byte */
         temp32 += *(tempPtr+1)<<16;

         /* build next byte */
         temp32 += *(tempPtr+2)<<8;

         /* build LSB */
         temp32 += *(tempPtr+3);
         break;
   }/* switch: on message type */
   return temp32;
}/* GetField() */
//---------------------------------------------------------------------------



