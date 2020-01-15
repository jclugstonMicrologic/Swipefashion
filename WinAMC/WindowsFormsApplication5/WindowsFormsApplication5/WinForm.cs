using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Net.Mail;

using STRUCTARRAY;
using ERRORCHECK;
using FileHandling;
using TcpClientServer;
using EmailHandler;
using SerialCom;

using Gigasoft.ProEssentials.Enums;

using System.Runtime.InteropServices; // required for DllImport( string, entrypoint )

using System.Diagnostics;

namespace WindowsFormsApplication5
{
    enum SERIAL_COMMS
    {
       PORT =0,
       BAUDRATE =57600 //115200
    }

    enum PACKET
    {
        DLE = 0x10,
        STX = 0x02,
        CF = 0x03,  // control field
        ETX = 0x03,  // control field

        SIZEOF_HEADER = 0x04,

        CMD_GET_FTC_SETUP = 0x2000,
        CMD_SET_FTC_SETUP = 0x2001,
        CMD_GET_MOTOR_DATA =0x2002,
        CMD_GET_FTC_OP   =0x2003,
        CMD_CLEAR_PULSER_OP = 0x2004,
        CMD_GET_ANALOGS = 0x2005,
        CMD_START_MOTOR =0x2006,
        CMD_GET_FTC_LOG = 0x2007,
        CMD_CLEAR_LOG = 0x2008,
        CMD_RESTART =0x2009,
        CMD_RESET = 0x200A,

        CMD_GET_VERSION = 0x2FFF,

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

            this.Text = String.Format("WinMAN {0}", String.Format(" v{0}", aboutBox.AssemblyVersion));

            DeployComboBox.SelectedIndex = -1;
        }

        [DllImport("UsbComms.dll", EntryPoint = "OpenUsbPort")]
        public static extern bool OpenUsbPort();

        cServer tcpServer =new cServer();
        cClient tcpClient = new cClient();

        public SerialFd serialFd = new SerialFd();

        public byte[] Payload = new byte[2000];
        public uint PayloadSize;

        string server = null;
        int port = (int)CLIENTS.PORT;
        int EmailCounter =0;
        int NbrEmails = 0;
        int MaxRecords = 7;

        public int PanelSelect = 0;

        public struct MAN_OP
        {
            public byte ballCount;
            public byte machState;

            public UInt16 vBatt;
            public UInt16 iBatt;

            public byte ioStates;
            public byte pulseCnt;

            public UInt32 timeStamp;
            public UInt32 timeOfTravel;
   
 //           public UInt16 crc;
        }

        public struct ANALOGS
        {
            public UInt16 vBatt;
            public UInt16 iBatt;
            public UInt16 spareA2;
            public UInt16 spareA3;
            public UInt16 spareA4;
            public UInt16 accelCal;
            public UInt16 accelY;
            public UInt16 accelX;
        }

        public struct MAN_SETUP
        {
            public byte setPoint;
            public byte deploy;
            public byte seqTime;
            public byte temp;
            public UInt32 maxTravTime;

            public UInt32 pcTimeStamp;

            public UInt16 deployWaitState;

            public UInt16 logAddr;

            public byte detailedLog;

            public byte externalEeprom;

            public byte setupInfo; //bit0 trigger on both switches or only one 

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
            public byte[] spare;

            public byte presSec;

            public UInt16 crc;
        }

        public struct MOTOR_DATA
        {
            public float speed;
            public float relativeAngle;
        }

        public MAN_OP FtcOp = new MAN_OP();
        public ANALOGS Analogs = new ANALOGS();
        public MAN_SETUP FtcSetup = new MAN_SETUP();
        public MOTOR_DATA MotorData = new MOTOR_DATA();
                
        public UInt16 Counter =0;
        public UInt16 DownloadStart = 0;
        public UInt16 EraseCnt = 0;
        public byte MotorRequest = 0;
        public UInt32 PrevFtcTimeStamp = 0;
        public UInt16 PrevBallCount = 0;
        public UInt16 TooFastBallCount = 0;

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
                dataGridView.Rows.Add("Parameter " + i.ToString(), "", "", "");
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


        private void FileHandleBtn_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "text Files (*.txt)|*.txt";
            openFileDialog1.FileName = "";

            openFileDialog1.Title = "File1";

            if (openFileDialog1.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                // user has canceled save
                return;
            }

            FileStream fs;
            byte[] dataArray = new byte[100];
            for (int i = 0; i < dataArray.Length; i++)
            {
                dataArray[i] = (byte)i;
            }

            cFileHandle fhandle = new cFileHandle();

            fhandle.Open(openFileDialog1.FileName, out fs);
            fhandle.Write(fs, dataArray.Length, dataArray);

            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
            string theString = "hello";
            dataArray = encoding.GetBytes(theString);

            fhandle.Write(fs, dataArray.Length, dataArray);
            fhandle.Read(fs, dataArray.Length, dataArray);
            fhandle.Close(fs);

        }

        private void button1_Click(object sender, EventArgs e)
        {
            int myValue = 0;
            Class1 theClass = new Class1();
            theClass.setValue(2);
            theClass.getValue(out myValue);
        }

        private void InitBasicGraph
        (
           string title_,
           string xaxisLbl_,
           string yaxisLbl_
        )
        {
/*
           PEGraph.PeFunction.Reset(); 
           PEGraph.PeConfigure.PrepareImages = true;
           PEGraph.Left =100;
           PEGraph.Top =29;
           PEGraph.Width =550;
           PEGraph.Height =325;
//           PEGraph.BringToFront();

           PEGraph.PeData.Subsets = 2;
           PEGraph.PeData.Points =1000;

           PEGraph.PeString.MainTitle = title_;
           PEGraph.PeString.SubTitle = "";

           // Set X scale label stuff bottom
//           PEGraph.XAxisOnTop =false;
           PEGraph.PeAnnotation.Line.XAxisColor[0] = System.Drawing.Color.Red;
           PEGraph.PeString.XAxisLabel = xaxisLbl_;
           
           // Set Manual X scale bottom
//           PEGraph.PeGrid.Configure.XAxisScaleControl = Gigasoft.ProEssentials.Enums.ScaleControl.Linear;
//           PEGraph.PeGrid.Configure.ManualScaleControlX = ManualScaleControl.MinMax;

           // Set Manual Y scale
           PEGraph.PeGrid.Configure.ManualScaleControlY = ManualScaleControl.MinMax;
           PEGraph.PeGrid.Configure.ManualMaxY =4000;
           PEGraph.PeGrid.Configure.ManualMinY =0;
           PEGraph.PeString.YAxisLabel = yaxisLbl_;
           PEGraph.PeAnnotation.Line.YAxisColor[0] =System.Drawing.Color.Black;

           PEGraph.PeUserInterface.Allow.Zooming = AllowZooming.HorzAndVert;

           PEGraph.PePlot.MarkDataPoints =true;
           PEGraph.PePlot.Method = GraphPlottingMethod.Line;
           PEGraph.Visible =true;

           for (int i =0; i < 10; i++)
           {
               PEGraph.PeData.Y[0, i] = 0;
               PEGraph.PeData.Y[1, i] = 0;
           }

           for (int i =0; i <10; i++)
           {
              PEGraph.PeData.Y[0, i] =i+1;
           }
           for (Int32 i = 0; i <100; i++)
           {
               PEGraph.PeData.Y[1, i] = 5*(float)(System.Math.Sin(i / 10.0) + System.Math.Sin(3 * i / 10.0)/3);
           }

           PEGraph.PeFunction.ReinitializeResetImage();
*/
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Width = 515;
            this.Height = 595;

            InitDataGrid();

            InitDownloadGrid();

            MainPanel.Top = 40;
            MainPanel.Left = 10;
            MainPanel.Width = 770;
            MainPanel.Height = 520;

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

            /*
            for (int j = 1; j < 32; j++)
            {
                String aStr;
                String result;
                int findStr;

                aStr = "COM" + j.ToString();

                result = serialFd.OpenPort((int)SERIAL_COMMS.PORT, (int)SERIAL_COMMS.BAUDRATE, aStr);
                findStr = result.IndexOf("Access to the port");

                if (result == "OK" ||
                   findStr != -1
                  )
                {
                    CommportComboBox.Items.Add(aStr);
                }
            }
            try
            {
                CommportComboBox.SelectedIndex = 0;
            }
            catch (Exception ex)
            {
                    MessageBox.Show("No Serial Ports Were Found",
                                    "Serial",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Warning
                                   );
     

            }
     */

   //         InitBasicGraph("BLDC Motor", "Time", "Speed");

            Sw1TextBox.BackColor = System.Drawing.Color.LightGray;
            Sw2TextBox.BackColor = System.Drawing.Color.LightGray;
            Sw3TextBox.BackColor = System.Drawing.Color.LightGray;
            Sw4TextBox.BackColor = System.Drawing.Color.LightGray;
            CommTextBox.BackColor = System.Drawing.Color.LightGray;
            EncTextBox.BackColor = System.Drawing.Color.LightGray;
            SuTextBox.BackColor = System.Drawing.Color.LightGray;
            SlTextBox.BackColor = System.Drawing.Color.LightGray;

            PanelSelect = (int)SET_COMMANDS.SET_COMMPORT;

            PanelsFrm panelsForm = new PanelsFrm(this);
            panelsForm.ShowDialog();
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

           tcpServer.StartServerSocket();

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

           while (true)
           {
              byte[] rxBuffer = new byte[1056];
              byte sw1State = 0;
              byte sw2State = 0;
              byte sw3State = 0;
              byte sw4State = 0;
              byte commState = 0;
              byte encState = 0;
              byte stuckSwitchUpper = 0;
              byte stuckSwitchLower = 0;
              
              float vBatt, iBatt;

              nbrBytesRx = serialFd.ReceiveMessage(0, 100, rxBuffer);

              if (nbrBytesRx != 0)
              {
                  SerialMonitorTimer.Enabled = false;

//                  RxListBox.Items.Add(rxBuffer[0].ToString("x") );

                  if (ParseMessage(out response, nbrBytesRx, rxBuffer))
                  {                     
                     switch(response)
                     {
                         case (int)PACKET.CMD_GET_FTC_SETUP:
                             if (FtcSetup.crc != 0x5aa5)
                             {
                                 MessageBox.Show("FTC Setup parameter error",
                                                 "FTC Communications",
                                                 MessageBoxButtons.OK,
                                                 MessageBoxIcon.Error
                                                );
                             }
                             if (FtcSetup.deploy == 0xff)
                             {
                                 MessageBox.Show("FTC Setup Erased",
                                                 "FTC Erased",
                                                  MessageBoxButtons.OK,
                                                  MessageBoxIcon.Warning
                                                );

                                 TriggerTextBox.Text ="0";
                                 DeployTextBox.Text = "0";
                                 DeployDelayTime.Text = "0";

                                 DeployComboBox.SelectedIndex = 1;
                                 LogTypeComboBox.SelectedIndex = 0;
                             }
                             else
                             {
                                 TriggerTextBox.Text = FtcSetup.setPoint.ToString();
                                 DeployTextBox.Text = FtcSetup.deploy.ToString();
                                 DeployDelayTime.Text =FtcSetup.deployWaitState.ToString(); // FtcSetup.maxTravTime.ToString();

                                 DeployComboBox.SelectedIndex = FtcSetup.deploy;
                                 LogTypeComboBox.SelectedIndex = FtcSetup.detailedLog;
                                 SwitchTriggerComboBox.SelectedIndex = (FtcSetup.setupInfo &0x01);
                             }

                             if (FtcSetup.externalEeprom == 1)
                             {
                                MaxRecords = 64;
                                label27.Text = "     Extended EEPROM";
                             }
                             else
                             {
                                MaxRecords = 7;
                                label27.Text = "     Standard EEPROM";
                             }
                             
                             break;
                         case (int)PACKET.CMD_SET_FTC_SETUP:
         
                             MessageBox.Show("FTC Setup Successfully for\n\n              "+ DeployComboBox.Text,
                                             "FTC Communications",
                                              MessageBoxButtons.OK                                            
                                            );
                             break;
                         case (int)PACKET.CMD_GET_FTC_OP:
                             sw1State = (byte)((FtcOp.ioStates & 0x01));
                             sw2State = (byte)((FtcOp.ioStates & 0x02) >> 1);
                             sw3State = (byte)((FtcOp.ioStates & 0x04) >> 2);
                             sw4State = (byte)((FtcOp.ioStates & 0x08) >> 3);
                             commState =(byte)((FtcOp.ioStates & 0x10) >> 4);
                             encState = (byte)((FtcOp.ioStates & 0x20) >> 5);

                             stuckSwitchUpper = (byte)((FtcOp.ioStates & 0x40) >> 6);
                             stuckSwitchLower = (byte)((FtcOp.ioStates & 0x80) >> 7);
                             
                             vBatt = (float)(((float)FtcOp.vBatt) / 1024 * 3.3 * 6);
                             iBatt = (float)(((float)FtcOp.iBatt) / 1024 * 3.3 /5.53)/(float)0.05;

                             VBattTextBox.Text = vBatt.ToString("0.00");
                             IBattTextBox.Text = iBatt.ToString("0.00");
                             BallCntTextBox.Text = FtcOp.ballCount.ToString();
                             PulseCntTextBox.Text = FtcOp.pulseCnt.ToString();

                             if (sw1State == 1)
                                 Sw1TextBox.BackColor = System.Drawing.Color.DarkGreen; //sw1State.ToString();
                             else
                                 Sw1TextBox.BackColor = System.Drawing.Color.LightGray;

                             if (sw2State == 1)
                                 Sw2TextBox.BackColor = System.Drawing.Color.DarkGreen; //sw1State.ToString();
                             else
                                 Sw2TextBox.BackColor = System.Drawing.Color.LightGray;

                             if (sw3State == 1)
                                 Sw3TextBox.BackColor = System.Drawing.Color.DarkGreen; //sw1State.ToString();
                             else
                                 Sw3TextBox.BackColor = System.Drawing.Color.LightGray;

                             if (sw4State == 1)
                                 Sw4TextBox.BackColor = System.Drawing.Color.DarkGreen; //sw1State.ToString();
                             else
                                 Sw4TextBox.BackColor = System.Drawing.Color.LightGray;

                             if (commState == 1)
                                 CommTextBox.BackColor = System.Drawing.Color.DarkGreen; //sw1State.ToString();
                             else
                                 CommTextBox.BackColor = System.Drawing.Color.LightGray;

                             if (encState == 1)
                                 EncTextBox.BackColor = System.Drawing.Color.DarkGreen; //sw1State.ToString();
                             else
                                 EncTextBox.BackColor = System.Drawing.Color.LightGray;

                             if(stuckSwitchUpper ==1)
                                SuTextBox.BackColor = System.Drawing.Color.DarkGreen;
                             else
                                SuTextBox.BackColor = System.Drawing.Color.LightGray;

                             if (stuckSwitchLower == 1)
                                 SlTextBox.BackColor = System.Drawing.Color.DarkGreen;
                             else
                                 SlTextBox.BackColor = System.Drawing.Color.LightGray;

                             switch (FtcOp.machState)
                             { 
                                 case 4:
                                     FtcMachState.Text = "Ball Monitor State";
                                     break;
                                 case 5:
                                     FtcMachState.Text = "1 upper switch triggered, waiting for other";
                                     break;
                                 case 6:
                                     FtcMachState.Text = "1 lower switch triggered, waiting for other";
                                     break;
                                 case 7:
                                     FtcMachState.Text = "Upper switches triggered, waiting for lower";
                                     break;
                                 case 8:
                                     FtcMachState.Text = "Lower switches triggered, waiting for upper";
                                     break;
                                 case 10:
                                     FtcMachState.Text = "Ball Count complete";
                                     break;
                                 case 11:
                                     FtcMachState.Text = "Ball Fail State";
                                     break;
                                 case 13:
                                     FtcMachState.Text = "Debouncing State";
                                     break;
                                 default:
                                     FtcMachState.Text = FtcOp.machState.ToString();
                                     break;
                             }

                             FtcMachState.Text +=" (" + FtcOp.machState.ToString() + ")";
                             
                             break;
                         case (int)PACKET.CMD_GET_ANALOGS:
                             VBattTextBox.Text = Analogs.vBatt.ToString();
                             IBattTextBox.Text = Analogs.iBatt.ToString();
                             break;  
                         case (int)PACKET.CMD_GET_MOTOR_DATA:                             
                             break;
                         case (int)PACKET.CMD_GET_VERSION:
                             FwVersionLbl.Text = "FTC: " + System.Text.Encoding.ASCII.GetString(Payload);

                                //                             FpcGroupBox.Text = "FPC: " + System.Text.Encoding.ASCII.GetString(Payload);
                                //                             MessageBox.Show("FW Version: " + System.Text.Encoding.ASCII.GetString(Payload),
                                //                                           "FPC Packet",
                                //                                         MessageBoxButtons.OK                                            
                                //                                      );
                                break;
                     }
                  }
              }

              Thread.Sleep(1);
           }
        }


        private void button5_Click(object sender, EventArgs e)
        {
            String clientInfo;

            server = Dns.GetHostName();

            server = IPAddrBox.Text;

            try
            {                
                clientInfo = tcpClient.StartClient(server, port);
                RxListBox.Items.Add(clientInfo + '\n');                
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(),
                                "Exception Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
            }

        }

        private void button6_Click(object sender, EventArgs e)
        {
            String clientData;
            clientData = tcpClient.SocketSendReceive();
            RxListBox.Items.Add(clientData + '\n');
            RxListBox.SelectedIndex = RxListBox.Items.Count - 1;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            SerialMonitorTimer.Enabled = true;
            /*
            if (!BuildSerialMessage((int)PACKET.CMD_GET_MOTOR_DATA)) 
            {
                timer1.Enabled = false;
                return;
            }
*/
            if( !BuildSerialMessage((int)PACKET.CMD_GET_FTC_OP) )
            {                
                timer1.Enabled = false;
                return;
            }
/*
            if (!BuildSerialMessage((int)PACKET.CMD_GET_ANALOGS))
            {                
                timer1.Enabled = false;
                return;
            }
*/                            
            Refresh();

        }

        private void button8_Click(object sender, EventArgs e)
        {
            EmailPanel.Top = 120;
            EmailPanel.Visible =true;
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

            label6.Visible = true;
            label6.Text = openFileDialog1.FileName;

        }

        private void button7_Click_1(object sender, EventArgs e)
        {
            cSmtpEmail smtpEmail = new cSmtpEmail();
            MAIL_INFO mailInfo = new MAIL_INFO();

            String emailResponse ="";

            panel1.Visible = true;
            
            mailInfo.ssl = true;
            mailInfo.mailServer = MailServerTextBox.Text; 

            mailInfo.fromAddr = FromAddrTextBox.Text; 
            mailInfo.toAddr = ToAddrTextBox.Text;

            NbrEmails = 0;

            while( NbrEmails++ < Convert.ToInt16(textBox1.Text) )
            {
                label7.Text ="Sending, Please Wait..." +NbrEmails.ToString();               
                Application.DoEvents();

                mailInfo.subject = SubjectTextBox.Text;
                mailInfo.body = BodyRichTextBox.Text;

                mailInfo.attachment = openFileDialog1.FileName;

                mailInfo.sslCredentials.username = "anaddress@gmail.com"; 
                mailInfo.sslCredentials.password = "talonrx8";

                emailResponse = smtpEmail.SendEmail(mailInfo);
            }
  
            panel1.Visible = false;
            textBox1.Visible = false;           

            MessageBox.Show(emailResponse,
                            "Email Status",
                            MessageBoxButtons.OK
                           );

            EmailPanel.Visible = false;
        }

        private void button10_Click(object sender, EventArgs e)
        {
            EmailPanel.Visible = false;
            label6.Visible = false;

            NbrEmails = 25;
        }

        private void EmailPanel_DoubleClick(object sender, EventArgs e)
        {
            ToAddrTextBox.Text = "someaddress@somewhere.com";
            textBox1.Visible = true;

            switch( EmailCounter )
            {
                case 0:
                    BodyRichTextBox.Text ="1";   
                    break;
                case 1:
                    BodyRichTextBox.Text ="2";
                    break;
                case 2:
                    BodyRichTextBox.Text ="3";
                    break;
                case 3:
                    BodyRichTextBox.Text ="4";
                    break;
            }

            if(EmailCounter++ == 3)
                EmailCounter = 0;        
        }

        private void button11_Click(object sender, EventArgs e)
        {
           String result;

           result = serialFd.OpenPort((int)SERIAL_COMMS.PORT, (int)SERIAL_COMMS.BAUDRATE, "COM20");

           if (result == "OK")           
           {
              Thread serialThread = new Thread(rxDataThread);
              serialThread.Start();

              // get motor run state (on/off)
              serialFd.SendMessage(0, 1, (byte)'B');
           }
           else
           {
              MessageBox.Show("Serial Port Open Failed",
                              "Serial Comm",
                              MessageBoxButtons.OK,
                              MessageBoxIcon.Error
                             );
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
            FtcMachState.Visible = true;
            label27.Visible = true;
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
                case (int)PACKET.CMD_GET_FTC_SETUP:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_GET_MOTOR_DATA:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_GET_FTC_OP:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_CLEAR_PULSER_OP:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_GET_ANALOGS:
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
                case (int)PACKET.CMD_START_MOTOR:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = MotorRequest;

                    TxBuf[nbrBytes++] = Payload[0];

                    break;                    
                case (int)PACKET.CMD_SET_FTC_SETUP:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload = StructArray.StructToByteArray(FtcSetup);

                    for (int j = 0; j < Marshal.SizeOf(FtcSetup); j++)
                    {
                        TxBuf[nbrBytes++] = Payload[j];
                    }                        
                    break;
                case (int)PACKET.CMD_GET_FTC_LOG:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;

                    Payload[0] = (byte)Counter;

                    TxBuf[nbrBytes++] = Payload[0];
                    break;
                case (int)PACKET.CMD_CLEAR_LOG:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
                    break;
                case (int)PACKET.CMD_RESTART:
                    // build the message here, then send
                    ConvetToBuffer16((int)command, out tempBuf);
                    tempBuf.CopyTo(TxBuf, (int)PACKET.SIZEOF_HEADER);
                    nbrBytes = (int)PACKET.SIZEOF_HEADER + 2;
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
            calculatedCRC = errCheck.Xmodem_crc((int)(nbrBytes), (int)CRC.SEED, TxBuf);

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
            calculatedCRC = errCheck.Xmodem_crc((int)(PayloadSize - 2), (int)CRC.SEED, rxBuffer);

            /*
            // check CRC
            if (rxCrc != calculatedCRC)
            {
                response = 1;
                return true;
            }
            */

            // get payload (i.e. remove header)
            System.Array.Copy(rxBuffer, ((int)PACKET.SIZEOF_HEADER + 2), tempBuf, 0, PayloadSize);

            status = true;

            // process response (could all be done in WinForm, where GUI stuff is done)
            switch (response)
            {
                case (int)PACKET.CMD_GET_FTC_SETUP:
                    //tempBuf = StructArray.SwapByteArray16(tempBuf);

                    FtcSetup = (MAN_SETUP)StructArray.ByteArrayToStruct(typeof(MAN_SETUP), tempBuf);
                    break;
                case (int)PACKET.CMD_GET_MOTOR_DATA:
                    tempBuf = StructArray.SwapByteArray16(tempBuf);

                    MotorData = (MOTOR_DATA)StructArray.ByteArrayToStruct(typeof(MOTOR_DATA), tempBuf);

//                    RelativeAngle = BitConverter.ToSingle(tempBuf, 12);
                    break;
                case (int)PACKET.CMD_GET_FTC_OP:                 
                    FtcOp = (MAN_OP)StructArray.ByteArrayToStruct(typeof(MAN_OP), tempBuf);
                    break;
                case (int)PACKET.CMD_GET_FTC_LOG:
                    //  downloadGridView[0, 0].Value = FtcOp.timeStamp;

                    label10.Text = label10.Text = "Ball Trigger Value: " + FtcSetup.setPoint.ToString();

                    System.DateTime dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
                    dtDateTime = dtDateTime.AddSeconds(FtcSetup.pcTimeStamp).ToLocalTime();
                    SetupTimeLbl.Text = "Setup Time: " + dtDateTime.ToString("MM/dd/yy HH:mm:ss");

         //           logDoneCnt =(FtcSetup.logAddr - 32)/16;
                    for (int k = 0; k < 2; k++)
                    {
                        float vBatt, iBatt;
                        FtcOp = (MAN_OP)StructArray.ByteArrayToStruct(typeof(MAN_OP), tempBuf);

                        if (FtcOp.vBatt == 0xffff && FtcOp.iBatt == 0xffff
                           )
                        {
                            EraseCnt++;
                            button16.Enabled = true;

                            if (FtcSetup.logAddr == 32)
                            {
                                MessageBox.Show("No Data Found, Log is Erased",
                                            "FTC Log",
                                            MessageBoxButtons.OK
                                    //MessageBoxIcon.Error
                                            );
                            }
                            else 
                            {
                                MessageBox.Show("Download Complete\n" + (downloadGridView.RowCount-1).ToString() + " Records Downloaded",
                                                "FTC Log",
                                                MessageBoxButtons.OK
                                                //MessageBoxIcon.Error
                                               );

                                if (TooFastBallCount > 0)
                                {
                                    MessageBox.Show((TooFastBallCount + " missed ball counts detected due to ball travel time too fast\n").ToString(),
                                                    "FTC Log",
                                                     MessageBoxButtons.OK,
                                                    MessageBoxIcon.Warning
                                                    );
                                }
                            }
                            return true; // continue;
                        }

                        vBatt = (float)(((float)FtcOp.vBatt) / 1024 * 3.3 * 6);                  // 1/6 R divider cct
                        iBatt = (float)(((float)FtcOp.iBatt) / 1024 * 3.3 / 5.53) / (float)0.05; // 5.53 gain 0.05ohm sense resistor                        

                        dtDateTime = new DateTime(1970, 1, 1, 0, 0, 0, 0,System.DateTimeKind.Utc);
                        dtDateTime = dtDateTime.AddSeconds(FtcSetup.pcTimeStamp +(double)FtcSetup.presSec/100 + (double)FtcOp.timeStamp / 1000 + Convert.ToInt16(textBox2.Text) ).ToLocalTime();// +(double)FtcOp.timeOfTravel/1000).ToLocalTime();

                        if (FtcOp.timeStamp == PrevFtcTimeStamp)
                        {
                            for (int j = 0; j < 16; j++)
                                tempBuf[j] = tempBuf[j + 16];

                            continue;
                        }

                        PrevFtcTimeStamp = FtcOp.timeStamp;

                        if ( (byte)(FtcOp.ioStates & 0x01) ==0x01)
                            sw1State ="Closed";
                        else
                            sw1State = "Open";

                        if( (byte)(FtcOp.ioStates & 0x02) ==0x02 )
                            sw2State = "Closed";
                        else
                            sw2State = "Open";

                        if ((byte)(FtcOp.ioStates & 0x04) == 0x04)
                            sw3State = "Closed";
                        else
                            sw3State = "Open";

                        if ((byte)(FtcOp.ioStates & 0x08) == 0x08)
                            sw4State = "Closed";
                        else
                            sw4State = "Open";

                        //sw3State = (byte)((FtcOp.ioStates & 0x04) >> 2);
                        //sw4State = (byte)((FtcOp.ioStates & 0x08) >> 3);

                        if (iBatt < 0.015)
                            iBatt = 0.0F;

                        //if( FtcOp.pulseCnt != 0 && (FtcOp.ballCount == FtcSetup.setPoint))
                        if ((FtcOp.ballCount == FtcSetup.setPoint) &&
                            iBatt > 1.0
                            //(FtcOp.machState ==0x0d || FtcOp.machState ==0x03)
                            //(FtcOp.machState >= 0x0a)
                            )
                        {
                            // trigger point
                            try
                            {
                                downloadGridView.Rows.Add(dtDateTime.ToString("HH:mm:ss.ffff"), "NA", FtcOp.timeOfTravel, vBatt.ToString("0.0"), iBatt.ToString("0.000"), FtcOp.ballCount, sw1State, sw2State, sw3State, sw4State);
                            }
                            catch (Exception ex)
                            {
                            }

                            for (int j = 0; j < downloadGridView.ColumnCount; j++)
                            {
                                downloadGridView[j, downloadGridView.RowCount - 2].Style.BackColor = Color.GreenYellow;
                            }
                        }
                        else if(PrevBallCount != 0 &&
                                FtcOp.ballCount == PrevBallCount
                             )
                        {
                            try
                            {
                                downloadGridView.Rows.Add(dtDateTime.ToString("HH:mm:ss.ffff"), FtcOp.timeOfTravel, "NA", vBatt.ToString("0.0"), iBatt.ToString("0.000"), FtcOp.ballCount, sw1State, sw2State, sw3State, sw4State);
                            }
                            catch (Exception ex)
                            {
                            }

                            for (int j = 0; j < downloadGridView.ColumnCount; j++)
                            {
                                downloadGridView[j, downloadGridView.RowCount - 2].Style.BackColor = Color.Red;
                            }

                            TooFastBallCount++;
                        }
                        else
                        {
                            try
                            {
                                downloadGridView.Rows.Add(dtDateTime.ToString("HH:mm:ss.ffff"), FtcOp.timeOfTravel, "NA", vBatt.ToString("0.0"), iBatt.ToString("0.000"), FtcOp.ballCount, sw1State, sw2State, sw3State, sw4State);
                            }
                            catch (Exception ex)
                            {
                            }
                        }

                        PrevBallCount = FtcOp.ballCount;

                        for (int j = 0; j < 16; j++)
                            tempBuf[j] = tempBuf[j + 16];
                    }

                    if (++DownloadStart >= MaxRecords)
                    {
                        button16.Enabled = true;

                        MessageBox.Show("Download Complete\n" + (downloadGridView.RowCount - 1).ToString() + " Records Downloaded",
                                        "FTC Log",
                                        MessageBoxButtons.OK
                                        //MessageBoxIcon.Error
                                        );


                        if (TooFastBallCount > 0)
                        { 
                            MessageBox.Show( (TooFastBallCount + " missed ball counts detected due to ball travel time too fast\n").ToString(),
                                            "FTC Log",
                                             MessageBoxButtons.OK,
                                            MessageBoxIcon.Warning
                                            );
                        }
                        return true;
                    }
                    else
                    {
                        Thread.Sleep(10);
                        BuildSerialMessage((int)PACKET.CMD_GET_FTC_LOG);
                    }
/*
                    else
                    {
                        button16.Enabled = true;

                        if (EraseCnt >= 14)
                        {
                            MessageBox.Show("No Data Found, Log is Erased",
                                            "FTC Log",
                                            MessageBoxButtons.OK
                                //MessageBoxIcon.Error
                                            );
                        }
                    }
 */

                    break;
                case (int)PACKET.CMD_GET_ANALOGS:
                    //tempBuf = StructArray.SwapByteArray16(tempBuf);

                    Analogs = (ANALOGS)StructArray.ByteArrayToStruct(typeof(ANALOGS), tempBuf);
                    break;
                case (int)PACKET.CMD_GET_VERSION:
                    System.Array.Copy(tempBuf, 0, Payload, 0, (PayloadSize - 8));
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

        private void button11_Click_1(object sender, EventArgs e)
        {

        }

        private void fPCToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MainPanel.Visible = false;
            DiagnosticPnl.Visible = false;
            DownloadPanel.Visible = true;

            BuildSerialMessage((int)PACKET.CMD_GET_FTC_LOG);
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
                FtcSetup.setPoint = Convert.ToByte(TriggerTextBox.Text);

                if(FtcSetup.setPoint ==0)
                {
                    MessageBox.Show("Ball trigger cannot be 0",
                                    "Table Setup Error",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                   );

                    return;
                }
                FtcSetup.deploy = Convert.ToByte(DeployComboBox.SelectedIndex );//DeployTextBox.Text);

                FtcSetup.setupInfo = Convert.ToByte(SwitchTriggerComboBox.SelectedIndex);//DeployTextBox.Text);

                FtcSetup.deployWaitState = Convert.ToUInt16(DeployDelayTime.Text); // Convert.ToByte(DeployComboBox.SelectedIndex);//DeployTextBox.Text);
  //              PulserSetup.spare[0] = Convert.ToByte(P3TextBox.Text);                

                DateTime localDate = DateTime.Now;
                DateTime origin = new DateTime(1970, 1, 1, 0, 0, 0, 0);
                TimeSpan diff = localDate.ToUniversalTime() - origin;
//                double x=diff.TotalSeconds;
                
                FtcSetup.pcTimeStamp =(UInt32)diff.TotalSeconds;

                UInt64 theDiff = (UInt64)diff.TotalMilliseconds;
                theDiff = theDiff % 1000;
                FtcSetup.presSec = (byte)(theDiff / 10);

                FtcSetup.detailedLog = Convert.ToByte(LogTypeComboBox.SelectedIndex);

                FtcSetup.crc = 0x5aa5;

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
            BuildSerialMessage((int)PACKET.CMD_GET_FTC_SETUP);
        }

        private void ClearFpcLogBtn_Click_1(object sender, EventArgs e)
        {
            DialogResult result1 = MessageBox.Show("Are you sure you wish to clear the FTC log",
                                                   "Memory Clear",
                                                    MessageBoxButtons.YesNo,
                                                    MessageBoxIcon.Warning
                                                   );

            if (result1 == DialogResult.No)
                return;

            if (!BuildSerialMessage((int)PACKET.CMD_CLEAR_LOG))
                return;

            MessageBox.Show("Log Erased Succesfully",
                            "FTC Log",
                            MessageBoxButtons.OK
                           );
        }

        private void button12_Click_2(object sender, EventArgs e)
        {
            if(button12.Text == "Start")
            {
                TriggerTextBox.Text ="";
                DeployTextBox.Text ="";

                // get FTC fw version
                if (!BuildSerialMessage((int)PACKET.CMD_GET_VERSION))
                    return;

                // get FTC setup information
                if (!BuildSerialMessage((int)PACKET.CMD_GET_FTC_SETUP))
                    return;

                button12.Text ="Stop";
                timer1.Enabled = true;
            }
            else
            {
                button12.Text = "Start";
                timer1.Enabled = false;
            }
        }

        private void SerialMonitorTimer_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            SerialMonitorTimer.Enabled = false;
            button12.Text = "Start";

            MessageBox.Show("No response from FTC",
                            "FTC Communications",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Error
                           );
            
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
//            if (!BuildSerialMessage((int)PACKET.CMD_GET_MOTOR_DATA))
//                return;

            if (!BuildSerialMessage((int)PACKET.CMD_START_MOTOR))
                return;
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
            BuildSerialMessage((int)PACKET.CMD_GET_MOTOR_DATA);

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

            FtcMachState.Visible = false;
            label27.Visible = false;

            downloadGridView.Rows.Clear();
        }

        private void button16_Click_1(object sender, EventArgs e)
        {
            if( button12.Text =="Stop")
            {
                DialogResult result1 = MessageBox.Show("Real time updates are running, click OK to continue with download",
                                                       "Download",
                                                       MessageBoxButtons.OKCancel,
                                                       MessageBoxIcon.Warning
                                                      );

                if (result1 == DialogResult.Cancel)
                    return;
            }

            PrevFtcTimeStamp = 0;
            PrevBallCount = 0;
            TooFastBallCount = 0;

            button12.Text = "Start";
            timer1.Enabled = false;

            button16.Enabled = false;
            DownloadStart = 0;
            EraseCnt = 0;
            downloadGridView.Rows.Clear();

            BuildSerialMessage((int)PACKET.CMD_GET_FTC_SETUP);
            BuildSerialMessage((int)PACKET.CMD_GET_FTC_LOG);
        }
      
        public void UpdateStatusStrip(String aStr)  
        {
            FtcStatusStrip.Items[0].Text = aStr;
        }
       
        private void button18_Click_1(object sender, EventArgs e)
        {
            // send stop command
            MotorRequest = 0;

            BuildSerialMessage((int)PACKET.CMD_START_MOTOR);          
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

            fhandle.Open(saveFileDialog1.FileName, out fs);

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
            MotorRequest = 0x81;

            BuildSerialMessage((int)PACKET.CMD_START_MOTOR);
        }

        private void button19_Click(object sender, EventArgs e)
        {
            MotorRequest = 0x80;

            BuildSerialMessage((int)PACKET.CMD_START_MOTOR);

            
            // command line stuff
      //      Process process = new Process();
        //    process.StartInfo.FileName = "convbin.exe";
         //   process.StartInfo.WorkingDirectory = "C:\\PhotoGeoTag";
          //  process.StartInfo.Arguments = "test.obs ubx_20090515c.ubx";
           // process.Start();
            

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

            BuildSerialMessage((int)PACKET.CMD_RESTART);  
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
    }
}
