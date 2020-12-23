using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Threading;
using System.Diagnostics;

using STRUCTARRAY;
using ERRORCHECK;
using FileHandling;
using SerialCom;

using System.Drawing;


using OpenCvSharp;
using OpenCvSharp.ML;

//using Gigasoft.ProEssentials.Enums;

using System.Runtime.InteropServices; // required for DllImport( string, entrypoint )
//using System.Windows.Devices.Bluetooth;

/* BTFramework libraries */
using wclCommon;
using wclBluetooth;

/*
https://www.servocity.com/high-power-simple-motor-controller-g2-18v25/
SKD for download:
https://www.pololu.com/docs/0J41
*/

namespace WindowsFormsApplication5
{
    enum SERIAL_COMMS
    {
       PORT =0,
       BAUDRATE =9600
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

    enum MC_PACKET
    {
        /* Motor control commands */
        CMD_GET_BRD_TEMP =0x18,
        CMD_GET_CURRENT = 0x2c,

        CMD_EXIT_SAFE_START  =0x83,
        CMD_SET_TRGT_SPEED_FWD = 0x85,
        CMD_SET_TRGT_SPEED_REV =0x86,
        CMD_BRAKE = 0x92,
        CMD_GET_VERSION = 0xc2,
        CMD_STOP_MOTOR = 0xe0,       
        CMD_GET_VAR = 0xA1,

    }

    enum MC_VARS
    {
        GET_FWD_LIMIT = 0x0e,
        GET_REV_LIMIT = 0x12,
        GET_TARGET_SPEED = 0x14,
        GET_MOTOR_SPEED = 0x15,
        GET_VOLTAGE = 0x17,
        GET_BRD_TEMP =0x18,
        GET_CURRENT =0x2c,
    }


    enum CRC
    {
        SEED = 0xffff
    }


    public partial class TheMainForm : Form
    {

        [DllImport("UsbComms.dll", EntryPoint = "OpenUsbPort")]
        public static extern bool OpenUsbPort();

        public SerialFd serialFd = new SerialFd();

        public byte[] Payload = new byte[2000];
        public uint PayloadSize;

        //string server = null;
        //int port = (int)CLIENTS.PORT;

        public int PanelSelect = 0;

        //int dataPt = 0;

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

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public float[] profileValue;

            public UInt16 crc;
        }
                
        //class CLIENT_INFO
        public struct CLIENT_INFO
        {
            public String addr;            
            public int character;
            public int brdId;

            //public wclGattCharacteristic[] FCharacteristics;
            public wclGattCharacteristic FCharacteristics;
        }

        //List<int> Stuff = new List<int>();
        List<CLIENT_INFO> ClientInfo = new List<CLIENT_INFO>();
                

        public AMC_OP AmcOp = new AMC_OP();
        public AMC_SETUP AmcSetup = new AMC_SETUP();
                
        public byte ValveNbr = 0;
        public float Pressure = 0;

        public int MotorSpeed= 0;
        public byte MotorVarId = 0;
        public int MotorCurrent = 0;

        public struct P_SENSOR_DATA
        {
            public float press;
            public float temp;
        }
        public P_SENSOR_DATA[] PSensorData = new P_SENSOR_DATA[8];

        public UInt16[] PressSensorPsi = new UInt16[8];

        public float[] ProValues = new float[14];
        //public float[] ProfileValuesCard1 = new float[7];
        //public float[] ProfileValuesCard2 = new float[7];

        int BoardId = 0;

        private wclBluetoothManager Manager;
        //private wclGattClient Client;
        
        private wclGattClient[] TClient = new wclGattClient[4];

        private wclGattCharacteristic[] FCharacteristics;
        //private wclGattDescriptor[] FDescriptors;
        private wclGattService[] FServices;

        Process p = new Process();

        int CameraState=0;
        bool StartCapture = false;
        
        const string DeviceName = "RN4871";

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

            CommSelectPnl.Top = 10;
            CommSelectPnl.Left = 80;

            InitProfileGrid(ProfileGridView);
            //InitProfileGrid2(ProfileGridView2);

            InitControllerGrid(ControllerGridView);
            InitController2Grid(Controller2GridView);

            FwBrd1VersionLbl.Top = 0;
            FwBrd1VersionLbl.Left = 15;

            FwBrd2VersionLbl.Top = 300;
            FwBrd2VersionLbl.Left = 15;

            StartFitBtn1.Top = 260;
            StartFitBtn1.Left = 15;

            StartFitBtn2.Top = 560;
            StartFitBtn2.Left = StartFitBtn1.Left;

            TestBtn.Top = 260;
            TestBtn.Left = 150;

            //StartPreviewBtn.Top = 560;
            //StartPreviewBtn.Left = 100;

            //CaptureBtn.Top = 560;
            //CaptureBtn.Left = 185;

            //StopCaptureBtn.Top = 560;
            //StopCaptureBtn.Left = 270;

            //UploadFilesBtn.Top = 560;
            //UploadFilesBtn.Left = 355;

            ProfileGroupBox.Top = 50;
            ProfileGroupBox.Left = 50;

            FwBrd1VersionLbl.Visible = false;
            FwBrd2VersionLbl.Visible = false;
            StartFitBtn1.Visible = true;
            StartFitBtn2.Visible = true;
            BrdTypeLbl.Visible = false;

            //FitTypeLoadComboBox.SelectedIndex = 0;

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
            //Client = new wclGattClient();
            /* lets make an arry of clients */
            //wclGattClient[] TClient = new wclGattClient[4];

            /*
            Manager.OnNumericComparison += new wclBluetoothNumericComparisonEvent(Manager_OnNumericComparison);
            Manager.OnPasskeyNotification += new wclBluetoothPasskeyNotificationEvent(Manager_OnPasskeyNotification);
            Manager.OnPasskeyRequest += new wclBluetoothPasskeyRequestEvent(Manager_OnPasskeyRequest);
            Manager.OnPinRequest += new wclBluetoothPinRequestEvent(Manager_OnPinRequest);
            */
            Manager.OnDeviceFound += new wclBluetoothDeviceEvent(Manager_OnDeviceFound);
            Manager.OnDiscoveringCompleted += new wclBluetoothResultEvent(Manager_OnDiscoveringCompleted);
            Manager.OnDiscoveringStarted += new wclBluetoothEvent(Manager_OnDiscoveringStarted);

            //Client.OnCharacteristicChanged += new wclGattCharacteristicChangedEvent(Client_OnCharacteristicChanged);
            //Client.OnConnect += new wclCommunication.wclClientConnectionConnectEvent(Client_OnConnect);
            //Client.OnDisconnect += new wclCommunication.wclClientConnectionDisconnectEvent(Client_OnDisconnect);

            for (int j = 0; j < TClient.Length; j++)
            {
                /* initialize the objects */
                TClient[j] = new wclGattClient();

                TClient[j].OnCharacteristicChanged += new wclGattCharacteristicChangedEvent(Client_OnCharacteristicChanged);
                TClient[j].OnConnect += new wclCommunication.wclClientConnectionConnectEvent(Client_OnConnect);
                TClient[j].OnDisconnect += new wclCommunication.wclClientConnectionDisconnectEvent(Client_OnDisconnect);
            }


            // In real application you should always analize the result code.
            // In this demo we assume that all is always OK.
            Int32 Res =Manager.Open();

            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

        }

        private void InitDataGrid()
        {


        }

        private void InitProfileGrid(DataGridView gridView)
        {
            gridView.Width = 280;
            gridView.Height = 180;
            gridView.Left = 320;
            gridView.Top = 15;

            gridView.ColumnCount = 2;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;

            gridView.Columns[0].Width = 75;
            gridView.Columns[1].Width = 75;

            /* create the rows */
            for (int i = 1; i < 8; i++)
            {
                //           ProfileGridView.Rows.Add("Parameter " + i.ToString(), "", "", "");
                gridView.Rows.Add("","", "");
                gridView.Rows[i-1].HeaderCell.Value = "Target P" + i.ToString();
            }


            gridView.Columns[0].HeaderText = "Press(psi)";
            gridView.Columns[1].HeaderText = "Press(psi)";
            //ProfileGridView.Columns[1].HeaderText = "Value";

            foreach (DataGridViewColumn column in gridView.Columns)
            {
                column.SortMode = DataGridViewColumnSortMode.NotSortable;
            }

            gridView.Visible = false;            
        }


        private void InitProfileGrid2(DataGridView gridView)
        {
            gridView.Width = 220;
            gridView.Height = 180;
            gridView.Left = 300;
            gridView.Top = 220;

            gridView.ColumnCount = 1;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;

            gridView.Columns[0].Width = 80;
            //gridView.Columns[1].Width = 80;

            /* create the rows */
            for (int i = 1; i < 8; i++)
            {
                //           ProfileGridView.Rows.Add("Parameter " + i.ToString(), "", "", "");
                gridView.Rows.Add("", "", "");
                gridView.Rows[i - 1].HeaderCell.Value = "Target P" + i.ToString();
            }

            gridView.Columns[0].HeaderText = "Press(psi)";
            //gridView.Columns[1].HeaderText = "Press(psi)";
            //ProfileGridView.Columns[1].HeaderText = "Value";

            foreach (DataGridViewColumn column in gridView.Columns)
            {
                column.SortMode = DataGridViewColumnSortMode.NotSortable;
            }

            gridView.Visible = true;
        }

        private void InitControllerGrid(DataGridView gridView)
        {
            gridView.Width = 420;
            gridView.Height = 240;
            gridView.Left = 15;
            gridView.Top = 20;

            gridView.ColumnCount = 4;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;
            gridView.RowHeadersWidth = 60;

            gridView.Columns[0].Width = 85;
            gridView.Columns[1].Width = 85;
            gridView.Columns[2].Width = 85;
            gridView.Columns[3].Width = 90;

            /* create the rows */
            for (int i = 1; i < 9; i++)
            {
                gridView.Rows.Add("", "", "");
                gridView.Rows[i - 1].HeaderCell.Value = "S" + i.ToString();
            }

            gridView.Columns[0].HeaderText = "Press (psi)";
            gridView.Columns[1].HeaderText = "Temp (DegC)";
            gridView.Columns[2].HeaderText = "Target (psi)";
            gridView.Columns[3].HeaderText = "Delta P (psi)";

            gridView.Visible = true;
        }

        private void InitController2Grid(DataGridView gridView)
        {
            gridView.Width = 420;
            gridView.Height = 240;
            gridView.Left = 15;
            gridView.Top = 320;

            gridView.ColumnCount = 4;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;
            gridView.RowHeadersWidth = 60;

            gridView.Columns[0].Width = 85;
            gridView.Columns[1].Width = 85;
            gridView.Columns[2].Width = 85;
            gridView.Columns[3].Width = 90;

            /* create the rows */
            for (int i = 1; i < 9; i++)
            {
                gridView.Rows.Add("", "", "");
                gridView.Rows[i - 1].HeaderCell.Value = "S" + i.ToString();
            }

            gridView.Columns[0].HeaderText = "Press (psi)";
            gridView.Columns[1].HeaderText = "Temp (DegC)";
            gridView.Columns[2].HeaderText = "Target (psi)";
            gridView.Columns[3].HeaderText = "Delta P (psi)";

            gridView.Visible = true;
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
        public Thread cameraThread;
        
        public void rxDataThread()
        {
            uint nbrBytesRx = 0;
            uint response = 0;
            Int16 rxValue = 0;

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
                                            catch (Exception ex)
                                            {
                                                MessageBox.Show(ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                            }
                                        }
                                        else
                                        {
                                            ControllerGridView[0, j].Value = "FAULT";
                                            ControllerGridView[1, j].Value = "FAULT";

                                            ControllerGridView[3, j].Value = "FAULT";

                                            ControllerGridView.Rows[j].Cells[0].Style.BackColor = Color.Orange;
                                            ControllerGridView.Rows[j].Cells[1].Style.BackColor = Color.Orange;
                                            ControllerGridView.Rows[j].Cells[2].Style.BackColor = Color.Orange;
                                            ControllerGridView.Rows[j].Cells[3].Style.BackColor = Color.Orange;
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
                                   FwBrd1VersionLbl.Text = "AMC: " + System.Text.Encoding.ASCII.GetString(Payload);
                                   break;
                               case (int)PACKET.CMD_GET_BRD_ID:
                                    BrdTypeLbl.Text ="Board Type: " + BoardId.ToString();
                                   break;                                
                         }
                    }
                }

                if (nbrBytesRx == 2)
                {
                    rxValue =(Int16)(rxBuffer[0] | (rxBuffer[1]<<8));

                    switch (MotorVarId)
                    {
                        case (int)MC_VARS.GET_VOLTAGE:
                            VoltageTextBox.Text = (rxValue/1000.0).ToString("0.00") +"V";
                            label24.Text = "Motor Detected " + VoltageTextBox.Text;
                            MotorVarId = (int)MC_VARS.GET_CURRENT;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_CURRENT:
                            CurrentTextBox.Text = rxValue.ToString() +"mA";
                            MotorVarId = (int)MC_VARS.GET_FWD_LIMIT;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_FWD_LIMIT:
                            if (rxValue != 0)
                                FwdLimitInd.BackColor = Color.Red;
                            else
                                FwdLimitInd.BackColor = Color.LightGray;

                            MotorVarId = (int)MC_VARS.GET_REV_LIMIT;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_REV_LIMIT:
                            if (rxValue != 0)
                                RevLimitInd.BackColor = Color.Red;
                            else
                                RevLimitInd.BackColor = Color.LightGray;

                            MotorVarId = (int)MC_VARS.GET_MOTOR_SPEED;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_MOTOR_SPEED:
                            if (rxValue > 0)
                            {
                                label19.Text = "Dir: Forward";
                                DirFwdInd.BackColor = Color.Green;
                                //DirRevInd.BackColor = Color.LightGray;
                            }
                            else if (rxValue < 0)
                            {
                                label19.Text = "Dir: Reverse";
                                //DirRevInd.BackColor = Color.Green;
                                DirFwdInd.BackColor = Color.Green;
                            }
                            else
                            {
                                label19.Text = "Dir: Stop";
                                DirFwdInd.BackColor = Color.Yellow;
                                DirRevInd.BackColor = Color.Yellow;
                            }

                            SpeedTextBox.Text = rxValue.ToString();
                            break;
                    }
                    
                    //MotorCurrent =(int)(rxBuffer[0] | (rxBuffer[1]<<8));
                    //MotorReadTextBox.Text = MotorCurrent.ToString();
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
        }

        private void setupToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FitTypeComboBox.SelectedIndex = 0;
            DateTextBox.Text = DateTime.Now.ToString("MMM_dd_yyyy hh:mm:ss");
            ProfileGroupBox.Visible = true;
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

            /*
            send via bluetooth
            try
            {
                wclGattCharacteristic Characteristic = FCharacteristics[0];// lvCharacteristics.SelectedItems[0].Index];

                Int32 Res = Client.WriteCharacteristicValue(Characteristic, TxBuf, wclGattProtectionLevel.plNone);// Protection());
                if (Res != wclErrors.WCL_E_SUCCESS)
                    MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex) { }
            */

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
            try
            {
                rxCrc = (UInt16)((UInt16)(rxBuffer[PayloadSize - 2] << 8) | (UInt16)rxBuffer[PayloadSize - 1]);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

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

        public bool BuildMCSerialMessage(byte cmd)
        {
            uint nbrBytes = 0;
            byte[] txBuf = new byte[32];

            txBuf[nbrBytes++] = cmd;

            switch (cmd)
            {
                case (int)MC_PACKET.CMD_GET_BRD_TEMP:
                    break;
                case (int)MC_PACKET.CMD_GET_CURRENT:
                    break;
                case (int)MC_PACKET.CMD_EXIT_SAFE_START:
                    break;
                case (int)MC_PACKET.CMD_SET_TRGT_SPEED_FWD:
                case (int)MC_PACKET.CMD_SET_TRGT_SPEED_REV:
                    txBuf[nbrBytes++] = (byte)(MotorSpeed&0x1f);
                    txBuf[nbrBytes++] = (byte)((MotorSpeed>>5)&0x7f);
                    break;
                case (int)MC_PACKET.CMD_STOP_MOTOR:
                    break;
                case (int)MC_PACKET.CMD_BRAKE:
                    txBuf[nbrBytes++] = 0x20;
                    break;
                case (int)MC_PACKET.CMD_GET_VAR:
                    txBuf[nbrBytes++] = MotorVarId;
                    break;
            }

            if (serialFd.SendMessage(0, nbrBytes, txBuf))
            {
                SerialMonitorTimer.Enabled = true;

                return true;
            }
            else
                return false;
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
            MotorCntrlTimer.Enabled = true;
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

      
        public void UpdateStatusStrip(String aStr)  
        {
            FtcStatusStrip.Items[0].Text = aStr;
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
            saveFileDialog1.FileName = SnTextBox.Text + "_"+FitTypeComboBox.SelectedIndex;// + "_" + DateTextBox.Text +".txt"; // "AMCProf.txt";

            saveFileDialog1.Title = "AMC Profile File";

            saveFileDialog1.InitialDirectory = "C:\\WinAMC\\Profile";

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

            string theString = "Fashion Swipe\nAMC Profile\n";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = DateTime.Now.ToString("MMM/dd/yyyy hh:mm:ss tt\n");
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = NameTextBox.Text +"\n";// "B1,B2,";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = ModelTextBox.Text + "\n";// "B1,B2,";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = SnTextBox.Text + "\n";// "B1,B2,";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = DateTime.Now.ToString("MMM/dd/yyyy hh:mm:ss\n");
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = SizeTextBox.Text + "\n";// "B1,B2,";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            theString = FitTypeComboBox.SelectedIndex.ToString() +"\n";
            dataArray = encoding.GetBytes(theString);
            fhandle.Write(fs, dataArray.Length, dataArray);

            int row = 0;
            /*
            ProfileGridView[0, row++].Value = TorsoUpDown.Value;
            ProfileGridView[0, row++].Value = RightArmUpDown.Value;
            ProfileGridView[0, row++].Value = RightBreastUpDown.Value;
            ProfileGridView[0, row++].Value = BellyUpDown.Value;
            ProfileGridView[0, row++].Value = RightHipUpDown.Value;
            ProfileGridView[0, row++].Value = RightButtockUpDown.Value;
            ProfileGridView[0, row++].Value = RightLegUpDown.Value;

            row = 0;
            ProfileGridView[1, row++].Value = LeftArmUpDown.Value;
            ProfileGridView[1, row++].Value = LeftBreastUpDown.Value;
            ProfileGridView[1, row++].Value = LeftHipUpDown.Value;
            ProfileGridView[1, row++].Value = LeftButtockUpDown.Value;
            ProfileGridView[1, row++].Value = LeftLegUpDown.Value;
            ProfileGridView[1, row++].Value = LeftCalfUpDown.Value;
            ProfileGridView[1, row++].Value = RightCalfUpDown.Value;
            */

            int i = 0;

            ProValues[i++] = (float)TorsoUpDown.Value;
            ProValues[i++] = (float)RightArmUpDown.Value;
            ProValues[i++] = (float)RightBreastUpDown.Value;
            ProValues[i++] = (float)BellyUpDown.Value;
            ProValues[i++] = (float)RightHipUpDown.Value;
            ProValues[i++] = (float)RightButtockUpDown.Value;
            ProValues[i++] = (float)RightLegUpDown.Value;

            ProValues[i++] = (float)LeftArmUpDown.Value;
            ProValues[i++] = (float)LeftBreastUpDown.Value;
            ProValues[i++] = (float)LeftHipUpDown.Value;
            ProValues[i++] = (float)LeftButtockUpDown.Value;
            ProValues[i++] = (float)LeftLegUpDown.Value;
            ProValues[i++] = (float)LeftCalfUpDown.Value;
            ProValues[i++] = (float)RightCalfUpDown.Value;

            for (row = 0; row < 7; row++)
            {
        //        ProfileGridView[0, row].Value = ProValues[row];
        //        ProfileGridView[1, row].Value = ProValues[row+7];
            }


            for (int j = 0; j <7; j++)// (downloadGridView.RowCount - 1); j++)
            {
                //dataArray[0] = (byte)'\n';
                //fhandle.Write(fs, 1, dataArray);
                //for (int i = 0; i <1; i++)// ProfileGridView.ColumnCount; i++)

  
                i = 0;

                {
                    try
                    {
                        profileValue[i] = ProValues[j].ToString()+",";// ProfileGridView[i, j].Value.ToString() + ",";
                        dataArray = encoding.GetBytes(profileValue[i]);

                        fhandle.Write(fs, dataArray.Length, dataArray);

                        profileValue[i] = ProValues[j+7].ToString() + ",\n"; //ProfileGridView[i+1, j].Value.ToString() + ",\n" ;
                        dataArray = encoding.GetBytes(profileValue[i]);

                        fhandle.Write(fs, dataArray.Length, dataArray);

                        //dataArray = BitConverter.GetBytes(pValues[i]);                       
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
            //FileStream fs;
            cFileHandle fhandle = new cFileHandle();
            byte[] dataArray = new byte[100];
            int rowCnt = 0;
            int rowProfileStart = 9;

            String textLine = string.Empty;
            String[] splitLine;

            if (e != null)
            {
                openFileDialog1.Filter = "text Files (*.txt)|*.txt";
                openFileDialog1.FileName = "";
                openFileDialog1.InitialDirectory = "C:\\WinAMC\\Profile";

                openFileDialog1.Title = "File1";

                if (openFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                {
                    /* user has canceled save */
                    return;
                }
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

                            if (rowCnt == (rowProfileStart - 6))
                            {
                                NameTextBox.Text = textLine;
                            }
                            else if (rowCnt == (rowProfileStart - 5))
                            {
                                ModelTextBox.Text = textLine;
                            }
                            else if (rowCnt == (rowProfileStart - 4))
                            {
                                SnTextBox.Text = textLine;
                            }
                            else if (rowCnt == (rowProfileStart - 3))
                            {
                                DateTextBox.Text = textLine;
                            }
                            else if (rowCnt == (rowProfileStart - 2))
                            {
                                SizeTextBox.Text = textLine;
                            }
                            else if (rowCnt == (rowProfileStart - 1))
                            {
                                FitTypeComboBox.SelectedIndex = Convert.ToInt16(textLine);
                            }


                            if (rowCnt >= rowProfileStart)
                            {
                                //         ProfileGridView[0, (rowCnt - rowProfileStart)].Value = splitLine[0];
                                //         ProfileGridView[1, (rowCnt - rowProfileStart)].Value = splitLine[1];

                                ProValues[(rowCnt - rowProfileStart)] = Convert.ToSingle(splitLine[0]);
                                ProValues[7 + (rowCnt - rowProfileStart)] = Convert.ToSingle(splitLine[1]);
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

                    return;
                }


                int row = 0;
                TorsoUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[0, row++].Value.ToString());
                RightArmUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[0, row++].Value.ToString());
                RightBreastUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[0, row++].Value.ToString()); 
                BellyUpDown.Value = new decimal((double)ProValues[row++]);//Decimal.Parse(ProfileGridView[0, row++].Value.ToString());
                RightHipUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[0, row++].Value.ToString());
                RightButtockUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[0, row++].Value.ToString());
                RightLegUpDown.Value = new decimal((double)ProValues[row++]);//Decimal.Parse(ProfileGridView[0, row++].Value.ToString());

                //row = 0;            
                LeftArmUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
                LeftBreastUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
                LeftHipUpDown.Value = new decimal((double)ProValues[row++]);//Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
                LeftButtockUpDown.Value = new decimal((double)ProValues[row++]);//Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
                LeftLegUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
                LeftCalfUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
                RightCalfUpDown.Value = new decimal((double)ProValues[row++]);// Decimal.Parse(ProfileGridView[1, row++].Value.ToString());
            }
            else
            {
                MessageBox.Show("File Error",
                                "File Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                                );

            }

            //fhandle.Open(openFileDialog1.FileName, out fs, 2);
            //fhandle.Read(fs, dataArray.Length, dataArray);
            //fhandle.Close(fs);
        }

        bool StartCameraRequest(int request )
        {
            string filePath = "C:\\WinAMC\\camera\\video\\";
            string fileName = DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") + ".h264";

            string cmdStr = "";

            //p.StartInfo.FileName = "c:\\python3\\python.exe";
            //p.StartInfo.WorkingDirectory = "C:\\Temp\\camerastuff";

            if (request == 0)
            {
          //      cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -dev list";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
                 //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -dev 1.6";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
                cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
            }
            else if(request == 1)
            {
                cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -rgbr 3040 -s depth_raw -dd -sh 2 -nce 1 -v " + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -rgbr 3040 -s color -dd -sh 2 -nce 1 -v " + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -s depth_raw -o";// + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -v " + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";

                //FileSystemWatcher.Path = filePath;
                //FileSystemWatcher.Filter = "*.h264";
                //FileSystemWatcher.NotifyFilter = NotifyFilters.Size;
            }
            else
                cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -dev list";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";\

            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.RedirectStandardOutput = true;

            p.StartInfo.FileName = "c:\\WinAMC\\camera\\py.exe";
            p.StartInfo.Arguments = cmdStr; // "C:\\WinAMC\\camera\\depthai_demo.py -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";

            try
            {
                p.Start();

                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
        }

        public void cameraDataThread()
        {
            string cmdStr ="";
            while(true)
            {
                switch (CameraState)
                {
                    case 0:
                        break;
                    case 1:
                        if (!StartCameraRequest(0))
                            return;

                        if (p.StartInfo.RedirectStandardOutput)
                        {
                            while (!p.StandardOutput.EndOfStream)
                            {
                                string line = p.StandardOutput.ReadLine();
                                if (line.Contains("Started thread for stream: previewout"))
                                {
                                    CaptureBtn.Enabled = true;
                                    break;
                                }
                                if (line.Contains("Started thread for stream: video"))
                                    break;
                                if (line.Contains("Successfully opened stream out"))
                                {
                                    //BleMsgTextBox.Text += line;
                                    CaptureBtn.Enabled = true;
                                }
                                //if (line.Contains("[FOUND]"))
                                //  BleMsgTextBox.Text += line;

                                BleMsgTextBox.Text += line;

                                // do something with line
                            }
                        }


                        p.WaitForExit();

                        if (StartCapture)
                            CameraState = 2;
                        else
                            CameraState = 3;
                        break;
                    case 2:
                        if (!StartCameraRequest(1))
                            return;

                        if (p.StartInfo.RedirectStandardOutput)
                        {
                            while (!p.StandardOutput.EndOfStream)
                            {
                                string line = p.StandardOutput.ReadLine();
                                if (line.Contains("Started thread for stream: previewout"))
                                    break;
                                if (line.Contains("Started thread for stream: video"))
                                    break;
                                //if (line.Contains("Successfully opened stream out"))
                                  //  BleMsgTextBox.Text += line;
                                //if (line.Contains("[FOUND]"))
                                  //  BleMsgTextBox.Text += line;

                                BleMsgTextBox.Text += line;
                                // do something with line
                            }
                        }
                        p.WaitForExit();
                        
                        CameraState = 3;
                        break;
                    case 3:
                        /* report file size/status */
                        CameraState =0;
                        cameraThread.Suspend();
                        break;
                    case 4:
                        if (!StartCameraRequest(2))
                            return;

                        if (p.StartInfo.RedirectStandardOutput)
                        {
                            while (!p.StandardOutput.EndOfStream)
                            {
                                string line = p.StandardOutput.ReadLine();
                                if (line.Contains("Started thread for stream: previewout"))
                                    break;
                                if (line.Contains("Started thread for stream: video"))
                                    break;
                                //if (line.Contains("Successfully opened stream out"))
                                //  BleMsgTextBox.Text += line;
                                //if (line.Contains("[FOUND]"))
                                //  BleMsgTextBox.Text += line;

                                BleMsgTextBox.Text += line;
                                // do something with line
                            }
                        }
                        p.WaitForExit();
                        
                        CameraState = 3;
                        break;
                }
            }                   
        }

        private void StartFitBtn_Click(object sender, EventArgs e)
        {
            for (int row = 0; row < 7; row++)
            {
                //if ((String)ProfileGridView[0, row].Value.ToString() == "")
                if (ProValues[row] == 0)
                {
                    MessageBox.Show("No Profile Selected",
                                    "Error",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );
                    return;
                }
   //             ControllerGridView[2, row].Value = ProfileGridView[0, row].Value;

                ControllerGridView[2, row].Value = ProValues[row];
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

        private void CaptureBtn_Click(object sender, EventArgs e)
        {
            if (StartCapture)
            {
                MessageBox.Show("Capture already in progress", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            try
            {
                p.Kill();
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString(), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                MessageBox.Show("There is no process running, do you forget to perform a preview check", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                return;
            }

            StartCapture = true;

            BleMsgTextBox.Clear();

            CameraState = 1;
        }

        private void StopCaptureBtn_Click(object sender, EventArgs e)
        {
            StartCapture = false;         

            try
            {
                p.Kill();

                CaptureBtn.Enabled = false;
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString(), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                MessageBox.Show("There is no process running", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                return;
            }
        }


        private void StartFitBtn2_Click(object sender, EventArgs e)
        {
            for (int row = 0; row < 7; row++)
            {
                if ((String)ProfileGridView[1, row].Value == "")
                {
                    MessageBox.Show("No Profile Selected",
                                    "Error",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );
                    return;
                }
       //         Controller2GridView[2, row].Value = ProfileGridView[1, row].Value;
                Controller2GridView[2, row].Value = ProValues[7+row];
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

            Controller2GridView[2, 7].Value = "0";

            timer1.Enabled = true;

            Refresh();
        }

        private void CloseProfileBtn_Click(object sender, EventArgs e)
        {
            ProfileGroupBox.Visible = false;
        }

        private void GetPressBtn_Click(object sender, EventArgs e)
        {
            //BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP);
            BuildSerialMessage((int)PACKET.CMD_GET_PRESS);
        }

        private void FileSystemWatcher_Changed(object sender, FileSystemEventArgs e)
        {
            if (e.ChangeType == WatcherChangeTypes.Changed)
            {
                var info = new FileInfo(e.FullPath);
                var theSize = info.Length;
            }
        }

        private void StartPreviewBtn_Click(object sender, EventArgs e)
        {
            if (CameraState == 0)
            {
                cameraThread = new Thread(cameraDataThread);

                cameraThread.Start();
                CameraState = 1;

                BleMsgTextBox.Clear();

                StartCapture = false;
            }
        }

        private void UploadFilesBtn_Click(object sender, EventArgs e)
        {
            var client = new WebClient();
            client.Credentials = new NetworkCredential("jclugston", "Trig2017");

            try
            {
                client.UploadFile("ftp://ftp.drivehq.com//TestDir/test.h264", WebRequestMethods.Ftp.UploadFile, "C:\\WinAMC\\camera\\video\\Test.h264");
            }
            catch (Exception ex)
            {
                MessageBox.Show("File upload failed", "FTP", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            MessageBox.Show("File uploaded", "FTP", MessageBoxButtons.OK, MessageBoxIcon.None);

            //FtpWebRequest request = (FtpWebRequest)WebRequest.Create("ftp://www.contoso.com/test.htm");
        }

        private void BleConnectBtn_Click(object sender, EventArgs e)
        {
            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];

                for (int j = 0; j < ClientInfo.Count; j++)
                {
                    if (ClientInfo[j].addr == Item.SubItems[0].Text)
                    {
                        break;
                    }
                }

                if (TClient[0].State == 0)
                {
                    TClient[0].Address = Convert.ToInt64(Item.Text, 16);
                    //Client.ConnectOnRead = cbConnectOnRead.Checked;
                    Int32 Res = TClient[0].Connect((wclBluetoothRadio)Item.Tag);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                }
            }
        }

        private void BleDisconnectBtn_Click(object sender, EventArgs e)
        {
            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];

                for (int j = 0; j < ClientInfo.Count; j++)
                {
                    if (ClientInfo[j].addr == Item.SubItems[0].Text)
                    {
                        break;
                    }
                }

                Int32 Res = TClient[0].Disconnect();
                if (Res != wclErrors.WCL_E_SUCCESS)
                    MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
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
                else
                {
                    BleMsgTextBox.AppendText("Start Discovering...\r\n");
                    BleMsgTextBox.ScrollToCaret();
                }

            }
        }

        int FindClientInfoAddr(String addrStr)
        {
            int j = 0;

            for (j = 0; j < ClientInfo.Count; j++)
            {
                if (ClientInfo[j].addr == addrStr)
                {
                    break;
                }
            }

            return j;
        }

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

/*
**************************************
* BLE MODULE
**************************************
*/
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
            {
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

                    if (DevName.Contains(DeviceName)) //connect to our ble device
                    {
                        if (TClient[ClientInfo.Count].State == 0)
                        {
                            TClient[ClientInfo.Count].Address = Convert.ToInt64(Item.Text, 16);
                            //Client.ConnectOnRead = cbConnectOnRead.Checked;
                            Res = TClient[ClientInfo.Count].Connect((wclBluetoothRadio)Item.Tag);
                            if (Res != wclErrors.WCL_E_SUCCESS)
                                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                }
            }

            //TraceEvent(0, "Discovering completed", "", "");

            BleMsgTextBox.AppendText("Discovering completed\r\n");
            BleMsgTextBox.ScrollToCaret();
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

            String addrStr = ((wclGattClient)Sender).Address.ToString("X12");

            for (int j = 0; j < lvDevices.Items.Count; j++)
            {
                /* find the row that has the matching address ((wclGattClient)Sender).Address */
                //if (lvDevices.Items[j].SubItems[1].Text.Contains("RN4871"))
                if (lvDevices.Items[j].SubItems[0].Text.Contains(addrStr))
                {
                    lvDevices.Items[0].UseItemStyleForSubItems = false;
                    for (int k = 0; k < 3; k++)
                    {
                        lvDevices.Items[j].SubItems[k].BackColor = Color.White;
                    }
                }
            }

            int res =FindClientInfoAddr(addrStr);
            ClientInfo.RemoveAt(res);

            /*
            for (int j = 0; j < ClientInfo.Count; j++)
            {
                if (ClientInfo[j].addr == addrStr)
                {
                    ClientInfo.RemoveAt(j);
                    break;
                }
            }
            */
        }

        void Client_OnConnect(object Sender, int Error)
        {
            // Connection property is valid here.
            TraceEvent(((wclGattClient)Sender).Address, "Connected", "Error", "0x" + Error.ToString("X8"));

            BleMsgTextBox.AppendText("Connected\r\n");
            BleMsgTextBox.ScrollToCaret();

            String addrStr = ((wclGattClient)Sender).Address.ToString("X12");

            for (int j = 0; j < lvDevices.Items.Count; j++)
            {
                /* find the row that has the matching address ((wclGattClient)Sender).Address */
                //if (lvDevices.Items[j].SubItems[1].Text.Contains("RN4871"))
                if (lvDevices.Items[j].SubItems[0].Text.Contains(addrStr))
                {
                    lvDevices.Items[0].UseItemStyleForSubItems = false;
                    for (int k = 0; k < 3; k++)
                    {
                        lvDevices.Items[j].SubItems[k].BackColor = Color.LightGreen;
                    }
                }
            }

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

                //RxTextBox.Text = Str;
                RxTextBox.Text = System.Text.Encoding.ASCII.GetString(Value);
            }
        }
        private wclBluetoothRadio GetRadio()
        {
            // Look for first available radio.
            for (Int32 i = 0; i < Manager.Count; i++)
                if (Manager[i].Available)
                    // Return first non MS.
                    return Manager[i];

            MessageBox.Show("No Bluetooth Radio found.", "Error", MessageBoxButtons.OK,
                MessageBoxIcon.Error);

            return null;
        }
 
        private void BleGetServices()
        {
            FServices = null;

            BleMsgTextBox.AppendText("Get Services\r\n");
            BleMsgTextBox.ScrollToCaret();

            //Int32 Res = Client.ReadServices(wclGattOperationFlag.goNone, out FServices);
            Int32 Res = TClient[ClientInfo.Count].ReadServices(wclGattOperationFlag.goNone, out FServices);

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
            int characterNbr = 0;
            bool serviceFound = false;
            String s ="";

            FCharacteristics = null;

            for (servNbr = 0; servNbr < FServices.Length; servNbr++)
            {
                if (!FServices[servNbr].Uuid.IsShortUuid)
                {
                    s = FServices[servNbr].Uuid.LongUuid.ToString();

                    /* 49535343-FE7D-4AE5-8FA9-9FAFD205E455.
                     * The Transparent UART Service contains the following data characteristics:
                     • Transparent UART Transmit(TX) Characteristic
                     • Transparent UART Receive(RX) Characteristic
                    */
                    if (s.Contains("fe7d"))
                    {
                        /* find FE7D in service string */
                        serviceFound = true;
                        break;
                    }

                    // 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
                    if (s.Contains("6e400001"))
                    {
                        serviceFound = true;
                        break;
                    }
                }
            }

            if (!serviceFound)
            {
                BleMsgTextBox.AppendText("Our Service Not Found\r\n");
                BleMsgTextBox.ScrollToCaret();
                return;
            }

            BleMsgTextBox.AppendText("Get Characteristics\r\n");
            BleMsgTextBox.ScrollToCaret();

            wclGattService Service = FServices[servNbr];// lvServices.SelectedItems[0].Index];            
            
            /* read the characteristics for this service of interest */
            Int32 Res = TClient[ClientInfo.Count].ReadCharacteristics(Service, wclGattOperationFlag.goNone, out FCharacteristics);
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

                    /* The Transparent UART TX Characteristic is used for data transmission 
                     * by the Server or the Client, so lets find that characteristic
                    */
                    if (s.Contains("1e4d"))
                    {
                        /* Transparent UART TX 49535343-1E4D-4BD9-BA61-23C647249616,
                         * find the 1E4D
                        */
                        break;
                    }

                    /* RX Characteristic UUID: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
                     * TX Characteristic UUID: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
                     * Nordic transparent UART (nRF52840)
                    */
                    if (s.Contains("6e400003"))
                    {
                        /* Transparent UART RX 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
                         * Transparent UART TX 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
                         * find the 6E400002
                        */
                        break;
                    }

                    characterNbr++;
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

            BleSubscribeCharacteristics(characterNbr);

            BleWriteCcd(characterNbr);

            CLIENT_INFO server = new CLIENT_INFO();
            server.addr = TClient[ClientInfo.Count].Address.ToString("X12");
            server.brdId = 1;
            server.character = characterNbr;
            //server.FCharacteristics = FCharacteristics;
            server.FCharacteristics = FCharacteristics[characterNbr];

            ClientInfo.Add(server);

        }

        private void BleSubscribeCharacteristics(int characterNbr)
        {

            BleMsgTextBox.AppendText("Subscribe\r\n");
            BleMsgTextBox.ScrollToCaret();

            wclGattCharacteristic Characteristic = FCharacteristics[characterNbr];

            // In case if characteristic has both Indication and Notification properties
            // set to True we have to select one of them. Here we use Notifications but
            // you can use other one.
            if (Characteristic.IsNotifiable && Characteristic.IsIndicatable)
                // Change the code line below to
                // Characteristic.IsNotifiable = false;
                // if you want to receive Indications instead of notifications.
                Characteristic.IsIndicatable = false;
            //Int32 Res = Client.Subscribe(Characteristic);
            Int32 Res = TClient[ClientInfo.Count].Subscribe(Characteristic);
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void BleWriteCcd(int characterNbr)        
        {
            BleMsgTextBox.AppendText("Write CCCD Subscribe\r\n");
            BleMsgTextBox.ScrollToCaret();

            wclGattCharacteristic Characteristic = FCharacteristics[characterNbr];

            // In case if characteristic has both Indication and Notification properties
            // set to True we have to select one of them. Here we use Notifications but
            // you can use other one.
            if (Characteristic.IsNotifiable && Characteristic.IsIndicatable)
                // Change the code line below to
                // Characteristic.IsNotifiable = false;
                // if you want to receive Indications instead of notifications.
                Characteristic.IsIndicatable = false;
            Int32 Res = TClient[ClientInfo.Count].WriteClientConfiguration(Characteristic, true, wclGattOperationFlag.goNone, wclGattProtectionLevel.plNone);
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void BleSendData(int clientIndex)
        {
            try
            {
                //int writeChar = ClientInfo[clientIndex].character;
                //wclGattCharacteristic Characteristic = ClientInfo[clientIndex].FCharacteristics[writeChar]; // FCharacteristics[ClientInfo[clientIndex].character];// lvCharacteristics.SelectedItems[0].Index];
                wclGattCharacteristic Characteristic = ClientInfo[clientIndex].FCharacteristics; // FCharacteristics[ClientInfo[clientIndex].character];// lvCharacteristics.SelectedItems[0].Index];

                String Str = edCharVal.Text;

                System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
                byte[] dataArray = new byte[32];
                dataArray = encoding.GetBytes(Str);

                if (Str.Length % 2 != 0)
                    Str = "0" + Str;
/*
                Byte[] Val = new Byte[Str.Length / 2];
                for (Int32 i = 0; i < Val.Length; i++)
                {
                    String b = Str.Substring(i * 2, 2);
                    Val[i] = Convert.ToByte(b, 16);
                }
*/
                Int32 Res = TClient[clientIndex].WriteCharacteristicValue(Characteristic, dataArray, wclGattProtectionLevel.plNone);// Protection());
                if (Res != wclErrors.WCL_E_SUCCESS)
                    MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void BleSendBtn_Click(object sender, EventArgs e)
        {
            BleSendData(0);
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

        private void BleMgsTextClear_Click(object sender, EventArgs e)
        {
            BleMsgTextBox.Clear();
        }


/*
**************************************
* BLE MODULE END
**************************************
*/


/*
**************************************
* OPEN CV STUFF
**************************************
*/

        private void button1_Click_1(object sender, EventArgs e)
        {
             
            /*
            FrameSource frameSource;

            //frameSource = Cv2.CreateFrameSource_Camera(0);
            VideoCapture capture;
            Mat frame;

            frame = new Mat();


            for (int j = 0; j < 100; j++)
            {
                capture = new VideoCapture(j);
                capture.Open(j);
                
                if (capture.IsOpened())
              
                  break;
            }
            */
            //            Cv2.NamedWindow("Video", WindowMode.AutoSize);

            /*
            if (capture.IsOpened())
            {
                while (true)
                {
                    if (capture.Read(frame))
                    {
                        Cv2.ImShow("Video", frame);

                        //            // press a key to end execution
                        int c = Cv2.WaitKey(10);
                        if (c != -1) { break; } // Assuming image has focus
                    }
                }
            }
            */
        }
        /*
        **************************************
        * END OPEN CV STUFF
        **************************************
        */


        private void MotorFwdBtn_Click(object sender, EventArgs e)
        {
            BuildMCSerialMessage((int)MC_PACKET.CMD_EXIT_SAFE_START);
            MotorSpeed = Convert.ToInt16(MotorSpeedUpDown.Value);
            BuildMCSerialMessage((int)MC_PACKET.CMD_SET_TRGT_SPEED_FWD);
        }

        private void MotorRevBtn_Click(object sender, EventArgs e)
        {
            BuildMCSerialMessage((int)MC_PACKET.CMD_EXIT_SAFE_START);
            MotorSpeed = Convert.ToInt16(MotorSpeedUpDown.Value);
            BuildMCSerialMessage((int)MC_PACKET.CMD_SET_TRGT_SPEED_REV);
        }

        private void MotorStopBtn_Click(object sender, EventArgs e)
        {
            BuildMCSerialMessage((int)MC_PACKET.CMD_STOP_MOTOR);
        }

        private void MotorReadBtn_Click(object sender, EventArgs e)
        {
            MotorVarId = (int)MC_VARS.GET_VOLTAGE;// GET_REV_LIMIT;// (int)MC_VARS.GET_CURRENT;
            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
        }

        private void CloseControlPanelBtn_Click(object sender, EventArgs e)
        {
            ControlPanel.Visible = false;
            MotorCntrlTimer.Enabled = false;
        }

        private void MotorCntrlTimer_Tick(object sender, EventArgs e)
        {
            MotorVarId = (int)MC_VARS.GET_VOLTAGE;
            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
        }

        private void motorDiagnosticToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ControlPanel.Top = 10;
            ControlPanel.Left = 10;
            ControlPanel.Height = 400;
            ControlPanel.Width = 400;
            ControlPanel.Visible = true;

            MotorCntrlTimer.Enabled = true;
        }

        private void button1_Click_2(object sender, EventArgs e)
        {
            OperationPanel.Top = 10;
            OperationPanel.Left = 10;
            OperationPanel.Height = 580;
            OperationPanel.Width = 450;
            OperationPanel.Visible = true;

            FitGroupBox.Top = 200;
            CameraGroupBox.Top = FitGroupBox.Top + 100;
            MotorGroupBox.Top = CameraGroupBox.Top + 100;

            UploadFilesBtn.Top = MotorGroupBox.Top + 80;
            UploadFilesBtn.Left = 15;
            
            OpCloseBtn.Top = MotorGroupBox.Top + 120;
            OpCloseBtn.Left = 15;

            StartPreviewBtn.Visible = true;            

            MotorVarId = (int)MC_VARS.GET_VOLTAGE;
            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);

            if (CameraState == 0)
            {
                cameraThread = new Thread(cameraDataThread);

                cameraThread.Start();
                CameraState = 4;

                BleMsgTextBox.Clear();

                StartCapture = false;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            MotorFwdBtn_Click(null, null);
        }

        private void RevBtn_Click(object sender, EventArgs e)
        {
            MotorRevBtn_Click(null, null);
        }

        private void StopBtn_Click(object sender, EventArgs e)
        {
            MotorStopBtn_Click(null, null);
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            OperationPanel.Visible = false;
        }

        private void button1_Click_3(object sender, EventArgs e)
        {
            int profCnt = 0;

            if (SnLoadTextBox.Text == "")
            {
                MessageBox.Show("Please enter a serial number", "Profile", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            OpLooseFitBtn.Enabled = false;
            OpNormalFitBtn.Enabled = false;
            OpTightFitBtn.Enabled = false;

            //if (!System.IO.File.Exists("C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_" + FitTypeLoadComboBox.SelectedIndex.ToString() + ".txt"))
            if (!System.IO.File.Exists("C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_0" + ".txt"))
            {
                MessageBox.Show("Loose fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                OpLooseFitBtn.Enabled = true;
                profCnt ++;
            }

            if (!System.IO.File.Exists("C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_1" + ".txt"))
            {
                MessageBox.Show("Normal fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                OpNormalFitBtn.Enabled = true;
                profCnt++;
            }

            if (!System.IO.File.Exists("C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_2" + ".txt"))
            {
                MessageBox.Show("Tight fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                OpTightFitBtn.Enabled = true;
                profCnt++;
            }

            //openFileDialog1.FileName = "C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_" + FitTypeLoadComboBox.SelectedIndex.ToString() + ".txt";

            //LoadProfileBtn_Click(null, null);
          
            MessageBox.Show(profCnt.ToString() + " of 3 profiles for " + SnLoadTextBox.Text +" succesfully loaded", "Profile", MessageBoxButtons.OK);

        }

        private void FitTypeLoadComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            button1_Click_3(null, null);
        }

        private void FitTypeLoadComboBox_SelectionChangeCommitted(object sender, EventArgs e)
        {
            //button1_Click_3(null, null);
        }

        private void OpLooseFitBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = "C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_0.txt";

            LoadProfileBtn_Click(null, null);

            //MessageBox.Show("Profile " + SnLoadTextBox.Text +" " + FitTypeLoadComboBox.Text +" fit succesfully loaded", "Profile", MessageBoxButtons.OK);
        }

        private void OpNormalFitBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = "C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_1.txt";

            LoadProfileBtn_Click(null, null);
        }

        private void OpTightFitBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = "C:\\WinAMC\\Profile\\" + SnLoadTextBox.Text + "_2.txt";

            LoadProfileBtn_Click(null, null);
        }
    }
}
