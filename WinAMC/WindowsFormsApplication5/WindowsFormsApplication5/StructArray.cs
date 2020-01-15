using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace STRUCTARRAY
{
    class StructArray
    {
        public static byte[] StructToByteArray(object oStruct)
        {          
            //Set the buffer to the correct size 
            byte[] buffer = new byte[Marshal.SizeOf(oStruct)];
            GCHandle h = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            // copy the struct into int byte[] mem alloc 
            Marshal.StructureToPtr(oStruct, h.AddrOfPinnedObject(), false);
            h.Free(); //Allow GC to do its job 

            return buffer;
        }

        public static object ByteArrayToStruct(System.Type type, params byte[] buffer)
        {           
            object oReturn = null;             

            GCHandle handle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            oReturn = (object)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), type);
            handle.Free();

            return oReturn;
        }

        public void SwapBytes16(out UInt16 newValue, UInt16 data )
        {
            newValue =(UInt16)( (data & 0xff00)>>8);
            newValue |= (UInt16)(data & 0x00ff);          
        }

        public static byte[] SwapByteArray16(params byte[] buffer)
        {
            // swap every two bytes

            byte[] newBuffer =new byte[buffer.Length];

            for( int j = 0; j < buffer.Length; j+=2)
            {
                newBuffer[j] =buffer[j+1];
                newBuffer[j+1] =buffer[j];
            }

            return newBuffer;
        }
    }

}
