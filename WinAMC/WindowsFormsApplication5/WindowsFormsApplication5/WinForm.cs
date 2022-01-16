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


//using OpenCvSharp;
//using OpenCvSharp.ML;

//using Gigasoft.ProEssentials.Enums;

using System.Runtime.InteropServices; // required for DllImport( string, entrypoint )
//using System.Windows.Devices.Bluetooth;

/* BTFramework libraries */
using wclCommon;
using wclBluetooth;
using Ionic.Zip;

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

        CMD_GET_VERSION = 0x0200,
        CMD_OPEN_VALVE  =0x020d,
        CMD_CLOSE_VALVE =0x020e,
        CMD_GET_PRESS_TEMP  =0x020f,
        CMD_GET_BRD_ID =0x0210,
        CMD_GET_PRESS = 0x0211,
        CMD_SET_VALVE = 0x0212,

        CMD_START_COMPR = 0x0213,
        CMD_STOP_COMPR = 0x0214,
        CMD_SET_COMPR = 0x0215,

        CMD_ACK =0x9999,
    }

    enum PACKET_XLOAD
    {
        CMD_ACK = 1,
        CMD_ERASE_FLASH = 2,
        CMD_FW_UPDATE_PACKET,
        CMD_FW_UPDATE_COMPLETE,

        CMD_RESET = 7,
        CMD_MEASURE = 8
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

    enum MAIN_DIM
    {
        TOP = 0,
        HEIGHT =720,
    }
    enum CRC
    {
        SEED = 0xffff
    }

    enum BOARD_TYPE
    {
        COMPR_CNTRL =0,
        LO_FLOW_CNTRL = 1,
        HI_FLOW_CNTRL = 2,        
    }

    enum CAMERA_GRID
    {
        STATUS = 0,
        DATA = 1,
        FILE_CNT = 2,
    }

    
    public partial class TheMainForm : Form
    {
        [DllImport("UsbComms.dll", EntryPoint = "OpenUsbPort")]
        public static extern bool OpenUsbPort();

        const string DeviceName = "SwipeFashion";
        const string FileCapturePath = "C:\\WinAMC\\capture\\";
        const string ProfilePath = "C:\\WinAMC\\Profile\\";

        const int MAX_NBR_CAMERAS = 5;

        public SerialFd serialFd = new SerialFd();

        public byte[] Payload = new byte[2000];
        public uint PayloadSize;

        public int PanelSelect = 0;

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

        public struct CLIENT_INFO
        {
            public String addr;
            public int character;
            public int brdId;
            public int tclientNbr;

            //public wclGattCharacteristic[] FCharacteristics;
            public wclGattCharacteristic txCharacteristic;
            public wclGattCharacteristic rxCharacteristic;
        }

        //List<int> Stuff = new List<int>();
        List<CLIENT_INFO> ClientInfo = new List<CLIENT_INFO>();
                               

        public struct motor_cntrl_t
        {
            public int speed;
            public int target;
            public float voltage;
            public int current;
            public byte varId;

            public bool fwdLimit;
            public bool revLimit;

            public bool msgReceived;
            public bool detected;
            public int msgErrCnt;
        }

        motor_cntrl_t MotorCntrl;

        public struct P_SENSOR_DATA
        {
            public float press;
            public float temp;
        }
        public P_SENSOR_DATA[] PSensorData = new P_SENSOR_DATA[8];

        public struct level_sensor_data_t
        {
            public UInt16 outage;
            public UInt16 temp;
        }
        public level_sensor_data_t LevelSensorData;
		
        public UInt16[] PressSensorPsi = new UInt16[8];

        public float[] ProValues = new float[14];

        public byte ValveNbr = 0;
        public byte ComprNbr = 0;

        public bool AckReceived = false;
		
        int BoardId = 0;
        int CompressorState = 0;
        int ReliefState = 0;

        bool LoFlowBoardFitStart = false;
        bool HiFlowBoardFitStart = false;


        private wclBluetoothManager Manager;
        //private wclGattClient Client;
        
        private wclGattClient[] TClient = new wclGattClient[4];

        private wclGattCharacteristic[] FCharacteristics;
        //private wclGattDescriptor[] FDescriptors;
        private wclGattService[] FServices;

        bool StartCapture = false;
        bool PhotoCaptureStarted = false;

        public int NbrCameras =0;
        public int PrevFileCnt = 0;

        public struct camera_info_t
        {
            public Process proc;

            public int state;
            public bool startCapture;

            public bool captureStarted;

            public float fileSize;
            public int fileCnt;

            public string portName;
        }

        camera_info_t[] CameraInfo =new camera_info_t[MAX_NBR_CAMERAS];

        float CapturedFileSize = 0;        
        float FolderSize = 0.0f;
        int FileCnt = 0;

        int NbrProjectDevices = 0;
        bool ProjectDeviceConnect=false;
        int[] ProjectDevices = new int[4];
        bool[] DeviceConnect = new bool[4];
        int DeviceConnectState = 0;

        public Thread[] cameraThread =new Thread[MAX_NBR_CAMERAS];

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
            this.Top = 5;
            this.Left = 20;
            this.Width = 850;
            this.Height = 735;          

            MainPanel.Top = 40;
            MainPanel.Left = 10;
            MainPanel.Width = 1000;
            MainPanel.Height = 700;

            CommSelectPnl.Top = 10;
            CommSelectPnl.Left = 80;

            InitProfileGrid(ProfileGridView);
            //InitProfileGrid2(ProfileGridView2);

            InitControllerGrid(ControllerGridView,20);
            //InitController2Grid(Controller2GridView);
            InitControllerGrid(Controller2GridView,285);

            InitCompressorGrid(CompressorCntrlGridView, 550);

            InitCameraGrid(CameraDataGridView, 480);            

            FwBrd1VersionLbl.Top = 0;
            FwBrd1VersionLbl.Left = 15;

            BrdTypeLbl.Top = FwBrd1VersionLbl.Top;
            BrdTypeLbl.Left = 175;

            FwBrd2VersionLbl.Top = 265;
            FwBrd2VersionLbl.Left = 15;

            Brd2TypeLbl.Top = FwBrd2VersionLbl.Top;
            Brd2TypeLbl.Left = 175;

            FwBrd3VersionLbl.Top = 530;
            FwBrd3VersionLbl.Left = 15;

            Brd3TypeLbl.Top = FwBrd3VersionLbl.Top;
            Brd3TypeLbl.Left = 175;

            StartFitBtn1.Top = 385;
            StartFitBtn1.Left = 465;

            StartFitBtn2.Top = StartFitBtn1.Top;
            StartFitBtn2.Left = StartFitBtn1.Left+ 82;

            TestBtn.Top = StartFitBtn2.Top;
            TestBtn.Left = StartFitBtn2.Left+82;

            ControllerGroupBox.Top = 300;
            ControllerGroupBox.Left = 5;

            ProfileGroupBox.Top = (int)MAIN_DIM.TOP;
            ProfileGroupBox.Left = 5;
            ProfileGroupBox.Height =(int)MAIN_DIM.HEIGHT;
            ProfileGroupBox.Width = 455;

            FwBrd1VersionLbl.Visible = true;
            FwBrd2VersionLbl.Visible = true;
            StartFitBtn1.Visible = true;
            StartFitBtn2.Visible = true;
            BrdTypeLbl.Visible = true;

            //FitTypeLoadComboBox.SelectedIndex = 0;

            //InitGenericPlot(PEGraph, "1", "2", "3");

            PumpSelectComboBox.SelectedIndex = 0;

            PanelSelect = (int)SET_COMMANDS.SET_COMMPORT;

            PanelsFrm panelsForm = new PanelsFrm(this);
            panelsForm.ShowDialog();

            if (!panelsForm.GetCommStatus())
            {
                FtcStatusStrip.Items[0].Text = "Motor Controller Comm Disconnected";
                FtcStatusStrip.BackColor = Color.Orange;
            }
            else
                FtcStatusStrip.BackColor = Color.SkyBlue;

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

        private void InitControllerGrid(DataGridView gridView, int top)
        {
            gridView.Width = 425;
            gridView.Height = 225;
            gridView.Left = 15;
            gridView.Top = top;

            gridView.ColumnCount = 5;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;
            gridView.RowHeadersWidth = 60;

            gridView.Columns[0].Width = 80;
            gridView.Columns[1].Width = 80;
            gridView.Columns[2].Width = 80;
            gridView.Columns[3].Width = 80;
            gridView.Columns[4].Width = 35;

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
            gridView.Columns[4].HeaderText = "V";

            gridView.Visible = true;
        }

        private void InitCompressorGrid(DataGridView gridView, int top)
        {
            gridView.Width = 425;
            gridView.Height = 72;
            gridView.Left = 15;
            gridView.Top = top;

            gridView.ColumnCount = 4;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;
            gridView.RowHeadersWidth = 60;

            gridView.Columns[0].Width = 80;
            gridView.Columns[1].Width = 80;
            gridView.Columns[2].Width = 80;
            gridView.Columns[3].Width = 80;

            // create the rows
            for (int i = 1; i < 2; i++)
            {
                gridView.Rows.Add("", "", "");
                gridView.Rows[i - 1].HeaderCell.Value = "";// "S" + i.ToString();
            }

            gridView.Columns[0].HeaderText = "Hi Comp";
            gridView.Columns[1].HeaderText = "Lo Comp";
            gridView.Columns[2].HeaderText = "Hi Relief";
            gridView.Columns[3].HeaderText = "Lo Relief";

            gridView.Visible = true;
        }

        private void InitCameraGrid(DataGridView gridView, int top)
        {
            gridView.Width = 355;
            gridView.Height = 115;
            gridView.Left = 15;
            gridView.Top = top;

            gridView.ColumnCount = 3;
            gridView.ColumnHeadersVisible = true;
            gridView.RowHeadersVisible = true;
            gridView.RowHeadersWidth = 55;

            gridView.Columns[(int)CAMERA_GRID.STATUS].Width = 140;
            gridView.Columns[(int)CAMERA_GRID.DATA].Width = 80;
            gridView.Columns[(int)CAMERA_GRID.FILE_CNT].Width = 75;

            // create the rows
            for (int i = 1; i < 5; i++)
            {
                gridView.Rows.Add("", "", "");
                gridView.Rows[i - 1].HeaderCell.Value = "C" + i.ToString();
            }

            gridView.Columns[(int)CAMERA_GRID.STATUS].HeaderText = "Status";
            gridView.Columns[(int)CAMERA_GRID.DATA].HeaderText = "Data(MB)";
            gridView.Columns[(int)CAMERA_GRID.FILE_CNT].HeaderText = "Files";

            gridView.Visible = true;
        }

   /*     
                private void InitGenericPlot(Gigasoft.ProEssentials.Pesgo pePlot, string title_,          string xaxisLbl_,                    string yaxisLbl_            )
                {
                    pePlot.PeFunction.Reset();

                    //pePlot.PeGrid.GridBands = false;

                    pePlot.PeConfigure.PrepareImages = true;
                    pePlot.Left = 20;// 20;
                    pePlot.Top = 25;// 490;
                    pePlot.Width = 610;// 600;
                    pePlot.Height = 375;
                    pePlot.BringToFront();

                    pePlot.PeData.Subsets = 2;
                    pePlot.PeData.Points = 1000;

                    pePlot.PeString.SubsetLabels[0] = "G Vol";// "Range";
                    pePlot.PeString.SubsetLabels[1] = "G Heel";
                    //        pePlot.PeString.SubsetLabels[2] = "";

                    pePlot.PeString.YAxisLabel = "Liters";
                    pePlot.PeGrid.Configure.ManualMinY = 0;
                    pePlot.PeGrid.Configure.ManualMaxY = 10000;

                    pePlot.PeLegend.SubsetColors[0] = System.Drawing.Color.DarkBlue;
                    pePlot.PeLegend.SubsetLineTypes[0] =LineType.MediumSolid;

                    pePlot.PeLegend.SubsetColors[1] = System.Drawing.Color.Red;
                    pePlot.PeLegend.SubsetLineTypes[1] = LineType.ThinSolid;

            // Set Manual Y scale
            //pePlot.PeGrid.Configure.ManualScaleControlY = ManualScaleControl.MinMax;

                    pePlot.PeLegend.SubsetColors[2] = System.Drawing.Color.Red;
//                    pePlot.PeLegend.SubsetLineTypes[2] = LineType.ThickSolid;


                    pePlot.PeString.MainTitle = title_;
                    pePlot.PeString.SubTitle = "";

                    // Set Manual X scale
                //    pePlot.PeGrid.Configure.ManualScaleControlX = ManualScaleControl.MinMax;
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
           //         pePlot.PeUserInterface.Cursor.PromptStyle = CursorPromptStyle.XYValues;
            //        pePlot.PeUserInterface.Menu.MultiAxisStyle = MenuControl.Show;

             //       pePlot.PeUserInterface.Allow.Zooming = AllowZooming.HorzAndVert;
             //       pePlot.PeUserInterface.Allow.ZoomStyle = ZoomStyle.Ro2Not;

                    pePlot.PePlot.Option.GradientBars = 12;

                    pePlot.PeColor.BitmapGradientMode = false;
             //       pePlot.PeColor.QuickStyle = QuickStyle.LightInset;
               //     pePlot.PeLegend.Style = LegendStyle.OneLineTopOfAxis;
               //     pePlot.PeUserInterface.Menu.ShowLegend = MenuControl.Show;
               //     pePlot.PeUserInterface.Menu.LegendLocation = MenuControl.Show;

               //     pePlot.PeFont.FontSize = FontSize.Large;

                    pePlot.PeLegend.SimplePoint = true;
                    pePlot.PeLegend.SimpleLine = true;
               //     pePlot.PeLegend.Style = LegendStyle.OneLineInsideOverlap;

                    //    FuelLevelStripChart.PeColor.GraphForeground = Color.FromArgb(50, 0, 0, 0);
                //    pePlot.PeGrid.LineControl = GridLineControl.Both;
               //     pePlot.PeGrid.Style = GridStyle.Dot;
               //     pePlot.PeConfigure.BorderTypes = TABorder.SingleLine;

                    pePlot.PeData.NullDataValueX = 0;

                    pePlot.PeUserInterface.HotSpot.Data = false;
                 //   pePlot.PeUserInterface.HotSpot.Size = HotSpotSize.Large;

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

        /* threads */    
        public Thread serialThread;
        //public Thread cameraThread;
        //cameraThread = new Thread(cameraDataThread);
        public Thread MotorThread;

        public void rxDataThread()
        {
            uint nbrBytesRx = 0;
            uint response = 0;
            Int16 rxValue = 0;

            //float delatP = 0.0F;

            while (true)
            {
                byte[] rxBuffer = new byte[1056];

                nbrBytesRx = serialFd.ReceiveMessage(0, 100, rxBuffer);

                if (nbrBytesRx != 0)
                {
                    SerialMonitorTimer.Enabled = false;

                    /*
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
                                        BuildSerialMessage((int)PACKET.CMD_CLOSE_VALVE,0);
                                    break;
                                case (int)PACKET.CMD_GET_PRESS:
                                    break;
                                case (int)PACKET.CMD_SET_AMC_SETUP:         
                                   break;
                               case (int)PACKET.CMD_GET_VERSION:
                                   FwBrd1VersionLbl.Text = "AMC: v" + System.Text.Encoding.ASCII.GetString(Payload);
                                   break;
                               case (int)PACKET.CMD_GET_BRD_ID:
                                   BrdTypeLbl.Text ="Board Type: " + BoardId.ToString();
                                   break;                                
                         }
                    }
                    */
                }

                if (nbrBytesRx == 2)
                {
                    MotorCntrl.msgReceived = true;

                    rxValue = (Int16)(rxBuffer[0] | (rxBuffer[1] << 8));

                    switch (MotorCntrl.varId)
                    {
                        case (int)MC_VARS.GET_VOLTAGE:
                            MotorCntrl.voltage = (float)rxValue / 1000.0F;

                            VoltageTextBox.Text = (rxValue / 1000.0).ToString("0.00") + "V";
                            VoltageLbl.Text = "Voltage: " + (rxValue / 1000.0).ToString("0.00") + "V";
                            MotorDetectLbl.Text = "Motor Detected: " + VoltageTextBox.Text;

                            if (MotorCntrl.voltage < 10.0)
                                MotorDetectLbl.ForeColor = Color.Red;
                            else
                                MotorDetectLbl.ForeColor = Color.Black;

                            MotorCntrl.voltage = rxValue;

                            MotorCntrl.varId = (int)MC_VARS.GET_CURRENT;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_CURRENT:
                            CurrentTextBox.Text = rxValue.ToString() + "mA";
                            CurrentLbl.Text = "Current: " + rxValue.ToString() + "mA";
                            MotorCntrl.varId = (int)MC_VARS.GET_FWD_LIMIT;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_FWD_LIMIT:
                            if (rxValue != 0)
                            {
                                FwdLimitInd.BackColor = Color.Red;
                                MotorCntrl.fwdLimit = true;
                            }
                            else
                            {
                                FwdLimitInd.BackColor = Color.LightGray;
                                MotorCntrl.fwdLimit = false;
                            }

                            MotorCntrl.varId = (int)MC_VARS.GET_REV_LIMIT;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                        case (int)MC_VARS.GET_REV_LIMIT:
                            if (rxValue != 0)
                            {
                                RevLimitInd.BackColor = Color.Red;
                                MotorCntrl.revLimit = true;
                            }
                            else
                            {
                                RevLimitInd.BackColor = Color.LightGray;
                                MotorCntrl.revLimit = false;
                            }

                            MotorCntrl.varId = (int)MC_VARS.GET_MOTOR_SPEED;
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

                            MotorCntrl.speed = rxValue;

                            SpeedTextBox.Text = rxValue.ToString();
                            SpeedLbl.Text = "Speed:   " + rxValue.ToString();

                            MotorCntrl.varId = (int)MC_VARS.GET_VOLTAGE;
                            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
                            break;
                    }

                    //MotorCurrent =(int)(rxBuffer[0] | (rxBuffer[1]<<8));
                    //MotorReadTextBox.Text = MotorCurrent.ToString();
                }

                Thread.Sleep(1);
            }
        }

        public void cameraDataThread(object cameraNbr)
        {            
            while (true)
            {
                if (!CameraMachine((int)cameraNbr)) //CameraInfo[0]))
                {
                    cameraThread[(int)cameraNbr].Abort(cameraNbr);                  
                }
            }
        }


        public void MotorDataThread()
        {
            int motorState = 0;

            while (true)
            {
                switch (motorState)
                {
                    case 0:
                        if (MotorCntrl.fwdLimit || MotorCntrl.revLimit)
                        {
                            motorState = 1;
                            CameraOperationLbl.Text = "";
                            MotorStatusLbl.Text = "Mannequin ready";
                        }
                        else
                        {
                            MotorStatusLbl.Text = "Positioning mannequin";
                            MotorFwdBtn_Click(null, null);
                        }
                        break;
                    case 1:
                        //if (CameraInfo[0].captureStarted)
                        if( PhotoCaptureStarted )
                        {
                            if (MotorCntrl.fwdLimit)
                                MotorRevBtn_Click(null, null);
                            else if (MotorCntrl.revLimit)
                                MotorFwdBtn_Click(null, null);
                            else
                            {
                                /*ERROR, no limit anymore??? */
                                MessageBox.Show("Mannequin out of position",
                                                "Error",
                                                MessageBoxButtons.OK,
                                                MessageBoxIcon.Error
                                               );

                                return;

                            }

                            MotorStatusLbl.Text = "Rotating mannequin";

                            motorState = 2;
                        }
                        break;
                    case 2:
                        if (!MotorCntrl.fwdLimit && !MotorCntrl.revLimit)
                        {
                            motorState = 3;
                        }
                        break;
                    case 3:
                        if (MotorCntrl.fwdLimit || MotorCntrl.revLimit)
                        {
                            motorState = 0;

                            MotorStatusLbl.Text = "Limit reached";

                            StopCaptureBtn_Click(null, null);

                            MotorThread.Abort();
                        }
                        break;
                }

                Thread.Sleep(1);
            }
        }

        bool StartCameraRequest(int request, int camera)
        {
            //Proc[camera] = new Process();
            CameraInfo[camera].proc = new Process();            

            CapturedFileSize = 0;

            //string filePath = "C:\\WinAMC\\camera\\video\\";
            string filePath = FileCapturePath;// "C:\\WinAMC\\capture\\";
            //fileName = DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") + ".h264";
            if (!System.IO.Directory.Exists(filePath))
                System.IO.Directory.CreateDirectory(filePath);

            string cmdStr = "";

            //p.StartInfo.FileName = "c:\\python3\\python.exe";
            //p.StartInfo.WorkingDirectory = "C:\\Temp\\camerastuff";

            if (request == 0)
            {
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -dev list";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";                
                
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";

                /* does not work on win10, but will need for multiple cameras */
                cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -dev " + CameraInfo[camera].portName + " -pos " + (camera+1); //"1.6";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
            }
            else if (request == 1)
            {
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -rgbr 3040 -s depth -dd -sh 2 -nce 1";// -v " + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -rgbr 3040 -s color -dd -sh 2 -nce 1";// + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";
                cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -rgbr 3040 -s color -dev " + CameraInfo[camera].portName + " -pos " + (camera + 1);
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -s depth_raw -o";// + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";
                //cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -v " + filePath + fileName;// C:\\WinAMC\\camera\\video\\" +DateTime.Now.ToString("MMM_dd_yyyy_hh_mm_ss") +".h264";

                /*
                FileSystemWatcher.Path = filePath;
                FileSystemWatcher.Filter = "*.png";
                FileSystemWatcher.NotifyFilter = NotifyFilters.Size;

                FileMonitorTimer.Enabled = true;
                */
            }
            else
                cmdStr = "C:\\WinAMC\\camera\\depthai_demo.py -dev list";// -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";\
            /*
            Proc[camera].StartInfo.UseShellExecute = false;
            Proc[camera].StartInfo.CreateNoWindow = true;
            Proc[camera].StartInfo.RedirectStandardOutput = true;

            Proc[camera].StartInfo.FileName = "c:\\WinAMC\\camera\\py.exe";
            Proc[camera].StartInfo.Arguments = cmdStr; // "C:\\WinAMC\\camera\\depthai_demo.py -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";
            */
            CameraInfo[camera].proc.StartInfo.UseShellExecute = false;
            CameraInfo[camera].proc.StartInfo.CreateNoWindow = true;
            CameraInfo[camera].proc.StartInfo.RedirectStandardOutput = true;
            CameraInfo[camera].proc.StartInfo.FileName = "c:\\WinAMC\\camera\\py.exe";
            CameraInfo[camera].proc.StartInfo.Arguments = cmdStr; // "C:\\WinAMC\\camera\\depthai_demo.py -v C:\\WinAMC\\camera\\vid.h264";// -s jpegout"; //"C:\\temp\\camerastuff\\depthai_demo.py";

            CameraInfo[camera].proc.OutputDataReceived += SortOutputHandler;

            try
            {
                //Proc[camera].Start();
                bool status =CameraInfo[camera].proc.Start();

                CameraInfo[camera].proc.BeginOutputReadLine();
                return true;
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString(), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                MessageBox.Show("Camera app failed to start", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

        }

        public void SortOutputHandler(object sendingProcess,DataReceivedEventArgs outLine)
        {            
            if (!String.IsNullOrEmpty(outLine.Data))
            {
                string line = outLine.Data;

                Process sp = (Process)sendingProcess;

                int cameraId = 0;
                
                for (cameraId = 0; cameraId < NbrCameras; cameraId++)
                {
                    if (CameraInfo[cameraId].proc.Id == sp.Id)
                        break;
                }

                if (line.Contains("required")||
                    line.Contains("installed") 
                   )
                {                
                    BleMsgTextBox.Text += line;
                }
                if (line.Contains("Detected"))
                {
                    CameraStatusLbl.Text = "Camera: " + line;

                    BleMsgTextBox.Text += line;
                }
                if (line.Contains("on USB port:"))
                {
                    int index = line.IndexOf(": ");
                    CameraInfo[NbrCameras].portName = line.Substring(index + 2, 3);// [index+1];

                    CameraPortLbl.Text += "Port: " + CameraInfo[NbrCameras].portName + " ";

                    NbrCameras++;

                    BleMsgTextBox.Text += line;
                }
                if (line.Contains("Successfully opened stream out"))
                {
                    CaptureBtn.Enabled = true;

                    if (CameraInfo[cameraId].state == 1)
                    {
                        CameraOperationLbl.Text = "Camera preview started";
                        CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraId].Value = "Preview started";
                    }
                    else
                    {
                        CameraOperationLbl.Text = "Camera Capture started";
                        CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraId].Value = "Capture started";
                    }

                    BleMsgTextBox.Text += line;
                }
                if (line.Contains("Started thread for stream: color"))
                {                   
                    CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraId].Value = "Capture started2";
                        
                    CameraOperationLbl.Text = "Camera capture started";

                    BleMsgTextBox.Text += line;
                }
                if (line.Contains("failed"))
                {    
                    CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraId].Value = "FAILED";

                    BleMsgTextBox.Text += line;
                }
                if (line.Contains("Failed to boot"))
                {
                    CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraId].Value = "Failed to boot";
                }

                if (NbrCameras == 0)
                    CameraStatusLbl.ForeColor = Color.Red;
                else
                    CameraStatusLbl.ForeColor = Color.Black;

               // try
                {
                 //   BleMsgTextBox.Text += line;
                }
                //catch(Exception ex){ }
            }

        }

        public bool CameraMachine(int cameraNbr)//out camera_info_t cameraInfo)
        {
            //camera_info_t cameraInfo = CameraInfo[cameraNbr];

            //while (true)
            {
                switch (CameraInfo[cameraNbr].state)// CameraState[cameraNbr])
                {
                    case 0:
                        PhotoCaptureStarted = false;
                        CameraInfo[cameraNbr].captureStarted = false;                        
                        //cameraThread.Abort(cameraNbr);
                        // cameraThread.Suspend();
                        break;
                    case 1:
                        if (!StartCameraRequest(0, cameraNbr))
                            return false;

                        Thread.Sleep(250);
                        CameraOperationLbl.Text = "Start Camera Preview";
                        CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraNbr].Value = "Preview";

                        PhotoCaptureStarted = false;
                        CameraInfo[cameraNbr].captureStarted = false;

                        CameraInfo[cameraNbr].proc.WaitForExit();
                        if (StartCapture)
                            CameraInfo[cameraNbr].state = 2;// CameraState[cameraNbr] = 2;
                        else
                            CameraInfo[cameraNbr].state = 5;

                        Thread.Sleep(100);
                        break;
                    case 2:
                        if (!StartCameraRequest(1, cameraNbr))
                            return false;

                        Thread.Sleep(100);

                        CameraOperationLbl.Text = "Start Camera Capture";
                        CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraNbr].Value = "Start Capture";                       

                        /*
                        if (CameraInfo[cameraNbr].proc.StartInfo.RedirectStandardOutput)
                        {
                            while(!CameraInfo[cameraNbr].proc.StandardOutput.EndOfStream)
                            {
                                string line = CameraInfo[cameraNbr].proc.StandardOutput.ReadToEnd();// ReadLine();
                                //if (line.Contains("Started thread for stream: previewout"))
                                    //break;
                                //if (line.Contains("Started thread for stream: video"))
                                    //break;
                                //if (line.Contains("Started thread for stream: color"))
                                    //break;

                                //            if (line.Contains("Successfully opened stream out"))
                                //   break;
                                //  BleMsgTextBox.Text += line;
                                //if (line.Contains("[FOUND]"))
                                //  BleMsgTextBox.Text += line;

                                try
                                {
                                    BleMsgTextBox.Text += line;
                                }
                                catch (Exception ex) { }

                                Thread.Sleep(1);
                                // do something with line
                            }
                        }
                        */

                        //CameraOperationLbl.Text = "Camera capture started";
                        //CameraDataGridView[0, cameraNbr].Value = "Capture started";

                        //CameraMsgTextBox.Text += "Camera" + (cameraNbr + 1).ToString() + " capture started\n";

                        CameraInfo[cameraNbr].captureStarted = true;
                        CameraInfo[cameraNbr].proc.WaitForExit();
                        CameraInfo[cameraNbr].state = 3;
                        break;
                    case 3:
                        /* report file size/status */
                        CameraOperationLbl.Text = "Camera capture complete: " + CapturedFileSize.ToString("0.00") + "MB\nFile Count: " +FileCnt;
                        CameraDataGridView[(int)CAMERA_GRID.STATUS, cameraNbr].Value = "Capture complete";                        

                        CameraInfo[cameraNbr].state = 10;
                        break;
                    case 5:
                        /* report file size/status */
                        CameraOperationLbl.Text = "Camera" + (cameraNbr + 1).ToString() + " preview complete: " + CapturedFileSize;
                        //CameraMsgTextBox.Text += "Camera" + (cameraNbr + 1).ToString() + " preview complete\n";
                        CameraInfo[cameraNbr].state = 10;
                        break;
                    case 6:
                        /* report file size/status */
                        CameraOperationLbl.Text = "Camera detect complete: " + CapturedFileSize;                        
                        CameraInfo[cameraNbr].state = 10;
                        break;
                    case 10:
                        CameraInfo[cameraNbr].state = 0;
                        cameraThread[cameraNbr].Abort(cameraNbr);
                        break;
                    case 4:
                        if (!StartCameraRequest(2, cameraNbr))
                            return false;

                        NbrCameras = 0;
                        CameraPortLbl.Text = "";

                        //if (Proc[0].StartInfo.RedirectStandardOutput)
                        if (CameraInfo[cameraNbr].proc.StartInfo.RedirectStandardOutput)
                        {
                            /*
                            while (!CameraInfo[cameraNbr].proc.StandardOutput.EndOfStream)
                            {                               
                                string line = CameraInfo[cameraNbr].proc.StandardOutput.ReadLine();// Proc[0].StandardOutput.ReadLine();

                                if (line.Contains("Detected"))
                                {
                                    CameraStatusLbl.Text = "Camera: " + line;
                                }
                                if (line.Contains("on USB port:"))
                                {
                                    int index = line.IndexOf(": ");
                                    CameraPortName[NbrCameras] = line.Substring(index + 2, 3);// [index+1];

                                    CameraPortLbl.Text += "Port: " + CameraPortName[NbrCameras] +" ";

                                    NbrCameras++;
                                }


                                BleMsgTextBox.Text += line;
                                // do something with line
                            }
                            */
                        }

                        if (NbrCameras == 0)
                            CameraStatusLbl.ForeColor = Color.Red;
                        else
                            CameraStatusLbl.ForeColor = Color.Black;

                        CameraInfo[cameraNbr].proc.WaitForExit();

                        CameraInfo[cameraNbr].state = 6;
                        break;
                }
            }

            return true;
        }
        public byte DisplayGridData(DataGridView gridView, bool brdFitStart)
        {
            float delatP = 0.0F;
            byte vNbr = 0;

            for (int j = 0; j < 8; j++)
            {
                delatP = (float)(PressSensorPsi[j] - PressSensorPsi[7]) / (float)1000;

                if (PressSensorPsi[j] != 0)
                {
                    gridView[0, j].Value = ((float)PressSensorPsi[j] / 1000).ToString("0.000");
                    //gridView[1, j].Value = (PSensorData[j].temp).ToString("0.0");

                    gridView[3, j].Value = delatP.ToString("0.000");

                    gridView.Rows[j].Cells[0].Style.BackColor = Color.White;
                    gridView.Rows[j].Cells[1].Style.BackColor = Color.White;
                    gridView.Rows[j].Cells[2].Style.BackColor = Color.White;
                    gridView.Rows[j].Cells[3].Style.BackColor = Color.White;

                    try
                    {
                        if (delatP < Convert.ToSingle(gridView[2, j].Value))
                            vNbr |= (byte)(0x01 << j);
                    }
                    catch (Exception ex)
                    {
                        //                     MessageBox.Show(ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                else
                {
                    gridView[0, j].Value = "FAULT";
                    gridView[1, j].Value = "FAULT";

                    gridView[3, j].Value = "FAULT";

                    gridView.Rows[j].Cells[0].Style.BackColor = Color.Orange;
                    gridView.Rows[j].Cells[1].Style.BackColor = Color.Orange;
                    gridView.Rows[j].Cells[2].Style.BackColor = Color.Orange;
                    gridView.Rows[j].Cells[3].Style.BackColor = Color.Orange;
                    gridView.Rows[j].Cells[4].Style.BackColor = Color.Orange;
                }

                if (!brdFitStart)
                    vNbr = 0;

                if ( (vNbr & (0x01<<j)) ==(0x01<<j) )
                    gridView[4, j].Value ="O";
                else
                    gridView[4, j].Value = "X";
            }

            return vNbr;

        }
        public byte DisplayComprGridData(DataGridView gridView)
        {
            byte cNbr = 0;

            if ( (CompressorState &0x0010) ==0x0010)
                gridView[0, 0].Value ="O";
            else
                gridView[0, 0].Value = "X";

            if ((CompressorState & 0x0008) == 0x0008)
                gridView[1, 0].Value = "O";
            else
                gridView[1, 0].Value = "X";

            if ((ReliefState & 0x0040) == 0x0040)
                gridView[2, 0].Value = "O";
            else
                gridView[2, 0].Value = "X";

            if ((ReliefState & 0x0020) == 0x0020)
                gridView[3, 0].Value = "O";
            else
                gridView[3, 0].Value = "X";

            if (LoFlowBoardFitStart)
                cNbr |= 0x01;
            if (HiFlowBoardFitStart)
                cNbr |= 0x02;

            return cNbr;
        }


        private void timer1_Tick(object sender, EventArgs e)
        {
            SerialMonitorTimer.Enabled = true;

            if( !BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP, 0) )
            {                
                timer1.Enabled = false;

                MessageBox.Show("Error: BLE comms", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
            int j = 0;

            FitTypeComboBox.SelectedIndex = 0;
            DateTextBox.Text = DateTime.Now.ToString("MMM_dd_yyyy hh:mm:ss");

            var foldersFound = Directory.GetDirectories(ProfilePath);// "C:\\WinAMC\\Profile\\");
            SnComboBox.Items.Clear();
            for (j = 0; j < foldersFound.Length; j++)
            {
                string result = Path.GetFileName(foldersFound[j]);
                SnComboBox.Items.Add(result);
            }

            try
            {
                SnComboBox.SelectedIndex = 0;
            }
            catch (Exception ex)
            {
                MessageBox.Show("No profiles found", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }

            ProfileGroupBox.Visible = true;
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox1 aboutBox = new AboutBox1();
            aboutBox.ShowDialog();
        }
   
        public bool BuildSerialMessage(UInt16 command, int addr)
        {
            uint nbrBytes = 0;
            uint nbrBytesToTx = 0;

            AckReceived = false;

            byte[] TxBuf = new byte[255];
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
                case (int)PACKET.CMD_SET_VALVE:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = ValveNbr;

                    TxBuf[nbrBytes++] = Payload[0];
                    break;
                case (int)PACKET.CMD_SET_COMPR:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = ComprNbr;

                    TxBuf[nbrBytes++] = Payload[0];
                    break;
                case (int)PACKET.CMD_START_COMPR:
                    // build the message here, then send

                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = ComprNbr;

                    TxBuf[nbrBytes++] = Payload[0];
                    break;
                case (int)PACKET.CMD_STOP_COMPR:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = ComprNbr;

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
                case (int)PACKET_XLOAD.CMD_ERASE_FLASH:
                case (int)PACKET_XLOAD.CMD_RESET:				
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET_XLOAD.CMD_FW_UPDATE_PACKET:
                case (int)PACKET_XLOAD.CMD_FW_UPDATE_COMPLETE:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    for (int j = 0; j < Payload.Length; j++)
                    {
                        TxBuf[nbrBytes++] = Payload[j];
                    }
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
            send via bluetooth*/
            try
            {
                wclGattCharacteristic Characteristic = ClientInfo[addr].rxCharacteristic;

                Array.Resize<byte>(ref TxBuf, (int)nbrBytes);
                Int32 Res = TClient[ClientInfo[addr].tclientNbr].WriteCharacteristicValue(Characteristic, TxBuf, wclGattProtectionLevel.plNone);// Protection());
                if (Res != wclErrors.WCL_E_SUCCESS)
                {
                    //MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
            }
            catch (Exception ex) { return false; }

            return true;
        }

        private bool ParseMessage(out uint response, uint nbrRxBytes, params byte[] rxBuffer)
        {
            bool status = false;
            UInt16 rxCrc = 0;
            byte[] tempBuf = new byte[2000];

            cErrorCheck errCheck = new cErrorCheck();
            int calculatedCRC = 0;
            int tempValue = 0;

            if(rxBuffer.Length <=6 )
            {
                // not a proper serial message
                response = 0;
                return false;
            }

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
                return false;
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
            System.Array.Copy(rxBuffer, ((int)PACKET.SIZEOF_HEADER + 2), tempBuf, 0, PayloadSize-8);

            status = true;

            response &= ~(uint)PACKET.MSG_ACK;

            // process response (could all be done in WinForm, where GUI stuff is done)
            switch (response)
            {
                case (int)PACKET.CMD_GET_PRESS_TEMP:
                    for (int j = 0; j < PSensorData.Length; j++)
                    {
                        int sizeofData = System.Runtime.InteropServices.Marshal.SizeOf(PSensorData[0]);

                        PSensorData[j] = (P_SENSOR_DATA)StructArray.ByteArrayToStruct(typeof(P_SENSOR_DATA), tempBuf);

                        System.Array.Copy(tempBuf, sizeofData, tempBuf, 0, (PayloadSize - sizeofData));
                    }
                    break;
                case (int)PACKET.CMD_GET_PRESS:
		            tempValue = (UInt16)System.BitConverter.ToUInt16(tempBuf, 0);
                    BoardId = (tempValue & 0x0003);
                    CompressorState = (tempValue & 0x0018);
                    ReliefState = (tempValue & 0x0060);
                                        
                    for (int j = 0; j < 8; j++)
                    {
                        //PressSensorPsi[j] =(UInt16)(tempBuf[3] << 8 | tempBuf[2]);
			            PressSensorPsi[j] = (UInt16)System.BitConverter.ToUInt16(tempBuf, 2);
                        System.Array.Copy(tempBuf, 2, tempBuf, 0, (PayloadSize - 2));
                    }
                    break;
                case (int)PACKET.CMD_GET_VERSION:
                    System.Array.Copy(tempBuf, 0, Payload, 0, (PayloadSize - 8));
                    break;
                case (int)PACKET.CMD_GET_BRD_ID:
                    BoardId = Convert.ToByte(tempBuf[0]);
                    break;
                case (int)PACKET.CMD_ACK:
                    break;
                case (int)PACKET_XLOAD.CMD_MEASURE:
                    LevelSensorData = (level_sensor_data_t)StructArray.ByteArrayToStruct(typeof(level_sensor_data_t), tempBuf);
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
                    txBuf[nbrBytes++] = (byte)(MotorCntrl.target & 0x1f);
                    txBuf[nbrBytes++] = (byte)((MotorCntrl.target >> 5)&0x7f);
                    break;
                case (int)MC_PACKET.CMD_STOP_MOTOR:
                    break;
                case (int)MC_PACKET.CMD_BRAKE:
                    txBuf[nbrBytes++] = 0x20;
                    break;
                case (int)MC_PACKET.CMD_GET_VAR:
                    txBuf[nbrBytes++] = MotorCntrl.varId;
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


        private void button11_Click_2(object sender, EventArgs e)
        {
            BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP, 0);
        }

        private void SerialMonitorTimer_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            SerialMonitorTimer.Enabled = false;          
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
                BuildSerialMessage((int)PACKET.CMD_GET_VERSION, 0);
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

            if (!panelsForm.GetCommStatus())
            {
                FtcStatusStrip.Items[0].Text = "Motor Controller Comm Disconnected";
                FtcStatusStrip.BackColor = Color.Orange;
            }
            else
            {
                FtcStatusStrip.BackColor = Color.SkyBlue;
            }

        }

        private void button15_Click(object sender, EventArgs e)
        {
            MotorCntrlTimer.Enabled = true;
        }



        private void TheMainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            serialFd.ClosePort(0);

            if (serialThread != null)
            {
                serialThread.Abort();
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

            BuildSerialMessage((int)PACKET.CMD_CLOSE_VALVE, 0);
        }

        private void button19_Click(object sender, EventArgs e)
        {
            ValveNbr = 0x01;

            BuildSerialMessage((int)PACKET.CMD_OPEN_VALVE, 0);            
        }


        private void SaveProfileBtn_Click(object sender, EventArgs e)
        {
            if (SnTextBox.Text == "" ||
                ModelTextBox.Text == "" ||
                SizeTextBox.Text == "" ||
                NameTextBox.Text == "" ||
                DateTextBox.Text == ""
                )
            {
                MessageBox.Show("Please fill in all Setup fields", "Profile", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            saveFileDialog1.Filter = "Profile Files (*.txt)|*.txt";
            saveFileDialog1.FileName = SnTextBox.Text + "_"+FitTypeComboBox.SelectedIndex;// + "_" + DateTextBox.Text +".txt"; // "AMCProf.txt";

            saveFileDialog1.Title = "AMC Profile File";

            saveFileDialog1.InitialDirectory = ProfilePath + SnTextBox.Text;

            if(!System.IO.Directory.Exists(saveFileDialog1.InitialDirectory))
                System.IO.Directory.CreateDirectory(saveFileDialog1.InitialDirectory);
            /*
            if (saveFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                user has canceled save
                return;
            }
            */

            //int row = 0;
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

            for (i = 0; i < 14; i++)
            {
                if (ProValues[i] == 0)
                {
                    MessageBox.Show("Value cannot be 0, please enter a value for all fields", "Profile", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
            }
            FileStream fs;
            byte[] dataArray = new byte[100];
            string[] profileValue = new string[10];
            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();

            cFileHandle fhandle = new cFileHandle();

            DialogResult result1 =0;

            if (System.IO.File.Exists(saveFileDialog1.InitialDirectory + "\\" + saveFileDialog1.FileName + ".txt"))
            {
                result1 =MessageBox.Show(FitTypeComboBox.Text + " fit profile for " +SnTextBox.Text + " already exists, do you wish to overwrite",
                               "Profile",
                                MessageBoxButtons.YesNo
                               ) ;
            }

            if (result1 == DialogResult.No)
                return;

            if (!fhandle.Open(saveFileDialog1.InitialDirectory + "\\" + saveFileDialog1.FileName + ".txt", out fs, 0))
            {
                MessageBox.Show("file open error for " + SnTextBox.Text,
                                               "Profile",
                                                MessageBoxButtons.OK, MessageBoxIcon.Error);                                 
                return;
            }

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

   
            for (int j = 0; j <7; j++)// (downloadGridView.RowCount - 1); j++)
            { 
                i = 0;

                try
                {
                    profileValue[i] = ProValues[j].ToString()+",";// ProfileGridView[i, j].Value.ToString() + ",";
                    dataArray = encoding.GetBytes(profileValue[i]);

                    fhandle.Write(fs, dataArray.Length, dataArray);

                    profileValue[i] = ProValues[j+7].ToString() + ",\n"; //ProfileGridView[i+1, j].Value.ToString() + ",\n" ;
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

            fhandle.Close(fs);

            MessageBox.Show(FitTypeComboBox.Text + " fit profile created",
                            "Profile",
                            MessageBoxButtons.OK
                          );
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
                if (SnTextBox.Text == "")
                {
                    MessageBox.Show("Please enter a serial number", "Profile", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }

                if (!System.IO.File.Exists(ProfilePath + SnTextBox.Text+"\\" + SnTextBox.Text + "_" + FitTypeComboBox.SelectedIndex.ToString() + ".txt"))
                {
                    MessageBox.Show(FitTypeComboBox.Text + " fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                else
                    openFileDialog1.FileName = ProfilePath + SnTextBox.Text + "\\" + SnTextBox.Text + "_" + FitTypeComboBox.SelectedIndex.ToString() + ".txt";
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

                            if (rowCnt == (rowProfileStart - 6))      NameTextBox.Text = textLine;                            
                            else if (rowCnt == (rowProfileStart - 5)) ModelTextBox.Text = textLine;                            
                            else if (rowCnt == (rowProfileStart - 4)) SnTextBox.Text = textLine;                            
                            else if (rowCnt == (rowProfileStart - 3)) DateTextBox.Text = textLine;                            
                            else if (rowCnt == (rowProfileStart - 2)) SizeTextBox.Text = textLine;
                            else if (rowCnt == (rowProfileStart - 1)) FitTypeComboBox.SelectedIndex = Convert.ToInt16(textLine);

                            ModelTextBox2.Text = ModelTextBox.Text;
                            SizeTextBox2.Text = SizeTextBox.Text;
                            NameTextBox2.Text = NameTextBox.Text;

                            if (rowCnt >= rowProfileStart)
                            {
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

                return;

            }

            MessageBox.Show(FitTypeComboBox.Text +  " fit profile for " + SnTextBox.Text + " loaded", "Profile", MessageBoxButtons.OK);
        }


        private void StartFitBtn_Click(object sender, EventArgs e)
        {
            int res = 0;

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
                ControllerGridView[2, row].Value = ProValues[row];
            }
           
            res =FindClientBrdId((int)BOARD_TYPE.COMPR_CNTRL);
            if (res == -1)
            {
                DialogResult result1 = MessageBox.Show("Compressor controller not connected, do you wish to proceed",
                                "Warning",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Warning
                               );

                if (result1 == DialogResult.No)
                    return;
            }
            else
            {
                ComprNbr =0x02;
                BuildSerialMessage((int)PACKET.CMD_START_COMPR, res);
            }

            res = FindClientBrdId((int)BOARD_TYPE.HI_FLOW_CNTRL);
            if (res == -1)
            {
                MessageBox.Show("Hi Flow Solenoid controller not connected, cannot proceed",
                                "Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
                return;
            }
            if (!BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP, res))
            {
                timer1.Enabled = false;
                return;
            }

            if (!BuildSerialMessage((int)PACKET.CMD_OPEN_VALVE, res))
            {
                timer1.Enabled = false;
                return;
            }

            ControllerGridView[2, 7].Value = "0";

            HiFlowBoardFitStart = true;
       //     timer1.Enabled = true;

            Refresh();
        }

        private void CaptureBtn_Click(object sender, EventArgs e)
        {
            if( MotorCntrl.speed !=0 )
            {
                MessageBox.Show("Please wait for mannequin to be positioned", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (StartCapture)
            {
                MessageBox.Show("Capture already in progress", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            for (int j = 0; j < NbrCameras; j++)
            {
                try
                {
                    CameraInfo[j].proc.Kill();
                }
                catch (Exception ex)
                {
                    //MessageBox.Show(ex.ToString(), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    MessageBox.Show("There is no process running for camera " + (j+1) + "\ndid you forget to perform a preview check", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                    //                return;
                }
            }

            StartCapture = true;

            BleMsgTextBox.Clear();

        //    FileSystemWatcher.Path = FileCapturePath;
       //     FileSystemWatcher.Filter = "*.png";
        //    FileSystemWatcher.NotifyFilter = NotifyFilters.Size;

        }

        private void StopCaptureBtn_Click(object sender, EventArgs e)
        {
            StartCapture = false;

            if (MotorCntrl.detected)
            {
                MotorStopBtn_Click(null, null);

                MotorStatusLbl.Text = "Stop";

          //      MotorThread.Abort();
            }

            for (int j = 0; j < NbrCameras; j++)
            {
                try
                {
                    CameraInfo[j].proc.Kill();

                    CameraInfo[j].captureStarted = false;

                    PhotoCaptureStarted = false;

                    CaptureBtn.Enabled = false;
                }
                catch (Exception ex)
                {
                    //MessageBox.Show(ex.ToString(), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    MessageBox.Show("There is no process running for camera " + (j+1), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                    //return;
                }
            }

            FileMonitorTimer.Enabled = false;
        }


        private void StartFitBtn2_Click(object sender, EventArgs e)
        {
            int res = 0;
              
            for (int row = 7; row < 14; row++)
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

                Controller2GridView[2, row - 7].Value = ProValues[row];
            }

            res = FindClientBrdId((int)BOARD_TYPE.COMPR_CNTRL);
            if (res == -1)
            {
                DialogResult result1 = MessageBox.Show("Compressor controller not connected, do you wish to proceed",
                                "Warning",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Warning
                               );

                if (result1 == DialogResult.No)
                    return;
            }
            else
            {
                ComprNbr = 0x01;
                BuildSerialMessage((int)PACKET.CMD_START_COMPR, res);
            }

            res =FindClientBrdId((int)BOARD_TYPE.LO_FLOW_CNTRL);
            if (res == -1)
            {
                MessageBox.Show("Lo Flow Solenoid controller not connected, cannot proceed",
                                "Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
                return;
            }
            if (!BuildSerialMessage((int)PACKET.CMD_GET_PRESS_TEMP, res))
            {
                timer1.Enabled = false;
                return;
            }

            if (!BuildSerialMessage((int)PACKET.CMD_OPEN_VALVE, res))
            {
                timer1.Enabled = false;
                return;
            }

            Controller2GridView[2, 7].Value = "0";

            //timer1.Enabled = true;
            LoFlowBoardFitStart = true;

            Refresh();
        }

        private void CloseProfileBtn_Click(object sender, EventArgs e)
        {
            ProfileGroupBox.Visible = false;
        }


        private void FileSystemWatcher_Changed(object sender, FileSystemEventArgs e)
        {
            FolderSize = 0;
            FileCnt = 0;

            for (int j = 0; j < NbrCameras; j++)
            {
                //CameraFileSize[j] = 0;
                CameraInfo[j].fileSize = 0;
            }

            foreach (string file in Directory.GetFiles("C:\\WinAMC\\capture"))
            {
                if (File.Exists(file))
                {
                    FileInfo finfo = new FileInfo(file);
                    FolderSize += finfo.Length;

                    if (finfo.FullName.Contains("c1_"))
                    {
                        //CameraFileSize[0] += finfo.Length;
                        CameraInfo[0].fileSize += finfo.Length;
                    }
                    if (finfo.FullName.Contains("c2_"))
                    {
                        //CameraFileSize[1] += finfo.Length;
                        CameraInfo[1].fileSize += finfo.Length;
                    }
                }


                FileCnt++;
            }            


            if (e.ChangeType == WatcherChangeTypes.Changed)
            {
                var info = new FileInfo(e.FullPath);
                var theSize = info.Length;

                CapturedFileSize = FolderSize / 1000000;// theSize /1000;
                CameraOperationLbl.Text = "Camera capture started " + CapturedFileSize.ToString("0.00") + "MB\nFile Count: " + FileCnt;
/*
                for (int j = 0; j < NbrCameras; j++)
                {
                    CameraDataGridView[1, j].Value = (CameraFileSize[j] / 1000000).ToString("0.00");
                }
                */
            }

        }

        private void StartPreviewBtn_Click(object sender, EventArgs e)
        {
            CaptureBtn.Enabled = true;

            if (CheckError()) return;

                //if (CameraInfo[NbrCameras].portName == "")
            if (NbrCameras ==0 )
            {
                MessageBox.Show("No Camera detected", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            for (int camera = 0; camera <NbrCameras; camera++)
            {
                if (CameraInfo[camera].state == 0)
                {                
                    cameraThread[camera] = new Thread(cameraDataThread);
                    cameraThread[camera].Start(camera);
                    
                    CameraInfo[camera].state = 1;

                    BleMsgTextBox.Clear();

                    StartCapture = false;

                    CameraOperationLbl.Visible = true;

                    //CameraTimer.Enabled = true;
                }

                //CameraDataGridView[1, camera].Value = "0.00";
            }

            if (MotorCntrl.detected)
            {
                MotorThread = new Thread(MotorDataThread);
                MotorThread.Start();
            }

            FileMonitorTimer.Enabled = true;
        }

        private void UploadFilesBtn_Click(object sender, EventArgs e)
        {
            var client = new WebClient();
            client.Credentials = new NetworkCredential("jclugston", "Trig2017");

            string zipFileName = "C:\\WinAMC\\capture\\" + SnLoadComboBox.Text + ".zip";
            //string ftpFileName = "ftp://ftp.drivehq.com//TestDir/" + SnLoadComboBox.Text + ".zip";
            string ftpFileName = FtpServerTextBox.Text + SnLoadComboBox.Text + ".zip";

            try
            {
                File.Delete(zipFileName);
            }
            catch (Exception ex) { }

            ZipFile zip = new ZipFile();

            foreach (string file in Directory.GetFiles("C:\\WinAMC\\capture"))
            {
                if (File.Exists(file))
                {
                    FileInfo finfo = new FileInfo(file);

                    string result = Path.GetFileName(finfo.FullName);


                    //using (ZipFile zip = new ZipFile())
                    {
                        // add this map file into the "images" directory in the zip archive
                        zip.AddFile(finfo.FullName, "images");                        
                    }
                    /*
                    try
                    {
                        client.UploadFile("ftp://ftp.drivehq.com//TestDir/"+ result, WebRequestMethods.Ftp.UploadFile, finfo.FullName);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("File upload failed", "FTP", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    */
                }
            }

            try
            {
                zip.Save(zipFileName);
            }
            catch (Exception ex) { }

            try
            {
                FtpUploadTimer.Enabled = true;

                //client.UploadFile(ftpFileName, WebRequestMethods.Ftp.UploadFile, zipFileName);
                client.UploadFileTaskAsync(ftpFileName, WebRequestMethods.Ftp.UploadFile, zipFileName);
            }
            catch (Exception ex)
            {
                MessageBox.Show("File upload failed", "FTP", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

     //       MessageBox.Show("File uploaded", "FTP", MessageBoxButtons.OK, MessageBoxIcon.None);

        }

        private void BleConnectBtn_Click(object sender, EventArgs e)
        {
            int j = 0;

            BleMsgTextBox.Clear();

            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];
                
                for (j = 0; j < ClientInfo.Count; j++)
                {
                    if (ClientInfo[j].addr == Item.SubItems[0].Text)
                    {
                        break;
                    }
                }

                for (j = 0; j < 4; j++)
                { 
                    if (TClient[j].State == 0)
                    {
                        TClient[j].Address = Convert.ToInt64(Item.Text, 16);
                        //Client.ConnectOnRead = cbConnectOnRead.Checked;
                        Int32 Res = TClient[j].Connect((wclBluetoothRadio)Item.Tag);
                        if (Res != wclErrors.WCL_E_SUCCESS)
                            MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                        break;
                    }
                }
            }
        }

        private void BleDisconnectBtn_Click(object sender, EventArgs e)
        {
            int j = 0; 

            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];

                for (j = 0; j < ClientInfo.Count; j++)
                {
                    if (ClientInfo[j].addr == Item.SubItems[0].Text)
                    //if (TClient[j].Address.ToString() == Item.SubItems[0].Text)
                    {
                        break;
                    }
                }

                try
                {
                    Int32 Res = TClient[ClientInfo[j].tclientNbr].Disconnect();
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Device Error", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }


        private void button1_Click(object sender, EventArgs e)
        {
            wclBluetoothRadio Radio = GetRadio();

            BleMsgTextBox.Clear();

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

            NbrProjectDevices = 0;
        }

        int FindClientInfoAddr(String addrStr)
        {
            int j = 0;

            for (j = 0; j < ClientInfo.Count; j++)
            {
                if (ClientInfo[j].addr == addrStr)
                {
                    return j;
                }
            }

            return -1;
        }

        int FindClientBrdId(int brdId)
        {
            int j = 0;

            for (j = 0; j < ClientInfo.Count; j++)
            {
                if (ClientInfo[j].brdId == brdId)
                {
                    return j;
                }
            }

            return -1;
        }

        int FindTClientInfoAddr(long addr)
        {
            int j = 0;
            //String tclientAddr = "";

            for (j = 0; j < 4; j++)
            {
                //tclientAddr = TClient[j].Address.ToString();
                if (TClient[j].Address == addr)
                {
                    return j;
                }
            }

            return -1;
        }
        private void TraceEvent(Int64 Address, String Event, String Param, String Value)
        {
            String s = "";
            if (Address != 0)
                s = Address.ToString("X12");
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

                    if (DevName.Contains(DeviceName)) //our ble device
                    {
                        ProjectDevices[NbrProjectDevices] =i;
                        ProjectDeviceConnect = false;
                        DeviceConnectState = 0;
                        NbrProjectDevices++;
                    }
                }
            }


            BleMsgTextBox.AppendText("Discovering completed\r\n");
            BleMsgTextBox.ScrollToCaret();

            MessageBox.Show(NbrProjectDevices + " Swipe Fashion Controllers Discovered", "SF", MessageBoxButtons.OK);

            //AutoConnectTimer.Enabled = true;
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
            BleMsgTextBox.AppendText("Disconnect\r\n");
            BleMsgTextBox.ScrollToCaret();

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

            int res = FindClientInfoAddr(addrStr);

            if (ClientInfo[res].brdId == (int)BOARD_TYPE.HI_FLOW_CNTRL)
            {
                BrdTypeLbl.Text = "Board Type: Hi Flow   DISCONNECTED";
                HiFlowInd.BackColor = Color.LightGray;
            }
            else if (ClientInfo[res].brdId == (int)BOARD_TYPE.LO_FLOW_CNTRL)
            {
                Brd2TypeLbl.Text = "Board Type: Lo Flow   DISCONNECTED";
                LoFlowInd.BackColor = Color.LightGray;
            }
            else if (ClientInfo[res].brdId == (int)BOARD_TYPE.COMPR_CNTRL)
            {
                Brd3TypeLbl.Text = "Board Type: Compr   DISCONNECTED";
                ComprInd.BackColor = Color.LightGray;
            }

            try
            {
                ClientInfo.RemoveAt(res);
            }
            catch (Exception ex) { }

        }

        void Client_OnConnect(object Sender, int Error)
        {
            BleMsgTextBox.AppendText("Connect\r\n");
            BleMsgTextBox.ScrollToCaret();

            int tclientIndex = FindTClientInfoAddr(((wclGattClient)Sender).Address);
            // Connection property is valid here.
            TraceEvent(((wclGattClient)Sender).Address, "Connected", "Error", "0x" + Error.ToString("X8"));

            if (TClient[tclientIndex].State == 0)
            {
                BleMsgTextBox.AppendText("Disconnect\r\n");
                BleMsgTextBox.ScrollToCaret();

                return;
            }
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

            CLIENT_INFO server = new CLIENT_INFO();
            server.addr = TClient[tclientIndex].Address.ToString("X12");
            server.brdId = -1;
            server.tclientNbr = tclientIndex;

            if (!BleGetServices(tclientIndex))
            {
                ClientInfo.Add(server);

                MessageBox.Show("No Service Found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                ProjectDeviceConnect = true;

                return;
            }


            server.character = BleGetCharacteristics(tclientIndex, "1e4d"); //"6e400003");

            if (server.character == -1)
            {
                ClientInfo.Add(server);
                return;
            }

            server.txCharacteristic = FCharacteristics[server.character];

            server.character = BleGetCharacteristics(tclientIndex, "1e4d");//"6e400002");
            server.rxCharacteristic = FCharacteristics[server.character];

            ClientInfo.Add(server);

            ProjectDeviceConnect = true;

            BleSubscribeCharacteristics(ClientInfo[ClientInfo.Count-1]);// ClientInfo[tclientIndex]);// characterNbr, server.tclientNbr);

            //      Thread.Sleep(5000);

            BleWriteCcd(ClientInfo[ClientInfo.Count - 1]);

            //BuildSerialMessage((int)PACKET.CMD_GET_BRD_ID, ClientInfo[ClientInfo.Count - 1].tclientNbr);

            //BuildSerialMessage((int)PACKET.CMD_GET_VERSION, ClientInfo[ClientInfo.Count - 1].tclientNbr);

            BuildSerialMessage((int)PACKET.CMD_GET_BRD_ID, ClientInfo.Count - 1);

            BuildSerialMessage((int)PACKET.CMD_GET_VERSION, ClientInfo.Count - 1);
        }

        void Client_OnCharacteristicChanged(object Sender, ushort Handle, byte[] Value)
        {
            uint response = 0;
            CLIENT_INFO temp;

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

                float delatP = 0.0F;
                
                if (ParseMessage(out response, (uint)Value.Length, Value))                
                {

                    int boardClient= FindClientInfoAddr(((wclGattClient)Sender).Address.ToString("X12"));

                    SerialMonitorTimer.Enabled = false;

                    switch (response)
                    {
                        case (int)PACKET.CMD_GET_PRESS_TEMP:
                            ValveNbr = 0;
                            
                            for (int j = 0; j < 8; j++)
                            {
                                delatP = (float)(PSensorData[j].press - PSensorData[7].press) / (float)6.89476;

                                DataGridView gridView = ControllerGridView; ;
                                if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.HI_FLOW_CNTRL)
                                {
                                    gridView = ControllerGridView;
                                }
                                else if(ClientInfo[boardClient].brdId == (int)BOARD_TYPE.LO_FLOW_CNTRL)
                                {
                                    gridView = Controller2GridView;
                                }

                                if (PSensorData[j].press != -1)
                                {
                                    gridView[0, j].Value = (PSensorData[j].press / 6.89476).ToString("0.000");
                                    gridView[1, j].Value = (PSensorData[j].temp).ToString("0.0");

                                    gridView[3, j].Value = delatP.ToString("0.000");

                                    try
                                    {
                                        if (delatP >= Convert.ToSingle(gridView[2, j].Value))
                                            ValveNbr |= (byte)(0x01 << j);
                                    }
                                    catch (Exception ex)
                                    {
                   //                     MessageBox.Show(ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    }
                                }
                                else
                                {
                                    ValveNbr |= (byte)(0x01 << j);

                                    gridView[0, j].Value = "FAULT";
                                    gridView[1, j].Value = "FAULT";

                                    gridView[3, j].Value = "FAULT";

                                    gridView.Rows[j].Cells[0].Style.BackColor = Color.Orange;
                                    gridView.Rows[j].Cells[1].Style.BackColor = Color.Orange;
                                    gridView.Rows[j].Cells[2].Style.BackColor = Color.Orange;
                                    gridView.Rows[j].Cells[3].Style.BackColor = Color.Orange;
                                }
                            }

                            if (ValveNbr != 0)
                                BuildSerialMessage((int)PACKET.CMD_CLOSE_VALVE, boardClient);
                            break;
                        case (int)PACKET.CMD_GET_PRESS:
                            ValveNbr = 0;
                            ComprNbr = 0;

                            temp = ClientInfo[boardClient];
                            temp.brdId = BoardId;
                            ClientInfo[boardClient] = temp;

                            if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.HI_FLOW_CNTRL)
                            {
                                ValveNbr = DisplayGridData(ControllerGridView, HiFlowBoardFitStart);

                                if (ValveNbr == 0)
                                    HiFlowBoardFitStart = false;
                            }
                            else if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.LO_FLOW_CNTRL)
                            {
                                ValveNbr = DisplayGridData(Controller2GridView, LoFlowBoardFitStart);

                                if (ValveNbr == 0)
                                    LoFlowBoardFitStart = false;
                            }
                            else if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.COMPR_CNTRL)
                                ComprNbr = DisplayComprGridData(CompressorCntrlGridView);


                            if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.COMPR_CNTRL)
                            {
                                /* send compressor control board its message */
                                BuildSerialMessage((int)PACKET.CMD_SET_COMPR , boardClient);
                            }
                            else
                            {
                                /* send solenoid controller board its message */
                                BuildSerialMessage((int)PACKET.CMD_SET_VALVE, boardClient);
                            }
                            break;
                        case (int)PACKET.CMD_GET_VERSION:
                            if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.HI_FLOW_CNTRL)
                                FwBrd1VersionLbl.Text = "AMC: v" + System.Text.Encoding.ASCII.GetString(Payload);
                            else if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.LO_FLOW_CNTRL)
                                FwBrd2VersionLbl.Text = "AMC: v" + System.Text.Encoding.ASCII.GetString(Payload);
                            else if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.COMPR_CNTRL)
                                FwBrd3VersionLbl.Text = "AMC: v" + System.Text.Encoding.ASCII.GetString(Payload);
                            break;
                        case (int)PACKET.CMD_GET_BRD_ID:
                            temp = ClientInfo[boardClient];
                            temp.brdId = BoardId;
                            ClientInfo[boardClient] = temp;

                            if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.HI_FLOW_CNTRL)
                            {
                                BrdTypeLbl.Text = "Board Type: Hi Flow   Addr: " + ((wclGattClient)Sender).Address.ToString("X");
                                HiFlowInd.BackColor =Color.Green;
                            }
                            else if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.LO_FLOW_CNTRL)
                            {
                                Brd2TypeLbl.Text = "Board Type: Lo Flow   Addr: " + ((wclGattClient)Sender).Address.ToString("X");
                                LoFlowInd.BackColor = Color.Green;
                            }
                            else if (ClientInfo[boardClient].brdId == (int)BOARD_TYPE.COMPR_CNTRL)
                            {
                                Brd3TypeLbl.Text = "Board Type: Compr   Addr: " + ((wclGattClient)Sender).Address.ToString("X");
                                ComprInd.BackColor = Color.Green;
                            }
                            break;
                        case (int)PACKET_XLOAD.CMD_ACK:
                        case (int)PACKET_XLOAD.CMD_ERASE_FLASH:
                        case (int)PACKET_XLOAD.CMD_FW_UPDATE_COMPLETE:
                        case (int)PACKET_XLOAD.CMD_FW_UPDATE_PACKET:
                        case (int)PACKET_XLOAD.CMD_RESET:
                            AckReceived = true;
                            break;
                        case (int)PACKET_XLOAD.CMD_MEASURE:
                            //  AckReceived = true;
                            break;							
                    }
                }
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

        private bool BleGetServices(int tclientIndex)
        {
            FServices = null;

            BleMsgTextBox.AppendText("Get Services\r\n");
            BleMsgTextBox.ScrollToCaret();

            //Int32 Res = Client.ReadServices(wclGattOperationFlag.goNone, out FServices);
            Int32 Res = TClient[tclientIndex].ReadServices(wclGattOperationFlag.goNone, out FServices);

            if (Res != wclErrors.WCL_E_SUCCESS)
            {
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            if (FServices == null)
                return false;

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

            return true;
        }

        private int BleGetCharacteristics(int tclientIndex, string characteristic)
        {
            int servNbr = 0;
            int characterNbr = 0;
            bool serviceFound = false;
            String s = "";

            FCharacteristics = null;

            for (servNbr = 0; servNbr < FServices.Length; servNbr++)
            {
                if (!FServices[servNbr].Uuid.IsShortUuid)
                {
                    s = FServices[servNbr].Uuid.LongUuid.ToString();

                    /* 49535343-FE7D-4AE5-8FA9-9FAFD205E455.
                     * The Transparent UART Service contains the following data characteristics:
                     â€¢ Transparent UART Transmit(TX) Characteristic
                     â€¢ Transparent UART Receive(RX) Characteristic
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
                return -1;
            }

            BleMsgTextBox.AppendText("Get Characteristics\r\n");
            BleMsgTextBox.ScrollToCaret();

            wclGattService Service = FServices[servNbr];// lvServices.SelectedItems[0].Index];            

            /* read the characteristics for this service of interest */
            Int32 Res = TClient[tclientIndex].ReadCharacteristics(Service, wclGattOperationFlag.goNone, out FCharacteristics);
            if (Res != wclErrors.WCL_E_SUCCESS)
            {
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return -1;
            }

            if (FCharacteristics == null)
                return -1;

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
                    //if (s.Contains("1e4d"))
                    {
                        /* Transparent UART TX 49535343-1E4D-4BD9-BA61-23C647249616,
                         * find the 1E4D
                        */
                      //  break;
                    }

                    /* RX Characteristic UUID: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
                     * TX Characteristic UUID: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
                     * Nordic transparent UART (nRF52840)
                    */
                    if (s.Contains(characteristic))//"6e400003"))
                    {
                        /* Transparent UART RX 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
                         * Transparent UART TX 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
                         * find the 6E400003
                        */
                        return characterNbr;
                    }

                    characterNbr++;
                }
            }


            return -1;
        }

        private void BleSubscribeCharacteristics(CLIENT_INFO clientInfo)//int characterNbr,int tclientIndex)
        {

            BleMsgTextBox.AppendText("Subscribe\r\n");
            BleMsgTextBox.ScrollToCaret();

            wclGattCharacteristic Characteristic = clientInfo.txCharacteristic;// FCharacteristics[clientInfo.character];

            // In case if characteristic has both Indication and Notification properties
            // set to True we have to select one of them. Here we use Notifications but
            // you can use other one.
            if (Characteristic.IsNotifiable && Characteristic.IsIndicatable)
                // Change the code line below to
                // Characteristic.IsNotifiable = false;
                // if you want to receive Indications instead of notifications.
                Characteristic.IsIndicatable = false;
            //Int32 Res = Client.Subscribe(Characteristic);
            Int32 Res = TClient[clientInfo.tclientNbr].Subscribe(Characteristic);
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private int BleWriteCcd(CLIENT_INFO clientInfo)
        {
            BleMsgTextBox.AppendText("Write CCCD Subscribe\r\n");
            BleMsgTextBox.ScrollToCaret();

            wclGattCharacteristic Characteristic = clientInfo.txCharacteristic;// FCharacteristics[clientInfo.character];

            // In case if characteristic has both Indication and Notification properties
            // set to True we have to select one of them. Here we use Notifications but
            // you can use other one.
            if (Characteristic.IsNotifiable && Characteristic.IsIndicatable)
                // Change the code line below to
                // Characteristic.IsNotifiable = false;
                // if you want to receive Indications instead of notifications.
                Characteristic.IsIndicatable = false;
            Int32 Res = TClient[clientInfo.tclientNbr].WriteClientConfiguration(Characteristic, true, wclGattOperationFlag.goNone, wclGattProtectionLevel.plNone);
            if (Res != wclErrors.WCL_E_SUCCESS)
                MessageBox.Show("Error: 0x" + Res.ToString("X8"), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

            return Res;
        }

        private void BleSendData(int clientIndex)
        {
            try
            {
                //int writeChar = ClientInfo[clientIndex].character;
                //wclGattCharacteristic Characteristic = ClientInfo[clientIndex].FCharacteristics[writeChar]; // FCharacteristics[ClientInfo[clientIndex].character];// lvCharacteristics.SelectedItems[0].Index];
                wclGattCharacteristic Characteristic = ClientInfo[clientIndex].rxCharacteristic; // FCharacteristics[ClientInfo[clientIndex].character];// lvCharacteristics.SelectedItems[0].Index];

                String Str = edCharVal.Text;

                System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
                byte[] dataArray = new byte[32];
                dataArray = encoding.GetBytes(Str);

                if (Str.Length % 2 != 0)
                    Str = "0" + Str;

                Int32 Res = TClient[ClientInfo[clientIndex].tclientNbr].WriteCharacteristicValue(Characteristic, dataArray, wclGattProtectionLevel.plNone);// Protection());
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
            if (lvDevices.SelectedItems.Count == 0)
                MessageBox.Show("Select device", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            else
            {
                ListViewItem Item = lvDevices.SelectedItems[0];

                int res = FindClientInfoAddr(Item.SubItems[0].Text);

                BleSendData(res);
            }
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

        /*
        **************************************
        * END OPEN CV STUFF
        **************************************
        */

        private bool CheckError()
        {
            if (!MotorCntrl.detected)
            {
                MessageBox.Show("No motor detected, please check comm port", "Motor", MessageBoxButtons.OK, MessageBoxIcon.Error);
                try
                {
                    MotorThread.Abort();
                }
                catch (Exception ex) { }
                return true;
            }
            else
                return false;
        }

        private void MotorFwdBtn_Click(object sender, EventArgs e)
        {
            if (CheckError()) return;

            BuildMCSerialMessage((int)MC_PACKET.CMD_EXIT_SAFE_START);
            MotorCntrl.target = Convert.ToInt16(MotorSpeedUpDown.Value);
            BuildMCSerialMessage((int)MC_PACKET.CMD_SET_TRGT_SPEED_FWD);
        }

        private void MotorRevBtn_Click(object sender, EventArgs e)
        {
            if (CheckError()) return;

            BuildMCSerialMessage((int)MC_PACKET.CMD_EXIT_SAFE_START);
            MotorCntrl.target = Convert.ToInt16(MotorSpeedUpDown.Value);
            BuildMCSerialMessage((int)MC_PACKET.CMD_SET_TRGT_SPEED_REV);
        }

        private void MotorStopBtn_Click(object sender, EventArgs e)
        {
            if (CheckError()) return;

            BuildMCSerialMessage((int)MC_PACKET.CMD_STOP_MOTOR);
        }

        private void MotorReadBtn_Click(object sender, EventArgs e)
        {
            if (CheckError()) return;

            MotorCntrl.varId = (int)MC_VARS.GET_VOLTAGE;// GET_REV_LIMIT;// (int)MC_VARS.GET_CURRENT;
            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
        }

        private void CloseControlPanelBtn_Click(object sender, EventArgs e)
        {
            ControlPanel.Visible = false;
            MotorCntrlTimer.Enabled = false;

            MotorCntrl.varId = 0;
        }

        private void MotorCntrlTimer_Tick(object sender, EventArgs e)
        {
            if (MotorCntrl.msgReceived)
            {
                MotorCntrl.detected = true;
                MotorCntrl.msgReceived = false;
            }
            else
            {
                MotorCntrl.varId = (int)MC_VARS.GET_VOLTAGE;
                BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);

                if (++MotorCntrl.msgErrCnt > 2)
                {
                    MotorCntrl.msgErrCnt = 0;
                    MotorCntrlTimer.Enabled = false;
                    MotorDetectLbl.Text = "Motor: ";
                    MotorCntrl.detected = false;

                    MessageBox.Show("Motor comms error", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void motorDiagnosticToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ControlPanel.Top = 0;
            ControlPanel.Left = 10;
            ControlPanel.Height = (int)MAIN_DIM.HEIGHT;
            ControlPanel.Width = 450;
            ControlPanel.Visible = true;

            MotorCntrlTimer.Enabled = true;
            MotorCntrl.varId = (int)MC_VARS.GET_VOLTAGE;
            BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR);
        }

        private void button1_Click_2(object sender, EventArgs e)
        {
            OperationPanel.Top = 0;
            OperationPanel.Left = 5;
            OperationPanel.Height = (int)MAIN_DIM.HEIGHT-60;
            OperationPanel.Width = 450;
            OperationPanel.Visible = true;

            MotorDetectLbl.Text = "Motor: ";
            CameraStatusLbl.Text = "Camera: ";

            //ControllerGroupBox.Top = 150;
            SettingsGroupBox.Top = SnLoadComboBox.Top + 25;
            SettingsGroupBox.Left =15;

            FitGroupBox.Top = 225;

            MotorGroupBox.Top = FitGroupBox.Top + 75;
            CameraGroupBox.Top = MotorGroupBox.Top + 85;

            UploadFilesBtn.Top = CameraGroupBox.Top + 220;
            UploadFilesBtn.Left = 15;

            OpCloseBtn.Top = UploadFilesBtn.Top;
            OpCloseBtn.Left = UploadFilesBtn.Left +355;

            FtpServerTextBox.Top = UploadFilesBtn.Top+2;
            FtpServerTextBox.Left = UploadFilesBtn.Left +85;

            StartPreviewBtn.Visible = true;

            if (BuildMCSerialMessage((int)MC_PACKET.CMD_GET_VAR))
            {
                /* remove for now, add when system has motor */
                MotorCntrlTimer.Enabled = true;
                MotorCntrl.varId = (int)MC_VARS.GET_VOLTAGE;
            }
                       
            CameraOperationLbl.Text = "Camera: ";
            MotorStatusLbl.Text = "Mannequin: ";

            var foldersFound = Directory.GetDirectories(ProfilePath);// "C:\\WinAMC\\Profile\\");
            SnLoadComboBox.Items.Clear();
            for (int j = 0; j < foldersFound.Length; j++)
            {
                string result = Path.GetFileName(foldersFound[j]);
                SnLoadComboBox.Items.Add(result);
            }

            try
            {
                SnLoadComboBox.SelectedIndex = 0;
            }
            catch (Exception ex)
            {
                MessageBox.Show("No profiles found", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }

            //if (CameraState[0] == 0)
            if ( CameraInfo[0].state ==0 )
            {
                cameraThread[0] = new Thread(cameraDataThread);

                cameraThread[0].Start(0);
                //CameraState[0] = 4;
                CameraInfo[0].state = 4;
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
            MotorCntrlTimer.Enabled = false;
            MotorCntrl.varId = 0;
        }

        //private void button1_Click_3(object sender, EventArgs e)
        private void OpLoadBtn_Click(object sender, EventArgs e)
        {
            int profCnt = 0;

            SnLoadTextBox.Text = SnLoadComboBox.Text;

            if (SnLoadTextBox.Text == "")
            {
                MessageBox.Show("Please enter a serial number", "Profile", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

        //    openFileDialog1.FileName = ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_0.txt";
         //   LoadProfileBtn_Click(null, null);

            OpLooseFitBtn.Enabled = false;
            OpNormalFitBtn.Enabled = false;
            OpTightFitBtn.Enabled = false;

            if (!System.IO.File.Exists(ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_0" + ".txt"))
            {
                MessageBox.Show("Loose fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                OpLooseFitBtn.Enabled = true;
                profCnt ++;
            }

            if (!System.IO.File.Exists(ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_1" + ".txt"))
            {
                MessageBox.Show("Normal fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                OpNormalFitBtn.Enabled = true;
                profCnt++;
            }

            if (!System.IO.File.Exists(ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_2" + ".txt"))
            {
                MessageBox.Show("Tight fit profile not found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                OpTightFitBtn.Enabled = true;
                profCnt++;
            }

            MessageBox.Show(profCnt.ToString() + " of 3 profiles for " + SnLoadTextBox.Text +" succesfully loaded", "Profile", MessageBoxButtons.OK);

        }

        private void FitTypeLoadComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            OpLoadBtn_Click(null, null);
        }

        private void FitTypeLoadComboBox_SelectionChangeCommitted(object sender, EventArgs e)
        {
            //button1_Click_3(null, null);
        }

        private void OpLooseFitBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_0.txt";

            LoadProfileBtn_Click(null, null);

            //MessageBox.Show("Profile " + SnLoadTextBox.Text +" " + FitTypeLoadComboBox.Text +" fit succesfully loaded", "Profile", MessageBoxButtons.OK);
        }

        private void OpNormalFitBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_1.txt";

            LoadProfileBtn_Click(null, null);
        }

        private void OpTightFitBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = ProfilePath + SnLoadTextBox.Text + "\\" + SnLoadTextBox.Text + "_2.txt";

            LoadProfileBtn_Click(null, null);
        }


        private void SnComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            SnTextBox.Text = SnComboBox.Text;
        }

        private void DefaultProfileBtn_Click(object sender, EventArgs e)
        {
            TorsoUpDown.Value = new decimal((double)1.1F);
            RightArmUpDown.Value = new decimal((double)2.2F);
            RightBreastUpDown.Value = new decimal((double)3.3F);
            BellyUpDown.Value = new decimal((double)4.4F); ;
            RightHipUpDown.Value = new decimal((double)5.5F); ;
            RightButtockUpDown.Value = new decimal((double)6.6);
            RightLegUpDown.Value = new decimal((double)7.7);

            //row = 0;            
            LeftArmUpDown.Value = new decimal((double)10.0);
            LeftBreastUpDown.Value = new decimal((double)11.1);
            LeftHipUpDown.Value = new decimal((double)12.2);
            LeftButtockUpDown.Value = new decimal((double)13.4);
            LeftLegUpDown.Value = new decimal((double)14.4);
            LeftCalfUpDown.Value = new decimal((double)15.5);
            RightCalfUpDown.Value = new decimal((double)16.6);
        }

        /* BOOTLOADER STUFF */
        private void button1_Click_3(object sender, EventArgs e)
        {
			FirmwareUpdate(3,0,false);
            /*
            FrameSource frameSource;
           // frameSource = Cv2.CreateFrameSource_Camera(0);

            VideoCapture capture;
            Mat frame;

           
            frame = new Mat();
            using (Window window = new Window("Webcam"))

            for (int j = 0; j < 100; j++)
            {               
                capture = new VideoCapture(j);
                capture.Read(frame);

                if (frame.Empty())
                  continue;

                window.ShowImage(frame);

                capture.Open(j);

                if (capture.IsOpened())
                    break;
            }
            */

            //            Cv2.NamedWindow("Video", WindowMode.AutoSize);

            /*
            //if (capture.IsOpened())
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
        private void FirmwareUpdate(int medium, UInt16 epAddr, bool withErase)
        {
            //openFileDialog1.Filter = "binary files (*.bin)|*.bin";
            openFileDialog1.Filter = "intel hex files (*.hex)|*.hex";
            openFileDialog1.FileName = "";

            if (medium == 0)
            {
                openFileDialog1.InitialDirectory = "C:\\WinXLOAD\\Firmware\\Gateway";
                openFileDialog1.FileName = "C:\\WinXLOAD\\Firmware\\Gateway\\gateway.hex";
            }
            else if (medium == 1)
            {
                openFileDialog1.InitialDirectory = "C:\\WinXLOAD\\Firmware\\HandHeldGateway";
                openFileDialog1.FileName = "C:\\WinXLOAD\\Firmware\\HandHeldGateway\\HH_gateway.hex";
            }
            else
            {
                openFileDialog1.InitialDirectory = "C:\\WinXload\\firmware\\Xload";
                openFileDialog1.FileName = "C:\\WinXload\\firmware\\xload\\Xload5.0_Ble_s112.hex";
            }

            openFileDialog1.Title = "Firmware file";

            if (!StartFwUpdate(openFileDialog1.FileName, medium, 0, 0, "0", withErase))
            {
                MessageBox.Show("Image file invalid or does not exist",
                                "Firmware Bootloader",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );

                if (openFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                {
                    //user has canceled open
                    return;
                }

                if (!StartFwUpdate(openFileDialog1.FileName, medium, 0, 0, "0", withErase))
                {
                    MessageBox.Show("Image file invalid or does not exist",
                                    "Firmware Bootloader",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );
                    return;
                }
            }
       
        }

        class FW_UPDATE
        {
            public int machState;
            public int prevmachState;
            public int nextmachState;			
            public float percentComplete;
            public bool eraseComplete;
            public bool uploadTimeout;
            public bool uploadSuccess;
            public UInt32 timeoutCnt;

            public float fwVersion;

            public int txStatus;

            public bool erase;
            public string binaryFilename;


        }

        static FileStream fStream = null;
        cFileHandle fhandle = new cFileHandle();
        static FW_UPDATE FwUpdate = new FW_UPDATE();

        static System.Windows.Forms.Timer FwUpdateTimer = new System.Windows.Forms.Timer();
        static System.Windows.Forms.Timer TimeoutTimer = new System.Windows.Forms.Timer();
        static System.Threading.Thread SerialFwUpdateThread;
        static System.Threading.Thread MlinkFwUpdateThread;
        static System.Threading.Thread BleFwUpdateThread;
        long fileLocation = 0;
        UInt16 value = 0;
        UInt16 fileChecksum = 0;
		
        private void BleFwUpdate()
        {            
            //cFwUpdate fwUpdate = new cFwUpdate();
            while (true)
            {
                switch (FwUpdate.machState)
                {
                    case 0:
                        BuildSerialMessage((int)PACKET_XLOAD.CMD_ERASE_FLASH, 0);

                        FwUpdate.nextmachState = 2;
                        FwUpdate.machState = 1;
                        break;
                    case 1:
                        /* wait for ack */                         
                        if (AckReceived)
                            FwUpdate.machState = FwUpdate.nextmachState;
                        break;
                    case 2:
                        {
                            fhandle.Open(FwUpdate.binaryFilename, out fStream, 2);
           //                 Payload = fwUpdate.HexToBin(10, fStream, out value);
                            fileChecksum = value;

                            while (Payload.Length != 0)
                            {
                  //              Payload = fwUpdate.hextobin(10, fStream, out value);
                                fileChecksum += value;
                            }
                            fhandle.Close(fStream);

                            fhandle.Open(FwUpdate.binaryFilename, out fStream, 2);
                            // fhandle.Read(fStream, 0, Payload);

                            fileLocation = fhandle.Tell(fStream);

//                            Payload = fwUpdate.HexToBin(10, fStream, out value);

                            BuildSerialMessage((int)PACKET_XLOAD.CMD_FW_UPDATE_PACKET, 0);

                            FwUpdate.eraseComplete = true;

                            FwUpdate.uploadTimeout = false;

                            FwUpdate.machState = 1;
                            FwUpdate.nextmachState = 4;
                        }
                        break;
                    case 4:
               //         Payload = fwUpdate.HexToBin(10, fStream, out value);

                        if (Payload.Length == 0)
                        {
                            Array.Resize<byte>(ref Payload, (int)2);
                            Payload[0] = (byte)((fileChecksum & 0xff00) >> 8);
                            Payload[1] = (byte)((fileChecksum & 0x00ff));
                            BuildSerialMessage((int)PACKET_XLOAD.CMD_FW_UPDATE_COMPLETE, 0);

                            FwUpdate.machState = 1;
                            FwUpdate.nextmachState = 5;

                            fhandle.Close(fStream);
                        }
                        else
                        {
                            FwUpdate.machState = 1;
                            FwUpdate.nextmachState = 4;

                            BuildSerialMessage((int)PACKET_XLOAD.CMD_FW_UPDATE_PACKET, 0);
                        }
                        break;
                    case 5:
                        BuildSerialMessage((int)PACKET_XLOAD.CMD_RESET, 0);
                        return;
                        break;
                    case 10:
                        break;
                }
            }
        }
		
        public bool StartFwUpdate(string filename, int medium, float fwVersion, int epAddr, string portname, bool withErase)
        {
            byte[] dataArray = new byte[128];

            FwUpdate.binaryFilename = filename;
            FwUpdate.fwVersion = fwVersion;

            try
            {
                fhandle.Open(FwUpdate.binaryFilename, out fStream, 2);
                fhandle.Read(fStream, dataArray.Length, dataArray);
            }
            catch (Exception ex)
            {
                return false;
            }

            if (dataArray[0] != ':')
                return false;

            if (!FwUpdateTimer.Enabled)
            {
                if (SerialFwUpdateThread != null)
                {
                    SerialFwUpdateThread.Abort();
                    SerialFwUpdateThread = null;
                }

                if (medium == 0 || medium == 1) // serial
                {
                   // SerialFwUpdateThread = new System.Threading.Thread(SerialFwUpdate);
                    SerialFwUpdateThread.Start();
                }
                else if (medium == 2) // MLink
                {
                    if (MlinkFwUpdateThread != null)
                    {
                        MlinkFwUpdateThread.Abort();
                        MlinkFwUpdateThread = null;
                    }

                //    MlinkFwUpdateThread = new System.Threading.Thread(MlinkFwUpdate);
                    MlinkFwUpdateThread.Start();
                }
                else if (medium == 3) // BLE
                {
                    if (BleFwUpdateThread != null)
                    {
                        BleFwUpdateThread.Abort();
                        BleFwUpdateThread = null;
                    }

                    BleFwUpdateThread = new System.Threading.Thread(BleFwUpdate);
                    BleFwUpdateThread.Start();
                }
            }


            if (!TimeoutTimer.Enabled)
            {
                TimeoutTimer.Interval = 100;
                TimeoutTimer.Tick += new EventHandler(TimeoutTimer_Tick);
                TimeoutTimer.Start();

                TimeoutTimer.Enabled = true;
            }

            FwUpdate.percentComplete = 0;

            FwUpdate.erase = withErase;

            FwUpdate.machState = 0;

            return true;
        }
        private void TimeoutTimer_Tick(object sender, EventArgs e)
        {
            FwUpdate.timeoutCnt++;
        }

        private void FileMonitorTimer_Tick(object sender, EventArgs e)
        {
            FolderSize = 0;
            FileCnt = 0;


            for (int j = 0; j < NbrCameras; j++)
            {
                CameraInfo[j].fileSize =0;
                CameraInfo[j].fileCnt =0;
            }

            foreach (string file in Directory.GetFiles("C:\\WinAMC\\capture"))
            {
                if (File.Exists(file))
                {
                    FileInfo finfo = new FileInfo(file);

                    if (finfo.FullName.Contains(".png"))
                    {
                        FolderSize += finfo.Length;
                    }

                    if (finfo.FullName.Contains("c1_"))
                    {
                        CameraInfo[0].fileSize += finfo.Length;
                        CameraInfo[0].fileCnt++;
                    }
                    if (finfo.FullName.Contains("c2_"))
                    {
                        CameraInfo[1].fileSize += finfo.Length;
                        CameraInfo[1].fileCnt++;
                    }
                    if (finfo.FullName.Contains("c3_"))
                    {
                        CameraInfo[2].fileSize += finfo.Length;
                        CameraInfo[2].fileCnt++;
                    }
                    if (finfo.FullName.Contains("c4_"))
                    {
                        //CameraFileSize[1] += finfo.Length;
                        CameraInfo[3].fileSize += finfo.Length;
                        CameraInfo[3].fileCnt++;
                    }
                }

            }

            for (int j = 0; j < NbrCameras; j++)
            {
                FileCnt += CameraInfo[j].fileCnt;
            }

            if ((PrevFileCnt != FileCnt) &&
                CameraInfo[0].captureStarted
                )
            {
                PhotoCaptureStarted = true;
            }

            PrevFileCnt = FileCnt;

            CapturedFileSize = FolderSize / 1000000;// theSize /1000;
 //           CameraOperationLbl.Text = "Camera capture started " + CapturedFileSize.ToString("0.00") + "MB\nFile Count: " + FileCnt;

            for (int j = 0; j < NbrCameras; j++)
            {
                CameraDataGridView[(int)CAMERA_GRID.DATA, j].Value = (CameraInfo[j].fileSize / 1000000).ToString("0.00");
                CameraDataGridView[(int)CAMERA_GRID.FILE_CNT, j].Value = CameraInfo[j].fileCnt;
            }
                                
        }

        private void FtpUploadTimer_Tick(object sender, EventArgs e)
        {
            string ftpFileName = FtpServerTextBox.Text + SnLoadComboBox.Text + ".zip";
            var request = (FtpWebRequest)FtpWebRequest.Create(ftpFileName);
            request.Credentials = new NetworkCredential("jclugston", "Trig2017");
            request.KeepAlive = false;
            //Use the GetFileSize FILE SIZE Protocol method
            request.Method = WebRequestMethods.Ftp.GetFileSize;
            request.UseBinary = true;

            int x = 0;
            long fileSize = 0;

            try
            {
                FtpWebResponse response = (FtpWebResponse)request.GetResponse();
                fileSize = response.ContentLength;

                if (fileSize > 0)
                    x = 2;
                response.Close();
                request.Abort();
            }
            catch (Exception ex)
            {
                FtpUploadTimer.Enabled = false;

                if( fileSize >0 )
                    MessageBox.Show("File uploaded complete", "FTP", MessageBoxButtons.OK, MessageBoxIcon.None);
                else
                    MessageBox.Show("File uploaded failed", "FTP", MessageBoxButtons.OK, MessageBoxIcon.None);
            }
        }

        private void AutoConnectTimer_Tick(object sender, EventArgs e)
        {
            switch (DeviceConnectState)
            {
                case 0:
                    lvDevices.Items[ProjectDevices[ClientInfo.Count]].Selected = true;
                    lvDevices.Select();

                    BleConnectBtn_Click(null, null);

                    ProjectDeviceConnect = false;
                    DeviceConnectState = 1;
                    break;
                case 1:
                    if (ProjectDeviceConnect)
                    {
                        if (ClientInfo.Count == NbrProjectDevices)
                        {
                            AutoConnectTimer.Enabled = false;
                            DeviceConnectState = 2;
                        }
                        else
                            DeviceConnectState = 0;
                    }
                    break;
            }
        }

        private void lvDevices_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            BleConnectBtn_Click(null, null);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            int res = 0;

            res = FindClientBrdId((int)BOARD_TYPE.COMPR_CNTRL);
            if (res == -1)
            {
                DialogResult result1 = MessageBox.Show("Compressor controller not connected, do you wish to proceed",
                                "Warning",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Warning
                               );

                if (result1 == DialogResult.No)
                    return;
            }
            else
            {
                if(PumpSelectComboBox.SelectedIndex == 0)
                    ComprNbr = 0x01;
                else
                    ComprNbr = 0x02;

                BuildSerialMessage((int)PACKET.CMD_START_COMPR, res);
            }
        }

        private void button3_Click_2(object sender, EventArgs e)
        {
            int res = 0;

            res = FindClientBrdId((int)BOARD_TYPE.COMPR_CNTRL);
            if (res == -1)
            {
                DialogResult result1 = MessageBox.Show("Compressor controller not connected, do you wish to proceed",
                                "Warning",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Warning
                               );

                if (result1 == DialogResult.No)
                    return;
            }
            else
            {
                if (PumpSelectComboBox.SelectedIndex == 0)
                    ComprNbr = 0x01;
                else
                    ComprNbr = 0x02;
                BuildSerialMessage((int)PACKET.CMD_STOP_COMPR, res);
            }
        }
    }       
}
