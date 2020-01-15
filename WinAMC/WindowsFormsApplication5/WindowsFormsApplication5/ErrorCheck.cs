using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ERRORCHECK
{
    class cErrorCheck
    {
        public int eventRecorder_crc() //int len, params byte[] ptr)
        {
            int eplCrc = 0;

            // TODO, CRC code

            return eplCrc;
        }

        public int Xmodem_crc(int len, int seed, params byte[] ptr)
        {
            int tblInitialized;
            int crcReg = 0;

            int[] crcTbl = new int[256];
            int i, j, k, m;

            tblInitialized = 0;

            if (tblInitialized == 0)
            {
                // init crc lookup table only once
                for (i = 0; i < 256; i++)
                {
                    j = i << 8;
                    for (k = 8; k > 0; k--)
                    {
                        if ((j & 0x8000) != 0)
                            j = (((j << 1) ^ 0x1021) & 0x0000ffff);
                        else
                            j = (j << 1);
                    }

                    crcTbl[i] = (int)j;
                }

                tblInitialized = 1;
            } // end of if(!tblInitialized)


            // always init register
            crcReg = seed;

            // calc CRC
            m = 0;
            for (k = len; k > 0; k--)
            {
                crcReg = (int)((crcReg << 8) ^ crcTbl[((crcReg >> 8) & 0x00ff) ^ ptr[m]]);

                m++;
            }

            // finish up and return
            return (crcReg & 0x0000ffff);

        } // end of crc() 


/*
 //CRC16
        public UInt16 CRC16_calcCrc
        (                      
            UInt8 *dataBufPtr_,
            UInt16 numBytes_   
        )                      
        {
            int i;
            int x;
            UInt16 crc, feed;
            UInt8 *tempPtr;

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

        }// end CRC16_calcCrc()
*/
    }
}






