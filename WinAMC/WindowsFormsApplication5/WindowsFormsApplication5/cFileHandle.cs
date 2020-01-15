using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace FileHandling //WindowsFormsApplication2
{
    class cFileHandle
    {
        public void Open(string fileName, out FileStream fs )
        {
            fs = new FileStream(fileName, FileMode.Create);
        }

        public void Read(FileStream fs, int size_, params byte[] data )
        {
            // set the stream position to the beginning of the file.
            fs.Seek(0, SeekOrigin.Begin);

            // read the data.
            for (int i = 0; i <size_; i++)
            {
                data[i] = (byte)fs.ReadByte();
            }            
        }

        public void Write(FileStream fs, int size_, byte[] data)
        {
           for (int i = 0; i < size_; i++)
           {
               fs.WriteByte(data[i]);
           }
        }

        public void Close(FileStream fs)
        {
            fs.Close();
        }
    } // end cFileHandle class

} // end Filehandling Namespace

