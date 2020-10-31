using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace SerialCom
{
   public class SerialFd
   {
      
      enum SERIAL_DEF
      {
         PORTS =32,
         MAX_TX_BUF_BYTES =100,
         MAX_RX_MSG_LENGTH =100,

         SERIAL_HEADER_INDEX =0,
         SERIAL_TYPE_INDEX =1,
         SERIAL_BYTES_TO_FOLLOW_MSB_INDEX =2,
         SERIAL_BYTES_TO_FOLLOW_LSB_INDEX =3,
         
         SIZE_OF_HEADER =1,
         SIZE_OF_MSG_TYPE =1,
         SIZE_OF_BYTES_TO_FOLLOW =2,
         SIZE_OF_CRC_VALUE =2,

         TX_MSG_HEADER ='$',
         RX_MSG_HEADER =0x10,
         OTHER
      }

      //* Structure for serial com port settings */             

      
      struct COM_SETTINGS
      {
      //   public bool fullDuplex;    /* full or half duplex            */
         //public int physicalLayer;/* RS232 or RS485                   */
         //public int activity;     /* bitfields for TX, RX...          */
      }
      

      public struct TX_CONTEXT
      {
         public byte[] txBuf;
         //int SerialTxMachState;
      }

      //COM_SETTINGS[] ComSettings = new COM_SETTINGS[(int)SERIAL_DEF.PORTS];
      TX_CONTEXT[] TxContext = new TX_CONTEXT[(int)SERIAL_DEF.PORTS];

      bool[] fPortActive = new bool[ (int)SERIAL_DEF.PORTS];

      static System.IO.Ports.SerialPort[] SerialComPort = new System.IO.Ports.SerialPort[(int)SERIAL_DEF.PORTS];

      public String OpenPort    /*| true if successful                        */
      (
         int portIndex_,       /*| identifies which port object to open      */         
         int baudRate,
         String comPort
      )
      {

         /***********************/
         /* C O D E             */
         /***********************/
         /* If the structure has not been defined use the following default values  */
         if ( fPortActive[ portIndex_ ] )
         {  /* port already exists, shut it down before redefining it */
            ClosePort( portIndex_);
         }
         

         /* try to get the serial port */
         try
         {
            SerialComPort[portIndex_] = new System.IO.Ports.SerialPort();
            SerialComPort[portIndex_].PortName = comPort;
            SerialComPort[portIndex_].BaudRate = baudRate;
            SerialComPort[portIndex_].Open();
         }
         catch (Exception ex)
         {
             return ex.Message;
         }
 
         /* mark port as existent */
         fPortActive[ portIndex_ ] = true;
         Thread.Sleep(200);

         return "OK";
      }/* OpenPort() */

      public bool ClosePort              /*| true if successful */
      (                            /*|                    */
         int portIndex_  /*|                    */
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
            SerialComPort[portIndex_].Close();
            Thread.Sleep(200);
         }

         return true;
      } /* ClosePort */


        public bool CheckPortOpen()
        {
            if (SerialComPort[0] != null &&
                SerialComPort[0].IsOpen
              )
            {
                return true;
            }
            else
                return false;
        }

        bool SendByte   //*| false if error occurred during send, else true
      (                            //*|
         int portIndex_, //*| identifies the port
         byte aByte_,              //*| the character to be sent out
         bool internal_            //*| used only for rs485 mode, allows the tx and
      )                            //*| rx buffers to be twiddled in here or not                            
      {
         /***********************/
         /* L O C A L   D A T A */
         /***********************/

          /***********************/
          /* C O D E             */
          /***********************/
         if(fPortActive[ portIndex_ ] == true)
         {
            //* serial port object exists, proceed with transmission */
            try
            {
               byte[] sendBuff = new byte[2];
        
               sendBuff[0] = (Byte)aByte_;
               SerialComPort[ portIndex_ ].Write(sendBuff, 0 , 1);
            }
            catch (Exception)
            {
               // transmission failed
               return false;
            }

            // transmission is successful
            return true;
         }
         else
         {
            return false;
         }
      }/* SendByte() */

      public bool SendMessage
      (   
         int portIndex_,
         uint numFields_ ,
         params byte[] txBufPtr_
      )
      {
         /*
         ***********************
          L O C A L   D A T A
         ***********************
         */
          int i;
          uint numBytes;

            try
            {
                if (!SerialComPort[portIndex_].IsOpen) //fPortActive[portIndex_] == false)
                {
                    MessageBox.Show("No serial port is open",
                                    "Commport",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Warning
                                   );


                    return false;
                }
            }
            catch (Exception) { }

          TxContext[portIndex_].txBuf = new byte[1040];

          /*
          *************************
           E R R O R   C H E C K S
          *************************
          */
          numBytes = numFields_;

          if (numBytes > 1040) //SERIAL_DEF.MAX_TX_BUF_BYTES )
          {
              /* not allowed */
              return false;
          }

          /*
          *************************
           C O D E
          *************************
          */

          // get data bytes of the real message
          for (i = 0;
                i < numBytes;
                i++
              )
          {
              TxContext[portIndex_].txBuf[i] = txBufPtr_[i];
          }

          try
          {
              SerialComPort[portIndex_].Write(TxContext[portIndex_].txBuf, 0, (int)numBytes);
          }
          catch (Exception) { }


          return true;

      }/* SendMessage() */
      
      bool GetByte   
      (                     
         int portIndex_ ,   
         int timeOut_,
         out byte rxBytePtr_
      )                     
      {
         int status;
         int rxTimeOutBackup;


         if(fPortActive[ portIndex_ ] == true)
         {
            // serial port object exists, proceed with rx
            rxTimeOutBackup = SerialComPort[portIndex_].ReadTimeout;
            SerialComPort[portIndex_].ReadTimeout = timeOut_;

            try
            {
               // NOTE: ReadByte is a blocking call!
               status = SerialComPort[ portIndex_ ].ReadByte();
            }
            catch (Exception)
            {
               // Timed out
               rxBytePtr_ = 0;
               SerialComPort[portIndex_].ReadTimeout = rxTimeOutBackup;
               return false;
            }

            rxBytePtr_ = (byte)status;
            SerialComPort[portIndex_].ReadTimeout = rxTimeOutBackup;
            // return the received byte
            return true;
         }
         else
         {
            // the serial port object does not exist
            rxBytePtr_ = 0;
            return false;
         }
  
      }// end GetByte


      public uint ReceiveMessage
      (
         int portIndex_,
         int timeout,
         params byte[] rxBytePtr_   
      )
      {
          /*
          ***********************
           L O C A L   D A T A
          ***********************
          */
          uint bufPos;
          byte rxByte;
          byte status;
          uint payload = 0;

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
          status = 0;

          // wait up to t milliseconds for header byte 
          if (!GetByte(portIndex_, 50, out rxByte)) // original
          {
              // no data found at all
              return 0;
          }

          bufPos = 0;

          // save header byte to rx buffer
          rxBytePtr_[bufPos++] = rxByte;
          status = rxByte;

          while (status == 0x10)
          {
              while (GetByte(portIndex_, timeout, out rxByte))
              {
                  // save byte to rx buffer
                  if (bufPos >= (rxBytePtr_.Length - 1))
                      return bufPos;

                  rxBytePtr_[bufPos++] = rxByte;

                  if (bufPos == 4)
                  {
                      payload = (uint)(rxBytePtr_[2] << 8) | (rxBytePtr_[3]);
                  }

                  if (bufPos >= 4 &&
                      bufPos >= payload)
                  {
                      // all bytes received
                      return bufPos;
                  }

              }

              while (!GetByte(portIndex_, 250, out rxByte))
              {
                  // rest of message is not coming in time?, quit
                  status = 0x00;
                  return bufPos;
              }
              rxBytePtr_[bufPos++] = rxByte;
          }


          // terminal messaging, run this while loop

          while (GetByte(portIndex_, timeout, out rxByte))
          {
              // save byte to rx buffer
              if (bufPos >= (rxBytePtr_.Length - 1))
              {
                  return bufPos;
              }

              rxBytePtr_[bufPos++] = rxByte;

          }

          return bufPos;

      }/* ReceiveMessage() */


      int CalcCrc 
      (            
         byte numBytes_,
         params byte[] dataBufPtr_
      )                            
      {
         /*
         ***********************
         L O C A L   D A T A
         ***********************
         */
         return 1;

      }/* end CalcCrc() */


   } //end class SerialFd
}





