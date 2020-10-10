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
        public bool Open(string fileName, out FileStream fs, int mode)
        {
            fs = null;
            try
            {
                if (mode == 0)
                    fs = new FileStream(fileName, FileMode.Create);
                else if (mode == 1)
                    fs = new FileStream(fileName, FileMode.Append);
                else
                {
                    fs = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                }

                return true;
            }
            catch (Exception)
            {
                return false;
            }
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

        public void Seek(FileStream fs, long pos)
        {
            fs.Seek(pos, SeekOrigin.Begin);
        }

        public void Close(FileStream fs)
        {
            fs.Close();
        }
    } // end cFileHandle class

} // end Filehandling Namespace

