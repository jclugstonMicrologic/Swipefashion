using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Threading;

using STRUCTARRAY;
using ERRORCHECK;
using FileHandling;
using TcpClientServer;
using SerialCom;

using Gigasoft.ProEssentials.Enums;

using System.Runtime.InteropServices; // required for DllImport( string, entrypoint )


namespace WindowsFormsApplication5
{
    enum SERIAL_COMMS
    {
       PORT =0,
       BAUDRATE =115200
    }

    enum PACKET
    {
        DLE = 0x10,
        STX = 0x02,
        CF = 0x03,  // control field
        ETX = 0x03,  // control field

        SIZEOF_HEADER = 0x04,

        MSG_ACK = 0x8000,

        /* AMC commands */
        CMD_GET_VERSION = 0x0200,
        CMD_OPEN_VALVE  =0x020d,
        CMD_CLOSE_VALVE =0x020e,
        CMD_GET_PRESS  =0x020f,
        CMD_GET_BRD_ID =0x0210,

        CMD_SET_FTC_SETUP = 0x2001,
        CMD_RESET = 0x200A,        
    }

    enum CRC
    {
        SEED = 0xffff
    }

    public partial class TheMainForm : Form
    {
        public TheMainForm()
        {
            InitializeComponent();
            // Microsoft VC# does not allow accessing form objects from 
            // a thread that the object was not created on
            // the following code disables the exception error
            // not sure if I should do that, but OK for now
            Control.CheckForIllegalCrossThreadCalls = false;

            AboutBox1 aboutBox = new AboutBox1();

            this.Text = String.Format("WinAMC {0}", String.Format(" v{0}", aboutBox.AssemblyVersion));

        }

        [DllImport("UsbComms.dll", EntryPoint = "OpenUsbPort")]
        public static extern bool OpenUsbPort();

        public SerialFd serialFd = new SerialFd();

        public byte[] Payload = new byte[2000];
        public uint PayloadSize;

        //string server = null;
        //int port = (int)CLIENTS.PORT;

        public int PanelSelect = 0;

        int dataPt = 0;

        public struct AMC_OP
        {
            public byte ioStates;
            public byte machState;

            public UInt16 vBatt;
            public UInt16 iBatt;

            public UInt32 spare1;
            public UInt32 spare2;
   
            public UInt16 crc;
        }

        public struct ANALOGS
        {
            public UInt16 vBatt;
            public UInt16 iBatt;
            public UInt16 spareA2;
            public UInt16 spareA3;
        }

        public struct AMC_SETUP
        {
            public byte setPoint;
            public byte deploy;
            public byte seqTime;
            public byte temp;

            public UInt32 pcTimeStamp;

            public UInt16 logAddr;

            public byte detailedLog;

            public byte setupInfo; //bit0 trigger on both switches or only one 

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 18)]
            public byte[] spare;

            public UInt16 crc;
        }

        public struct MOTOR_DATA
        {
            public float speed;
            public float relativeAngle;
        }

        public AMC_OP AmcOp = new AMC_OP();
        public ANALOGS Analogs = new ANALOGS();
        public AMC_SETUP AmcSetup = new AMC_SETUP();
        public MOTOR_DATA MotorData = new MOTOR_DATA();
                
        public UInt16 Counter =0;
        public UInt16 DownloadStart = 0;
        public UInt16 EraseCnt = 0;

        public byte ValveNbr = 0;
        public float Pressure = 0;

        public struct P_SENSOR_DATA
        {
            public float press;
            public float temp;
        }
        public P_SENSOR_DATA[] PSensorData = new P_SENSOR_DATA[8];

        int BoardId = 0;

        private void InitDataGrid()
        {
            dataGridView.Width  =400;
            dataGridView.Height =180;
            dataGridView.Left   =15;
            dataGridView.Top    =40;

            dataGridView.ColumnCount = 3;
            dataGridView.ColumnHeadersVisible = true;
            dataGridView.RowHeadersVisible = false;

            for (int i = 0; i < dataGridView.ColumnCount; i++)
            {
                dataGridView.Columns[i].Width = 120;
            }
            

            // create the rows
            for (int i = 1; i < 7; i++)
            {
                dataGridView.Rows.Add("Target P" + i.ToString(), "", "", "");
            }

            dataGridView.Columns[0].HeaderText = "Setup";
            dataGridView.Columns[1].HeaderText = "Value";

            //dataGridView.Rows[0].HeaderCell.Value = "1";
/*
            dataGridView.Rows.Add("Initial cycles/time for motor activation", "", "", "");          
◦ Number of rotations for retraction of linear drive device
◦ Amperage/time thresholds for the major event characteristics of the retraction and
extension and final seating of the linear drive device
◦ Initial motor activation timing
◦ Operating/running amperage threshold after intiation for retraction and extension
revolutions required
◦ Amperage threshold for the end of the extension phase to limit force exerted in a
dead-end-stop position*/
        }

        private void InitProfileGrid()
        {
            ProfileGridView.Width = 240;
            ProfileGridView.Height = 180;
            ProfileGridView.Left = 10;
            ProfileGridView.Top = 20;

            ProfileGridView.ColumnCount = 1;
            ProfileGridView.ColumnHeadersVisible = true;
            ProfileGridView.RowHeadersVisible = true;

            ProfileGridView.Columns[0].Width = 80;
            //ProfileGridView.Columns[1].Width = 80;

            /* create the rows */
            for (int i = 1; i < 8; i++)
            {
     //           ProfileGridView.Rows.Add("Parameter " + i.ToString(), "", "", "");
                ProfileGridView.Rows.Add("","", "");
                ProfileGridView.Rows[i-1].HeaderCell.Value = "Target P" + i.ToString();
            }

            ProfileGridView.Columns[0].HeaderText = "Press(psi)";
            //ProfileGridView.Columns[1].HeaderText = "Value";

            ProfileGridView.Visible = true;
            //dataGridView.Rows[0].HeaderCell.Value = "1";

        }

        private void InitControllerGrid()
        {
            ControllerGridView.Width = 420;
            ControllerGridView.Height = 240;
            ControllerGridView.Left = 15;
            ControllerGridView.Top = 20;

            ControllerGridView.ColumnCount = 4;
            ControllerGridView.ColumnHeadersVisible = true;
            ControllerGridView.RowHeadersVisible = true;
            ControllerGridView.RowHeadersWidth = 60;

            ControllerGridView.Columns[0].Width = 80;
            ControllerGridView.Columns[1].Width = 80;
            ControllerGridView.Columns[2].Width = 80;
            ControllerGridView.Columns[3].Width = 80;

            /* create the rows */
            for (int i = 1; i < 9; i++)
            {
                ControllerGridView.Rows.Add("", "", "");
                ControllerGridView.Rows[i - 1].HeaderCell.Value = "S" + i.ToString();
            }

            ControllerGridView.Columns[0].HeaderText = "Press (psi)";
            ControllerGridView.Columns[1].HeaderText = "Temp (DegC)";
            ControllerGridView.Columns[2].HeaderText = "Target (psi)";
            ControllerGridView.Columns[3].HeaderText = "Delta P (psi)";


            ControllerGridView.Visible = true;
        }

        private void InitGenericPlot(Gigasoft.ProEssentials.Pesgo pePlot, string title_,          string xaxisLbl_,                    string yaxisLbl_            )
        {
            pePlot.PeFunction.Reset();

            //pePlot.PeGrid.GridBands = false;

            pePlot.PeConfigure.PrepareImages = true;
            pePlot.Left = 620;// 20;
            pePlot.Top = 25;// 490;
            pePlot.Width = 610;// 600;
            pePlot.Height = 375;
            // PESGraph.BringToFront();

            pePlot.PeData.Subsets = 2;
            pePlot.PeData.Points = 1000;

            pePlot.PeString.SubsetLabels[0] = "G Vol";// "Range";
            pePlot.PeString.SubsetLabels[1] = "G Heel";
            //        pePlot.PeString.SubsetLabels[2] = "";

            pePlot.PeString.YAxisLabel = "Liters";
            pePlot.PeGrid.Configure.ManualMinY = 0;
            pePlot.PeGrid.Configure.ManualMaxY = 10000;

            pePlot.PeLegend.SubsetColors[0] = System.Drawing.Color.DarkBlue;
            pePlot.PeLegend.SubsetLineTypes[0] = LineType.MediumSolid;

            pePlot.PeLegend.SubsetColors[1] = System.Drawing.Color.Red;
            pePlot.PeLegend.SubsetLineTypes[1] = LineType.ThinSolid;

            // Set Manual Y scale
            //pePlot.PeGrid.Configure.ManualScaleControlY = ManualScaleControl.MinMax;

            pePlot.PeLegend.SubsetColors[2] = System.Drawing.Color.Red;
            pePlot.PeLegend.SubsetLineTypes[2] = LineType.ThickSolid;


            pePlot.PeString.MainTitle = title_;
            pePlot.PeString.SubTitle = "";

            // Set Manual X scale
            pePlot.PeGrid.Configure.ManualScaleControlX = ManualScaleControl.MinMax;
            pePlot.PeGrid.Configure.ManualMinX = 0;// DateTime.Now.ToOADate();
            pePlot.PeGrid.Configure.ManualMaxX = pePlot.PeData.Points;// FluidLevelStripChart.PeGrid.Configure.ManualMinX + 0.021;// 0.0035;

            // Enable Bar Glass Effect 
            //pePlot.PePlot.Option.BarGlassEffect = true;

            // Set X scale label stuff bottom
            // PESGraph.XAxisOnTop =false;
            pePlot.PeAnnotation.Line.XAxisColor[0] = System.Drawing.Color.Red;
            pePlot.PeString.XAxisLabel = "Data Point";

            pePlot.PePlot.MarkDataPoints = false;
            pePlot.Visible = true;

            // enable double precision which is usually 
            // required for date time handling 
            // This means we pass x data to XDataII
            pePlot.PeData.UsingXDataii = false;

            pePlot.PeUserInterface.Cursor.PromptTracking = true;
            pePlot.PeUserInterface.Cursor.PromptStyle = CursorPromptStyle.XYValues;
            pePlot.PeUserInterface.Menu.MultiAxisStyle = MenuControl.Show;

            pePlot.PeUserInterface.Allow.Zooming = AllowZooming.HorzAndVert;
            pePlot.PeUserInterface.Allow.ZoomStyle = ZoomStyle.Ro2Not;

            pePlot.PePlot.Option.GradientBars = 12;

            pePlot.PeColor.BitmapGradientMode = false;
            pePlot.PeColor.QuickStyle = QuickStyle.LightInset;
            pePlot.PeLegend.Style = LegendStyle.OneLineTopOfAxis;
            pePlot.PeUserInterface.Menu.ShowLegend = MenuControl.Show;
            pePlot.PeUserInterface.Menu.LegendLocation = MenuControl.Show;

            pePlot.PeFont.FontSize = FontSize.Large;

            pePlot.PeLegend.SimplePoint = true;
            pePlot.PeLegend.SimpleLine = true;
            pePlot.PeLegend.Style = LegendStyle.OneLineInsideOverlap;

            //    FuelLevelStripChart.PeColor.GraphForeground = Color.FromArgb(50, 0, 0, 0);
            pePlot.PeGrid.LineControl = GridLineControl.Both;
            pePlot.PeGrid.Style = GridStyle.Dot;
            pePlot.PeConfigure.BorderTypes = TABorder.SingleLine;

            pePlot.PeData.NullDataValueX = 0;

            pePlot.PeUserInterface.HotSpot.Data = false;
            pePlot.PeUserInterface.HotSpot.Size = HotSpotSize.Large;

            // Improves Metafile Export 
            pePlot.PeSpecial.DpiX = 600;
            pePlot.PeSpecial.DpiY = 600;

            pePlot.PeData.DateTimeMode = false;

            for (int j = 0; j < 10; j++)
            {
                pePlot.PeData.X[0, j] = j;
                pePlot.PeData.Y[0, j] = 0;

                pePlot.PeData.X[1, j] = j;
                pePlot.PeData.Y[1, j] = 0;

                pePlot.PeData.X[1, j] = j;
                pePlot.PeData.Y[1, j] = 0;
            }

            pePlot.PeColor.Desk = System.Drawing.Color.WhiteSmoke;
            pePlot.PeColor.GraphBackground = System.Drawing.Color.White;

            // Update image and force paint 
            pePlot.PeFunction.ReinitializeResetImage();
            pePlot.Refresh();
        }

        
        private void UpdateGenericPlot(Gigasoft.ProEssentials.Pesgo pePlot, float fluidLvl, float fluidMass, float temperature)
        {
            float[] newY = new float[3];
            float[] newX = new float[3];

            newY[0] = fluidLvl;
            newY[1] = fluidMass;
            newY[2] = temperature;

            // New y value and x value
            newX[0] = dataPt;// DateTime.Now.ToOADate();
            newX[1] = newX[0];
            newX[2] = newX[0];

            Gigasoft.ProEssentials.Api.PEvset(pePlot.PeSpecial.HObject, Gigasoft.ProEssentials.DllProperties.AppendYData, newY, 1);
            Gigasoft.ProEssentials.Api.PEvset(pePlot.PeSpecial.HObject, Gigasoft.ProEssentials.DllProperties.AppendXData, newX, 1);
            
            dataPt++;

            if (dataPt == pePlot.PeData.Points)
                pePlot.PeGrid.Configure.ManualScaleControlX = ManualScaleControl.None;


            // Update image and force paint 
            pePlot.PeFunction.ReinitializeResetImage();
            pePlot.Refresh();
        }
        

        private void InitDownloadGrid()
        {
            downloadGridView.Visible = true;

            downloadGridView.Width = 390;
            downloadGridView.Height = 450;
            downloadGridView.Left = 15;
            downloadGridView.Top = 20;

            downloadGridView.ColumnCount = 10;
            downloadGridView.ColumnHeadersVisible = true;
            downloadGridView.RowHeadersVisible = false;

            for (int i = 0; i < downloadGridView.ColumnCount; i++)
            {
                downloadGridView.Columns[i].Width = 62;
            }
            downloadGridView.Columns[0].Width = 80;

            downloadGridView.Columns[6].Width = 45;
            downloadGridView.Columns[7].Width = 42;
            downloadGridView.Columns[8].Width = 42;
            downloadGridView.Columns[9].Width = 42;
/*
            // create the rows
            downloadGridView.Rows.Add("11:00:00", "125", "150", "3.1", "1.1");
            downloadGridView.Rows.Add("12:00:00", "100", "120", "3.0", "1.0");
*/
            for (int row = 0; row < 120; row++)
            {
                downloadGridView.Rows.Add("", "", "", "");
            }

            downloadGridView.Columns[0].HeaderText = "Time";
            downloadGridView.Columns[1].HeaderText = "Travel (ms)";
            downloadGridView.Columns[2].HeaderText = "Run Time (ms)";
            downloadGridView.Columns[3].HeaderText = "VBatt (V)";
            downloadGridView.Columns[4].HeaderText = "IMotor (A)";
            downloadGridView.Columns[5].HeaderText = "Ball Count";

            downloadGridView.Columns[6].HeaderText = "Sw1";
            downloadGridView.Columns[7].HeaderText = "Sw2";
            downloadGridView.Columns[8].HeaderText = "Sw3";
            downloadGridView.Columns[9].HeaderText = "Sw4";

            label10.Text = "Ball Trigger Value: NA";
            SetupTimeLbl.Text = "Setup Time: NA"; 
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            this.Width = 650;
            this.Height = 700;

            InitDataGrid();

            InitDownloadGrid();

            MainPanel.Top = 40;
            MainPanel.Left = 10;
            MainPanel.Width = 770;
            MainPanel.Height = 600;

            DiagnosticPnl.Top = MainPanel.Top;
            DiagnosticPnl.Left = MainPanel.Left;
            DiagnosticPnl.Width = 700;

            DownloadPanel.Top = MainPanel.Top;
            DownloadPanel.Left = MainPanel.Left;
            DownloadPanel.Width = 700;
            DownloadPanel.Height = 210;

            listBox1.Items.Add("Serial Tx");
            RxListBox.Items.Add("Serial Rx");

            CommSelectPnl.Top = 10;
            CommSelectPnl.Left =80;


            Sw1TextBox.BackColor = System.Drawing.Color.LightGray;
            Sw2TextBox.BackColor = System.Drawing.Color.LightGray;
            Sw3TextBox.BackColor = System.Drawing.Color.LightGray;
            Sw4TextBox.BackColor = System.Drawing.Color.LightGray;
            CommTextBox.BackColor = System.Drawing.Color.LightGray;
            EncTextBox.BackColor = System.Drawing.Color.LightGray;
            SuTextBox.BackColor = System.Drawing.Color.LightGray;
            

            InitProfileGrid();
            InitControllerGrid();
            //InitGenericPlot(GenericPlot, "1", "2", "3");

            PanelSelect = (int)SET_COMMANDS.SET_COMMPORT;

            PanelsFrm panelsForm = new PanelsFrm(this);
            panelsForm.ShowDialog();

            if(!panelsForm.GetCommStatus())
            {
                FtcStatusStrip.Items[0].Text = "Disconnected";
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                if (!OpenUsbPort())
                {
                    MessageBox.Show("USB Open Port Failed",
                                    "Usb Comm",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );
                }
                else
                {
                    MessageBox.Show("USB Open Port Succesful",
                                    "Usb Comm",
                                    MessageBoxButtons.OK
                                   );
                }
            }
            catch (EntryPointNotFoundException EpNotFoundEx)
            {
               MessageBox.Show(EpNotFoundEx.ToString());
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            List<int> myInts = new List<int>();

            myInts.Add(5);
            myInts.Add(4);
            myInts.Add(3);
            myInts.Add(2);
            myInts.Add(1);

            myInts.Remove(2);
            myInts.Insert(2, 10);

            myInts.Sort();

            if( myInts.Contains(7) )
               myInts[myInts.IndexOf(7)] =22; // replace with 22
            if( myInts.Contains(3) )
               myInts[myInts.IndexOf(3)] =4;  // replace with 4 which is a duplicate

            int x;
            x =myInts.LastIndexOf(4);

            x = myInts.Capacity;            
            myInts.TrimExcess();
            x = myInts.Capacity;

            for (int i = 0; i < myInts.Count; i++)
            {
               MessageBox.Show("MyInts:\n" + myInts[i].ToString(),
                               "List<int> Example",
                               MessageBoxButtons.OK
                              );
            }

            myInts.Clear();
        }


        private void button4_Click(object sender, EventArgs e)
        {
           button4.Enabled = false;           

           Thread ctThread = new Thread(clientChat);            
           ctThread.Start();
        }
        
        private void clientChat()
        {
            while (true)
            {
                if (cServer.serverData != null)
                {
                    listBox1.Items.Add(cServer.serverData);
                    listBox1.SelectedIndex = listBox1.Items.Count - 1;

                    cServer.serverData = null;
                }

                if (handleClient.dataFromClient != null)
                {
                    listBox1.Items.Add(handleClient.dataFromClient);
                    listBox1.SelectedIndex = listBox1.Items.Count - 1;

                    handleClient.dataFromClient = null;
                }

                Thread.Sleep(1);
            }
        }

        public Thread serialThread;

        public void rxDataThread()
        {
           uint nbrBytesRx = 0;
           uint response = 0;
           byte sw1State = 0;
           byte sw2State = 0;
           byte sw3State = 0;
           byte sw4State = 0;
           byte commState = 0;
           byte encState = 0;

           float vBatt, iBatt;
           float delatP = 0.0F;

           while (true)
           {
               byte[] rxBuffer = new byte[1056];

               nbrBytesRx = serialFd.ReceiveMessage(0, 100, rxBuffer);

               if (nbrBytesRx != 0)
               {
                   SerialMonitorTimer.Enabled = false;

//                  RxListBox.Items.Add(rxBuffer[0].ToString("x") );

                   if (ParseMessage(out response, nbrBytesRx, rxBuffer))
                   {                     
                        switch(response)
                        { 
                            case (int)PACKET.CMD_GET_PRESS:                               
                                ValveNbr =0;

                                for (int j = 0; j < 8; j++)
                                {
                                    delatP =(float)(PSensorData[j].press - PSensorData[7].press) / (float)6.89476;

                                    if (PSensorData[j].press != -1)
                                    {
                                        ControllerGridView[0, j].Value = (PSensorData[j].press / 6.89476).ToString("0.000");
                                        ControllerGridView[1, j].Value = (PSensorData[j].temp).ToString("0.0");

                                        ControllerGridView[3, j].Value = delatP.ToString("0.000");

                                        try
                                        {
                                            if (delatP >= Convert.ToSingle(ControllerGridView[2, j].Value))
                                                ValveNbr |= (byte)(0x01 << j);
                                        }
                                        catch (Exception ex){ }
                                    }
                                    else
                                    {
                                        ControllerGridView[0, j].Value = "FAULT";
                                        ControllerGridView[1, j].Value = "FAULT";

                                        ControllerGridView[3, j].Value = "FAULT";
                                    }
                                }

                                if(ValveNbr !=0)
                                    BuildSerialMessage((int)PACKET.CMD_CLOSE_VALVE);
                                break;
                            case (int)PACKET.CMD_SET_FTC_SETUP:         
                               break;
                           case (int)PACKET.CMD_GET_VERSION:
                               FwVersionLbl.Text = "AMC: " + System.Text.Encoding.ASCII.GetString(Payload);
                               break;
                           case (int)PACKET.CMD_GET_BRD_ID:
                                BrdTypeLbl.Text ="Board Type: " + BoardId.ToString();
                               break;                                
                     }
                  }
              }

              Thread.Sleep(1);
           }
        }


        private void timer1_Tick(object sender, EventArgs e)
        {
            SerialMonitorTimer.Enabled = true;

            if( !BuildSerialMessage((int)PACKET.CMD_GET_PRESS) )
            {                
                timer1.Enabled = false;
                return;
            }
            Refresh();

        }

        private void button8_Click(object sender, EventArgs e)
        {
        }

        private void button9_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "(*.*)|*.*";
            openFileDialog1.FileName = "";

            openFileDialog1.Title = "File1";

            if (openFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                // user has canceled save
                return;
            }
        }

      
        private void button12_Click(object sender, EventArgs e)
        {
           serialFd.SendMessage(0, 1, (byte)'S');
        }

        private void MotorDirBtn_Click(object sender, EventArgs e)
        {
           serialFd.SendMessage(0, 1, (byte)'D');
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
           byte[] txBuffer =new byte[10];
           int desiredSpeed;
           desiredSpeed = Convert.ToInt16(numericUpDown1.Value); //textBox2.Text);

           txBuffer[0] = (byte)'V';
           txBuffer[1] = (byte)((desiredSpeed & 0xff00) >> 8);
           txBuffer[2] = (byte)(desiredSpeed & 0x00ff);

           serialFd.SendMessage(0, 3, txBuffer);
        }

        private void diagnosticToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            timer1.Enabled = true;

            MainPanel.Visible = false;
            DiagnosticPnl.Visible = true;
            DownloadPanel.Visible =false;
        }

        private void setupToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MainPanel.Visible = true;
            DiagPnl.Visible = false;
            DownloadPanel.Visible = false;
            BrdTypeLbl.Visible = true;
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox1 aboutBox = new AboutBox1();
            aboutBox.ShowDialog();
        }

        private void downloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
   //         MainPanel.Visible = false;
     //       DiagnosticPnl.Visible = false;
       //     DownloadPanel.Visible = true;
        }

        private void SendToFpcBtn_Click(object sender, EventArgs e)
        {

        }
   
        public bool BuildSerialMessage(UInt16 command)
        {
            uint nbrBytes = 0;
            uint nbrBytesToTx = 0;

            byte[] TxBuf = new byte[256];
            byte[] tempBuf = new byte[4];

            int calculatedCRC = 0;


            cErrorCheck errCheck = new cErrorCheck();
            Array.Clear(TxBuf, 0, TxBuf.Length);

                // adjust the array size (ensure it is still max size)
//                Array.Resize(ref Payload, 2000);

            // head
            TxBuf[0] = (byte)PACKET.DLE;
            TxBuf[1] = (byte)PACKET.STX;

            // there is a message in our que, build, and transmit
            switch (command)
            {
                case (int)PACKET.CMD_OPEN_VALVE:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = ValveNbr;

                    TxBuf[nbrBytes++] = Payload[0];
                    break;
                case (int)PACKET.CMD_CLOSE_VALVE:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = ValveNbr;

                    TxBuf[nbrBytes++] = Payload[0];
                    break;
                case (int)PACKET.CMD_GET_PRESS:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_GET_BRD_ID:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_GET_VERSION:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_SET_FTC_SETUP:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload = StructArray.StructToByteArray(AmcSetup);

                    for (int j = 0; j < Marshal.SizeOf(AmcSetup); j++)
                    {
                        TxBuf[nbrBytes++] = Payload[j];
                    }                        
                    break;
                case (int)PACKET.CMD_RESET:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
            }

            // do this now because it needs to go through the CRC calc
            nbrBytesToTx = nbrBytes + 2;
            TxBuf[2] = (byte)((nbrBytesToTx & 0xff00) >> 8);
            TxBuf[3] = (byte)(nbrBytesToTx & 0x00ff);

            // calculate CRC of transmit buffer
            calculatedCRC = errCheck.CrcCalc16((int)(nbrBytes), TxBuf);

            // tail 
            TxBuf[nbrBytes++] = (byte)((calculatedCRC & 0xff00) >> 8); //0x5a;  // CRC MSB
            TxBuf[nbrBytes++] = (byte)(calculatedCRC & 0x00ff);        //0xa5;  // CRC LSB

            if (serialFd.SendMessage(0, nbrBytes, TxBuf))
            {
                SerialMonitorTimer.Enabled = true;

                return true;
            }
            else
                return false;
        }

        private bool ParseMessage(out uint response, uint nbrRxBytes, params byte[] rxBuffer)
        {
            bool status = false;
            UInt16 rxCrc = 0;
            byte[] tempBuf = new byte[2000];

            cErrorCheck errCheck = new cErrorCheck();
            int calculatedCRC = 0;
            int logDoneCnt = 0;

            string sw1State = "";
            string sw2State = "";
            string sw3State = "";
            string sw4State = "";

            // check header
            if (rxBuffer[0] != 0x10 &&
                rxBuffer[1] != 0x02
               )
            {
                // not a proper serial message
                response = 0;
                return false;
            }

            Array.Clear(tempBuf, 0, tempBuf.Length);
            Array.Clear(Payload, 0, Payload.Length);

            // get size of payload
            PayloadSize = (uint)((rxBuffer[(int)PACKET.SIZEOF_HEADER - 2] << 8) | rxBuffer[(int)PACKET.SIZEOF_HEADER - 1]);

            if (PayloadSize > 2000)
                PayloadSize = 2000;

            if (PayloadSize < 8)
                PayloadSize = 8;

            // get command response
            response = (uint)((rxBuffer[(int)PACKET.SIZEOF_HEADER] << 8) | rxBuffer[(int)PACKET.SIZEOF_HEADER + 1]);

            // get received CRC
            rxCrc = (UInt16)((UInt16)(rxBuffer[PayloadSize - 2] << 8) | (UInt16)rxBuffer[PayloadSize - 1]);

            // calculate CRC of received data
            calculatedCRC = errCheck.CrcCalc16((int)(PayloadSize - 2), rxBuffer);
            
            // check CRC
            if (rxCrc != calculatedCRC)
            {
                response = 1;
                return true;
            }           

            // get payload (i.e. remove header)
            System.Array.Copy(rxBuffer, ((int)PACKET.SIZEOF_HEADER + 2), tempBuf, 0, PayloadSize);

            status = true;

            response &= ~(uint)PACKET.MSG_ACK;

            // process response (could all be done in WinForm, where GUI stuff is done)
            switch (response)
            {
                case (int)PACKET.CMD_GET_PRESS:
                    //tempBuf = StructArray.SwapByteArray16(tempBuf);

                    //AmcSetup = (AMC_SETUP)StructArray.ByteArrayToStruct(typeof(AMC_SETUP), tempBuf);
                    //Pressure    = System.BitConverter.ToSingle(tempBuf, 0);
                    //Temperature = System.BitConverter.ToSingle(tempBuf, 0);
                    for (int j = 0; j < PSensorData.Length; j++)
                    {
                        int sizeofData = System.Runtime.InteropServices.Marshal.SizeOf(PSensorData[0]);

                        PSensorData[j] = (P_SENSOR_DATA)StructArray.ByteArrayToStruct(typeof(P_SENSOR_DATA), tempBuf);

                        System.Array.Copy(tempBuf, sizeofData, tempBuf, 0, (PayloadSize - sizeofData));
                    }
                    break;
                case (int)PACKET.CMD_GET_VERSION:
                    System.Array.Copy(tempBuf, 0, Payload, 0, (PayloadSize - 8));
                    break;
                case (int)PACKET.CMD_GET_BRD_ID:
                    BoardId = Convert.ToByte(tempBuf[0]);
                    break;
            }

            return status;
        }

        public void ConvetToBuffer16(int theValue, out byte[] txBufPtr)
        {
            txBufPtr = new byte[2];

            txBufPtr[0] = (byte)((theValue & 0xff00) >> 8);
            txBufPtr[1] = (byte)((theValue & 0x00ff));
        }

        private void fPCToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void ClearFpcLogBtn_Click(object sender, EventArgs e)
        {

        }

        private void button12_Click_1(object sender, EventArgs e)
        {

        }

        private void SendToFpcBtn_Click_1(object sender, EventArgs e)
        {
            try
            {
                if(AmcSetup.setPoint ==0)
                {
                    MessageBox.Show("Ball trigger cannot be 0",
                                    "Table Setup Error",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );

                    return;
                }

                DateTime localDate = DateTime.Now;
                DateTime origin = new DateTime(1970, 1, 1, 0, 0, 0, 0);
                TimeSpan diff = localDate.ToUniversalTime() - origin;

                AmcSetup.pcTimeStamp =(UInt32)diff.TotalSeconds;

                UInt64 theDiff = (UInt64)diff.TotalMilliseconds;
                theDiff = theDiff % 1000;

                AmcSetup.crc = 0x5aa5;

                BuildSerialMessage((int)PACKET.CMD_SET_FTC_SETUP);
            }
            catch (Exception)
            {
                MessageBox.Show("Table contains invalid entries",
                                "Exception Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
            }
        }

        private void button11_Click_2(object sender, EventArgs e)
        {
            BuildSerialMessage((int)PACKET.CMD_GET_PRESS);
        }

        private void ClearFpcLogBtn_Click_1(object sender, EventArgs e)
        {
        }

        private void button12_Click_2(object sender, EventArgs e)
        {
        }

        private void SerialMonitorTimer_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            SerialMonitorTimer.Enabled = false;


      //      MessageBox.Show("No response from FTC",
           //                 "FTC Communications",
                        //    MessageBoxButtons.OK,
                        //    MessageBoxIcon.Error
                        //   );
            
        }

        private void button13_Click(object sender, EventArgs e)
        {
            string result, aStr;

            serialFd.ClosePort((int)SERIAL_COMMS.PORT);

            aStr = CommportComboBox.Items[CommportComboBox.SelectedIndex].ToString();

            result = serialFd.OpenPort((int)SERIAL_COMMS.PORT, (int)SERIAL_COMMS.BAUDRATE, aStr);

            if (result == "OK")
            {
                Thread serialThread = new Thread(rxDataThread);
                serialThread.Start();

                // get FTC fw version
                BuildSerialMessage((int)PACKET.CMD_GET_VERSION);
            }
            else
            {
                MessageBox.Show("Serial Port Open Failed",
                                "Serial Comm",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
            }

            CommSelectPnl.Visible = false;
        }

        private void button14_Click(object sender, EventArgs e)
        {
            CommSelectPnl.Visible = false;
        }

        private void connectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            PanelSelect = (int)SET_COMMANDS.SET_COMMPORT;
            PanelsFrm panelsForm = new PanelsFrm(this);
            panelsForm.ShowDialog();
        }

        private void StartMotorBtn_Click(object sender, EventArgs e)
        {
        }

        private void fPCParametersToolStripMenuItem_Click(object sender, EventArgs e)
        {
            PanelSelect = (int)SET_COMMANDS.SET_FPC_PARAM;
            PanelsFrm panelsForm = new PanelsFrm(this);
            panelsForm.ShowDialog();

        }

        private void button15_Click(object sender, EventArgs e)
        {
            timer2.Enabled = true;
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
        }

        private void counterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            PanelSelect = (int)SET_COMMANDS.SET_FPC_COUNTER;
            PanelsFrm panelsForm = new PanelsFrm(this);
            panelsForm.ShowDialog();
        }

        private void TheMainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            serialFd.ClosePort(0);

            if (serialThread != null)
            {
                serialThread.Suspend();
                serialThread = null;
            }

            System.Windows.Forms.Application.Exit();
            System.Diagnostics.Process.GetCurrentProcess().Kill();
        }

        private void logToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DiagPnl.Top = 0;
            DiagPnl.Left = 0;
            DiagPnl.Width = 500;
            DiagPnl.Height = 520;
            DiagPnl.Visible = true;

            BrdTypeLbl.Visible = false;

            downloadGridView.Rows.Clear();
        }

        private void button16_Click_1(object sender, EventArgs e)
        {

        }
      
        public void UpdateStatusStrip(String aStr)  
        {
            FtcStatusStrip.Items[0].Text = aStr;
        }
       
        private void button18_Click_1(object sender, EventArgs e)
        {

        }

        private void button17_Click(object sender, EventArgs e)
        {
            ClearFpcLogBtn_Click_1(null, null);
        }

        private void SaveLogBtn_Click(object sender, EventArgs e)
        {
            if (downloadGridView.RowCount == 1)
            {
                MessageBox.Show("Table is empty, ensure you have downloaded a log",
                                "FTC Log",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Warning
                               );

                return;
            }

            saveFileDialog1.Filter = "Log Files (*.txt)|*.txt";
            saveFileDialog1.FileName = "FTCLog.txt";

            saveFileDialog1.Title = "FTC Log File";

            if (saveFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                // user has canceled save
                return;
            }

            FileStream fs;
            byte[] dataArray = new byte[100];
            string[] logValue =new string[10];
            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();

            cFileHandle fhandle = new cFileHandle();

            fhandle.Open(saveFileDialog1.FileName, out fs, 2);

            string theString = "MAN\nMAN Log\n";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = DateTime.Now.ToString("MMM/dd/yyyy hh:mm:ss tt\n\n");
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = "Time,Travel (ms),Run Time (ms),VBatt(V),IMotor(A),Ball Count,Sw1,Sw2,Sw3,Sw4";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            for (int j = 0; j < (downloadGridView.RowCount-1); j++)
            {
                dataArray[0] = (byte)'\n';

                fhandle.Write(fs, 1, dataArray);
                for (int i = 0; i < downloadGridView.ColumnCount; i++)
                {
                    try
                    {
              //          if( i>=6 )
                //            logValue[i] = downloadGridView[i, j].Value + ",";
                  //      else
                            logValue[i] = downloadGridView[i, j].Value.ToString() + ",";

                        dataArray = encoding.GetBytes(logValue[i]);

                        fhandle.Write(fs, dataArray.Length, dataArray);
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("Table contains invalid entries",
                                        "Exception Error",
                                        MessageBoxButtons.OK,
                                        MessageBoxIcon.Error
                                    );

                        fhandle.Close(fs);
                        return;
                    }
                }
            }

            fhandle.Close(fs);           
        }

        private void button20_Click(object sender, EventArgs e)
        {
            ValveNbr = 0x01;

            BuildSerialMessage((int)PACKET.CMD_CLOSE_VALVE);
        }

        private void button19_Click(object sender, EventArgs e)
        {
            ValveNbr = 0x01;

            BuildSerialMessage((int)PACKET.CMD_OPEN_VALVE);            
        }

        private void button21_Click(object sender, EventArgs e)
        {
            DialogResult result1 = MessageBox.Show("Are you sure you wish to restart the FTC ball states",
                                                   "Ball state",
                                                    MessageBoxButtons.YesNo,
                                                    MessageBoxIcon.Warning
                                                   );

            if (result1 == DialogResult.No)
                return;

            byte[] aBuf = new byte[256];
            aBuf[0] = (byte)'$';
            aBuf[1] = (byte)'$';
            aBuf[2] = (byte)'$';

            /* string to array */
            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
            aBuf = encoding.GetBytes("$$$");

            serialFd.SendMessage(0, (uint)aBuf.Length, aBuf);

            /* array to string */
            //aStr = System.Text.Encoding.ASCII.GetString(anArray);

            //BuildSerialMessage((int)PACKET.CMD_RESTART);  
        }

        private void button22_Click(object sender, EventArgs e)
        {
            DialogResult result1 = MessageBox.Show("Are you sure you wish to reset the FTC controller",
                                                   "Ball state",
                                                    MessageBoxButtons.YesNo,
                                                    MessageBoxIcon.Warning
                                                   );

            if (result1 == DialogResult.No)
                return;

            BuildSerialMessage((int)PACKET.CMD_RESET); 
        }

        private void SaveProfileBtn_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = "Profile Files (*.txt)|*.txt";
            saveFileDialog1.FileName = "AMCProf.txt";

            saveFileDialog1.Title = "AMC Profile File";

            if (saveFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                // user has canceled save
                return;
            }

            FileStream fs;
            byte[] dataArray = new byte[100];
            string[] profileValue = new string[10];
            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();

            cFileHandle fhandle = new cFileHandle();

            fhandle.Open(saveFileDialog1.FileName, out fs, 0);

            string theString = "AMC\nAMC Profile\n";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = DateTime.Now.ToString("MMM/dd/yyyy hh:mm:ss tt\n\n");
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = "Time,Spot1,";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            for (int j = 0; j <7; j++)// (downloadGridView.RowCount - 1); j++)
            {
                dataArray[0] = (byte)'\n';

                fhandle.Write(fs, 1, dataArray);
                //for (int i = 0; i <1; i++)// ProfileGridView.ColumnCount; i++)
                int i = 0;
                {
                    try
                    {
                        profileValue[i] = ProfileGridView[i, j].Value.ToString() + ",";

                        dataArray = encoding.GetBytes(profileValue[i]);

                        fhandle.Write(fs, dataArray.Length, dataArray);
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("Table contains invalid entries",
                                        "Exception Error",
                                        MessageBoxButtons.OK,
                                        MessageBoxIcon.Error
                                       );

                        fhandle.Close(fs);
                        return;
                    }
                }
            }

            fhandle.Close(fs);
        }

        private void LoadProfileBtn_Click(object sender, EventArgs e)
        {
            FileStream fs;
            cFileHandle fhandle = new cFileHandle();
            byte[] dataArray = new byte[100];
            int rowCnt = 0;
            int rowProfileStart = 5;

            String textLine = string.Empty;
            String[] splitLine;

            openFileDialog1.Filter = "text Files (*.txt)|*.txt";
            openFileDialog1.FileName = "";
            //openFileDialog1.InitialDirectory "";
                
            openFileDialog1.Title = "File1";

            if (openFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                /* user has canceled save */
                return;
            }

            if (System.IO.File.Exists(openFileDialog1.FileName))
            {
                try
                {
                    System.IO.StreamReader objReader = new System.IO.StreamReader(openFileDialog1.FileName);

                    do
                    {
                        textLine = objReader.ReadLine();
                        if (textLine != "")
                        {
                            splitLine = textLine.Split(',');

                            if (rowCnt >= rowProfileStart)
                                ProfileGridView[0, (rowCnt- rowProfileStart)].Value = splitLine[0];
                        }

                        rowCnt++;

                    } while (objReader.Peek() != -1);

                    objReader.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show((string)ex.ToString(),
                                    "File Error",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                    );
                }
            }

            //fhandle.Open(openFileDialog1.FileName, out fs, 2);
            //fhandle.Read(fs, dataArray.Length, dataArray);
            //fhandle.Close(fs);
        }

        private void StartFitBtn_Click(object sender, EventArgs e)
        {            
            for (int row = 0; row < 7; row++)
            {
                if (ProfileGridView[0, row].Value == "")
                {
                    MessageBox.Show("No Profile Selected",
                                    "Error",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );
                    return;
                }
                ControllerGridView[2, row].Value = ProfileGridView[0, row].Value;
            }

            if (!BuildSerialMessage((int)PACKET.CMD_GET_PRESS))
            {
                timer1.Enabled = false;
                return;
            }

            ValveNbr = 0x87;

            if (!BuildSerialMessage((int)PACKET.CMD_OPEN_VALVE))
            {
                timer1.Enabled = false;
                return;
            }

            ControllerGridView[2, 7].Value = "0";

            timer1.Enabled = true;

            Refresh();
        }

        private void GetPressBtn_Click(object sender, EventArgs e)
        {
            BuildSerialMessage((int)PACKET.CMD_GET_PRESS);
        }
    }
}
