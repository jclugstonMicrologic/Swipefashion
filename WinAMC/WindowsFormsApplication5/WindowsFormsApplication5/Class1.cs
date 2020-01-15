using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsFormsApplication5
{
    class Class1
    {
        private int aValue =0;

        // Constructor
        public Class1()
        {
        }

        public void getValue( out int theValue_ )
        {
            theValue_ =aValue;
        }
        public void setValue( int theValue_ )
        {
           aValue =theValue_;
        }

        // Destructor
        ~Class1()
        {
        }
    }
}
