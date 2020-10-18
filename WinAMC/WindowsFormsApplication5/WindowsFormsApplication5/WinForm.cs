using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Threading;

using STRUCTARRAY;
using ERRORCHECK;
using FileHandling;
using SerialCom;

//using Gigasoft.ProEssentials.Enums;

using System.Runtime.InteropServices; // required for DllImport( string, entrypoint )
//using System.Windows.Devices.Bluetooth;

/* BTFramework libraries */
using wclCommon;
using wclBluetooth;

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
        CMD_GET_PRESS_TEMP  =0x020f,
        CMD_GET_BRD_ID =0x0210,
        CMD_GET_PRESS = 0x0211,

        CMD_SET_AMC_SETUP = 0x2001,
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


        public AMC_OP AmcOp = new AMC_OP();
        public AMC_SETUP AmcSetup = new AMC_SETUP();
                
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

        public UInt16[] PressSensorPsi = new UInt16[8];

        int BoardId = 0;

        private wclBluetoothManager Manager;
        private wclGattClient Client;
        private wclGattClient Client2;

        private wclGattCharacteristic[] FCharacteristics;
        private wclGattDescriptor[] FDescriptors;
        private wclGattService[] FServices;

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Width = 850;
            this.Height = 700;

            InitDataGrid();

            InitDownloadGrid();

            MainPanel.Top = 40;
            MainPanel.Left = 10;
            MainPanel.Width = 1000;
            MainPanel.Height = 600;

            DownloadPanel.Top = MainPanel.Top;
            DownloadPanel.Left = MainPanel.Left;
            DownloadPanel.Width = 700;
            DownloadPanel.Height = 210;

            CommSelectPnl.Top = 10;
            CommSelectPnl.Left = 80;


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

            if (!panelsForm.GetCommStatus())
            {
                FtcStatusStrip.Items[0].Text = "Disconnected";
            }

            /* init BTFramework stuff */
            Manager = new wclBluetoothManager();
            Client = new wclGattClient();
            Client2 = new wclGattClient();
            /*
            Manager.OnNumericComparison += new wclBluetoothNumericComparisonEvent(Manager_OnNumericComparison);
            Manager.OnPasskeyNotification += new wclBluetoothPasskeyNotificationEvent(Manager_OnPasskeyNotification);
            Manager.OnPasskeyRequest += new wclBluetoothPasskeyRequestEvent(Manager_OnPasskeyRequest);
            Manager.OnPinRequest += new wclBluetoothPinRequestEvent(Manager_OnPinRequest);
            */
            Manager.OnDeviceFound += new wclBluetoothDeviceEvent(Manager_OnDeviceFound);
            Manager.OnDiscoveringCompleted += new wclBluetoothResultEvent(Manager_OnDiscoveringCompleted);
            Manager.OnDiscoveringStarted += new wclBluetoothEvent(Manager_OnDiscoveringStarted);

            Client.OnCharacteristicChanged += new wclGattCharacteristicChangedEvent(Client_OnCharacteristicChanged);
            Client.OnConnect += new wclCommunication.wclClientConnectionConnectEvent(Client_OnConnect);
            Client.OnDisconnect += new wclCommunication.wclClientConnectionDisconnectEvent(Client_OnDisconnect);

            Client2.OnCharacteristicChanged += new wclGattCharacteristicChangedEvent(Client_OnCharacteristicChanged);
            Client2.OnConnect += new wclCommunication.wclClientConnectionConnectEvent(Client_OnConnect);
            Client2.OnDisconnect += new wclCommunication.wclClientConnectionDisconnectEvent(Client_OnDisconnect);

            // In real application you should always analize the result code.
            // In this demo we assume that all is always OK.
            Manager.Open();

        }

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

        }

        private void InitProfileGrid()
        {
            ProfileGridView.Width = 300;
            ProfileGridView.Height = 180;
            ProfileGridView.Left = 10;
            ProfileGridView.Top = 20;

            ProfileGridView.ColumnCount = 2;
            ProfileGridView.ColumnHeadersVisible = true;
            ProfileGridView.RowHeadersVisible = true;

            ProfileGridView.Columns[0].Width = 80;
            ProfileGridView.Columns[1].Width = 80;

            /* create the rows */
            for (int i = 1; i < 8; i++)
            {
     //           ProfileGridView.Rows.Add("Parameter " + i.ToString(), "", "", "");
                ProfileGridView.Rows.Add("","", "");
                ProfileGridView.Rows[i-1].HeaderCell.Value = "Target P" + i.ToString();
            }

            ProfileGridView.Columns[0].HeaderText = "Press(psi)";
            ProfileGridView.Columns[1].HeaderText = "Press(psi)";
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

            ControllerGridView.Columns[0].Width = 85;
            ControllerGridView.Columns[1].Width = 85;
            ControllerGridView.Columns[2].Width = 85;
            ControllerGridView.Columns[3].Width = 90;

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
/*
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
*/        

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

       
        private void clientChat()
        {
            while (true)
            {
                Thread.Sleep(1);
            }
        }

        public Thread serialThread;

        public void rxDataThread()
        {
           uint nbrBytesRx = 0;
           uint response = 0;

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
                            case (int)PACKET.CMD_GET_PRESS_TEMP:                               
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
                            case (int)PACKET.CMD_GET_PRESS:
                                break;
                            case (int)PACKET.CMD_SET_AMC_SETUP:         
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

            if( !BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP) )
            {                
                timer1.Enabled = false;
                return;
            }
            Refresh();

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


        private void diagnosticToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            timer1.Enabled = true;

            MainPanel.Visible = false;
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
                case (int)PACKET.CMD_GET_PRESS_TEMP:
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
                case (int)PACKET.CMD_SET_AMC_SETUP:
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
                case (int)PACKET.CMD_GET_PRESS_TEMP:
                    //tempBuf = StructArray.SwapByteArray16(tempBuf);
                    //Pressure    = System.BitConverter.ToSingle(tempBuf, 0);
                    //Temperature = System.BitConverter.ToSingle(tempBuf, 0);
                    for (int j = 0; j < PSensorData.Length; j++)
                    {
                        int sizeofData = System.Runtime.InteropServices.Marshal.SizeOf(PSensorData[0]);

                        PSensorData[j] = (P_SENSOR_DATA)StructArray.ByteArrayToStruct(typeof(P_SENSOR_DATA), tempBuf);

                        System.Array.Copy(tempBuf, sizeofData, tempBuf, 0, (PayloadSize - sizeofData));
                    }
                    break;
                case (int)PACKET.CMD_GET_PRESS:
                    //tempBuf = StructArray.SwapByteArray16(tempBuf);

                    //Pressure    = System.BitConverter.ToSingle(tempBuf, 0);
                    //Temperature = System.BitConverter.ToSingle(tempBuf, 0);
                    for (int j = 0; j < 8; j++)
                    {
                        PressSensorPsi[j] =(UInt16)(tempBuf[1] << 8 | tempBuf[0]);

                        System.Array.Copy(tempBuf, 2, tempBuf, 0, (PayloadSize - 2));
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
            BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP);
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

            theString = "B1,B2,";
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

                        profileValue[i] = ProfileGridView[i+1, j].Value.ToString() + ",";
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
                            {
                                ProfileGridView[0, (rowCnt - rowProfileStart)].Value = splitLine[0];
                                ProfileGridView[1, (rowCnt - rowProfileStart)].Value = splitLine[1];
                            }
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

            if (!BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP))
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
            //BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP);
            BuildSerialMessage((int)PACKET.CMD_GET_PRESS);
        }

/*
**************************************
* BLE MODULE
**************************************
*/        
        private void TraceEvent(Int64 Address, String Event, String Param, String Value)
        {
            String s = "";
            if (Address != 0)
                s = Address.ToString("X12");
            //ListViewItem Item = lvEvents.Items.Add(s);
            //Item.SubItems.Add(Event);
            //Item.SubItems.Add(Param);
            //Item.SubItems.Add(Value);
        }

        void Manager_OnDiscoveringStarted(object Sender, wclBluetoothRadio Radio)
        {
            lvDevices.Items.Clear();
            TraceEvent(0, "Discovering started", "", "");
        }

        void Manager_OnDiscoveringCompleted(object Sender, wclBluetoothRadio Radio, int Error)
        {
            if (lvDevices.Items.Count == 0)
                MessageBox.Show("No BLE devices were found.", "Discovering for BLE devices", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
                // Here we can update found devices names.
                for (Int32 i = 0; i < lvDevices.Items.Count; i++)
                {
                    ListViewItem Item = lvDevices.Items[i];

                    Int64 Address = Convert.ToInt64(Item.Text, 16);
                    String DevName;
                    Int32 Res = Radio.GetRemoteName(Address, out DevName);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        Item.SubItems[1].Text = "Error: 0x" + Res.ToString("X8");
                    else
                        Item.SubItems[1].Text = DevName;

                    if(DevName.Contains("RN4871")) //connect to our ble device
                    {
                        if (Client.State == 0)
                        {
                            Client.Address = Convert.ToInt64(Item.Text, 16);
                            //Client.ConnectOnRead = cbConnectOnRead.Checked;
                            Res = Client.Connect((wclBluetoothRadio)Item.Tag);
                            if (Res != wclErrors.WCL_E_SUCCESS)
                                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                }

            TraceEvent(0, "Discovering completed", "", "");
        }

        void Manager_OnDeviceFound(object Sender, wclBluetoothRadio Radio, long Address)
        {
            wclBluetoothDeviceType DevType = wclBluetoothDeviceType.dtMixed;
            Int32 Res = Radio.GetRemoteDeviceType(Address, out DevType);

            ListViewItem Item = lvDevices.Items.Add(Address.ToString("X12"));

            Item.SubItems.Add(""); // We can not read a device's name here.
            Item.Tag = Radio; // To use it later.

            if (Res != wclErrors.WCL_E_SUCCESS)
            {
                Item.SubItems.Add("Error: 0x" + Res.ToString("X8"));
            }
            else
                switch (DevType)
                {
                    case wclBluetoothDeviceType.dtClassic:
                        Item.SubItems.Add("Classic");
                        break;
                    case wclBluetoothDeviceType.dtBle:
                        Item.SubItems.Add("BLE");
                        break;
                    case wclBluetoothDeviceType.dtMixed:
                        Item.SubItems.Add("Mixed");
                        break;
                    default:
                        Item.SubItems.Add("Unknown");
                        break;
                }

            TraceEvent(Address, "Device found", "", "");
        }
        void Client_OnDisconnect(object Sender, int Reason)
        {
            // Connection property is valid here.
            TraceEvent(((wclGattClient)Sender).Address, "Disconnected", "Reason", "0x" + Reason.ToString("X8"));
        }

        void Client_OnConnect(object Sender, int Error)
        {
            // Connection property is valid here.
            TraceEvent(((wclGattClient)Sender).Address, "Connected", "Error", "0x" + Error.ToString("X8"));

            BleGetServices();

            BleGetCharacteristics();
        }

        void Client_OnCharacteristicChanged(object Sender, ushort Handle, byte[] Value)
        {
            TraceEvent(((wclGattClient)Sender).Address, "ValueChanged", "Handle", Handle.ToString("X4"));
            if (Value == null)
                TraceEvent(0, "", "Value", "");
            else
                if (Value.Length == 0)
                TraceEvent(0, "", "Value", "");
            else
            {
                String Str = "";

                for (Int32 i = 0; i < Value.Length; i++)
                    Str = Str + Value[i].ToString("X2");

                TraceEvent(0, "", "Value", Str);

                RxTextBox.Text = Str;
            }
        }
        private wclBluetoothRadio GetRadio()
        {
            // Look for first available radio.
            for (Int32 i = 0; i < Manager.Count; i++)
                if (Manager[i].Available)
                    // Return first non MS.
                    return Manager[i];

            MessageBox.Show("No one Bluetooth Radio found.", "Error", MessageBoxButtons.OK,
                MessageBoxIcon.Error);

            return null;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            wclBluetoothRadio Radio = GetRadio();
            if (Radio != null)
            {
                Int32 Res = Radio.Discover(10, wclBluetoothDiscoverKind.dkBle);
                if (Res != wclErrors.WCL_E_SUCCESS)
                    MessageBox.Show("Error starting discovering: 0x" + Res.ToString("X8"),
                        "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        
        private void BleConnectBtn_Click(object sender, EventArgs e)
        {            
            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];
                if (Client.State == 0)
                {
                    Client.Address = Convert.ToInt64(Item.Text, 16);
                    //Client.ConnectOnRead = cbConnectOnRead.Checked;
                    Int32 Res = Client.Connect((wclBluetoothRadio)Item.Tag);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    Client2.Address = Convert.ToInt64(Item.Text, 16);
                    //Client.ConnectOnRead = cbConnectOnRead.Checked;
                    Int32 Res = Client2.Connect((wclBluetoothRadio)Item.Tag);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

            }
        }

        private void BleDisconnectBtn_Click(object sender, EventArgs e)
        {
            Int32 Res = Client.Disconnect();
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void BleGetServices()
        {
            FServices = null;

            Int32 Res = Client.ReadServices(wclGattOperationFlag.goNone, out FServices);

            if (Res != wclErrors.WCL_E_SUCCESS)
            {
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (FServices == null)
                return;

            foreach (wclGattService Service in FServices)
            {
                String s;
                if (Service.Uuid.IsShortUuid)
                    s = Service.Uuid.ShortUuid.ToString("X4");
                else
                    s = Service.Uuid.LongUuid.ToString();

                //ListViewItem Item = lvServices.Items.Add(s);
                //Item.SubItems.Add(Service.Uuid.IsShortUuid.ToString());
                //Item.SubItems.Add(Service.Handle.ToString("X4"));
            }
        }

        private void BleGetCharacteristics()
        {
            int servNbr = 0;
            String s ="";

            FCharacteristics = null;

            for (servNbr = 0; servNbr < FServices.Length; servNbr++)
            {
                if (!FServices[servNbr].Uuid.IsShortUuid)
                {
                    s = FServices[servNbr].Uuid.LongUuid.ToString();

                    if (s.Contains("fe7d"))
                    {
                        break;
                    }
                }
            }

            wclGattService Service = FServices[servNbr];// lvServices.SelectedItems[0].Index];
            Int32 Res = Client.ReadCharacteristics(Service, wclGattOperationFlag.goNone, out FCharacteristics);
            if (Res != wclErrors.WCL_E_SUCCESS)
            {
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (FCharacteristics == null)
                return;

            foreach (wclGattCharacteristic Character in FCharacteristics)
            {
                //String s;
                if (Character.Uuid.IsShortUuid)
                    s = Character.Uuid.ShortUuid.ToString("X4");
                else
                {
                    s = Character.Uuid.LongUuid.ToString();

                    if (s.Contains("1e4d"))
                    {
                        break;
                    }
                }
                /*
                ListViewItem Item = lvCharacteristics.Items.Add(s);

                Item.SubItems.Add(Character.Uuid.IsShortUuid.ToString());
                Item.SubItems.Add(Character.ServiceHandle.ToString("X4"));
                Item.SubItems.Add(Character.Handle.ToString("X4"));
                Item.SubItems.Add(Character.ValueHandle.ToString("X4"));
                Item.SubItems.Add(Character.IsBroadcastable.ToString());
                Item.SubItems.Add(Character.IsReadable.ToString());
                Item.SubItems.Add(Character.IsWritable.ToString());
                Item.SubItems.Add(Character.IsWritableWithoutResponse.ToString());
                Item.SubItems.Add(Character.IsSignedWritable.ToString());
                Item.SubItems.Add(Character.IsNotifiable.ToString());
                Item.SubItems.Add(Character.IsIndicatable.ToString());
                Item.SubItems.Add(Character.HasExtendedProperties.ToString());
                */
            }

            BleSubscribeCharacteristics();

            BleWriteCcd();
        }

        private void BleSubscribeCharacteristics()
        {
            wclGattCharacteristic Characteristic = FCharacteristics[0];

            // In case if characteristic has both Indication and Notification properties
            // set to True we have to select one of them. Here we use Notifications but
            // you can use other one.
            if (Characteristic.IsNotifiable && Characteristic.IsIndicatable)
                // Change the code line below to
                // Characteristic.IsNotifiable = false;
                // if you want to receive Indications instead of notifications.
                Characteristic.IsIndicatable = false;
            Int32 Res = Client.Subscribe(Characteristic);
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void BleWriteCcd()        
        {
            wclGattCharacteristic Characteristic = FCharacteristics[0];

            // In case if characteristic has both Indication and Notification properties
            // set to True we have to select one of them. Here we use Notifications but
            // you can use other one.
            if (Characteristic.IsNotifiable && Characteristic.IsIndicatable)
                // Change the code line below to
                // Characteristic.IsNotifiable = false;
                // if you want to receive Indications instead of notifications.
                Characteristic.IsIndicatable = false;
            Int32 Res = Client.WriteClientConfiguration(Characteristic, true, wclGattOperationFlag.goNone, wclGattProtectionLevel.plNone);
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void BleSendData()
        {
            try
            {
                wclGattCharacteristic Characteristic = FCharacteristics[0];// lvCharacteristics.SelectedItems[0].Index];

                String Str = edCharVal.Text;
                if (Str.Length % 2 != 0)
                    Str = "0" + Str;

                Byte[] Val = new Byte[Str.Length / 2];
                for (Int32 i = 0; i < Val.Length; i++)
                {
                    String b = Str.Substring(i * 2, 2);
                    Val[i] = Convert.ToByte(b, 16);
                }

                Int32 Res = Client.WriteCharacteristicValue(Characteristic, Val, wclGattProtectionLevel.plNone);// Protection());
                if (Res != wclErrors.WCL_E_SUCCESS)
                    MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex) { }

        }

        private void BleSendBtn_Click(object sender, EventArgs e)
        {
            BleSendData();
        }

        private void BleRssiBtn_Click(object sender, EventArgs e)
        {
            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];
                Int64 Address = Convert.ToInt64(Item.Text, 16);
                wclBluetoothRadio Radio = (wclBluetoothRadio)Item.Tag;
                SByte Rssi;
                Int32 Res = Radio.GetRemoteRssi(Address, out Rssi);
                if (Res != wclErrors.WCL_E_SUCCESS)
                    MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                else
                    MessageBox.Show("RSSI: " + Rssi.ToString());
            }
        }


        /*
**************************************
* BLE MODULE END
**************************************
*/
    }
}
