using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

using System.IO.Ports;
using System.Management;
using System.Text.RegularExpressions;

using SerialCom;
using WindowsFormsApplication5;
                

namespace WindowsFormsApplication5
{
    enum SET_COMMANDS
    {
        SET_COMMPORT = 1,
        SET_FPC_PARAM = 2,
        SET_FPC_COUNTER = 3,

    }

    public partial class PanelsFrm : Form
    {
//        SerialFd serialFd = new SerialFd();

        private TheMainForm theMainForm = null;

        public PanelsFrm(TheMainForm mainForm)
        {
            InitializeComponent();

            this.theMainForm = mainForm;

            CommportBox.Top = 10;
            CommportBox.Left =10;

            FpcParamBox.Top = CommportBox.Top;
            FpcParamBox.Left =CommportBox.Left;

            CounterBox.Top = CommportBox.Top;
            CounterBox.Left =CommportBox.Left;

            switch (this.theMainForm.PanelSelect)
            {
                case (int)SET_COMMANDS.SET_FPC_PARAM:
                    this.Height = 250;
                    this.Width  = 450;

                    //P1TextBox.Text = this.theMainForm.PulserSetup.p1.ToString();
                    //P2TextBox.Text = this.theMainForm.PulserSetup.p2.ToString();
                    //P3TextBox.Text = this.theMainForm.PulserSetup.p3.ToString();
                    //P4TextBox.Text = this.theMainForm.PulserSetup.p4.ToString();
                    //P5TextBox.Text = this.theMainForm.PulserSetup.p5.ToString();
                    //P6TextBox.Text = this.theMainForm.PulserSetup.p6.ToString();

                    FpcParamBox.Visible =true;
                    break;
                case (int)SET_COMMANDS.SET_FPC_COUNTER:
                    this.Height = 220;
                    this.Width = 220;

                    CounterBox.Visible = true;
                    break;
                case (int)SET_COMMANDS.SET_COMMPORT:
                    /*
                    this.Height = 220;
                    this.Width = 220;
                    CommportBox.Visible =true;

                    this.theMainForm.UpdateStatusStrip("Disconnected");

                    for (int j = 1; j < 32; j++)
                    {
                        String aStr;
                        String result;
                        int findStr;

                        aStr = "COM" + j.ToString();

                        result = this.theMainForm.serialFd.OpenPort((int)SERIAL_COMMS.PORT, (int)SERIAL_COMMS.BAUDRATE, aStr);
                        findStr = result.IndexOf("Access to the port");

                        if (result == "OK" ||
                           findStr != -1
                          )
                        {
                            ComportBox.Items.Add(aStr);
                        }
                    }
                    
                    try
                    {
                        ComportBox.SelectedIndex = 0;
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
                    this.Height = 260;
                    this.Width = 300;

                    SerialFd serialFd = new SerialFd();

                    //HideAllPanels();
                    int WirelessDonglePort = 0;

                    CommportBox.Visible = true;

                    ManagementClass processClass = new ManagementClass("Win32_PnPEntity");
                    ManagementObjectCollection Ports = processClass.GetInstances();
                    string[] device = new string[10];// = "No recognized";

                    device[0] = "No recognized";

                    foreach (ManagementObject property in Ports)
                    {
                        if (property.GetPropertyValue("Name") != null)
                            if (//property.GetPropertyValue("Name").ToString().Contains("XDS110") &&
                                //property.GetPropertyValue("Name").ToString().Contains("COM"))
                                property.GetPropertyValue("Name").ToString().Contains("XDS110 Class Application/User UART"))
                            {
                                Console.WriteLine(property.GetPropertyValue("Name").ToString());
                                device[0] = property.GetPropertyValue("Name").ToString();
                                break;
                            }
                    }

                    string[] ports = SerialPort.GetPortNames();
                    int portNbr = 0;

                    foreach (string port in ports)
                    {
                        //                    @"\bHello\b"
                        string matchStr = "\\b" + ports[portNbr] + "\\b";

                        ComportBox.Items.Add(port);// + " " + device[0]);
                        ComportBox.SelectedIndex = portNbr;


                        if (Regex.IsMatch(device[0], @matchStr))
                        {
                            //ComportSelectBox.Enabled = false;
                            //BaudrateSelectBox.Enabled = false;
                            ComDescLbl.Text = device[0];

                            WirelessDonglePort = portNbr;
                            //break;
                        }

                        portNbr++;
                    }

                    try
                    {
                        //   ComportSelectBox.SelectedIndex = 0;
                        ComportBox.Select();
                        ComportBox.Focus();

                        ComportBox.SelectedIndex = WirelessDonglePort;
                        //BaudrateSelectBox.SelectedIndex = 10;
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("No Communication Ports Were Found",
                                        "TRIG Communication",
                                        MessageBoxButtons.OK,
                                        MessageBoxIcon.Warning
                                       );

                    }
                    break;
            }

        }


        private void button1_Click_1(object sender, EventArgs e)
        {
            string result, aStr;

            this.theMainForm.serialFd.ClosePort((int)SERIAL_COMMS.PORT);

            aStr = ComportBox.Items[ComportBox.SelectedIndex].ToString();

            result = this.theMainForm.serialFd.OpenPort((int)SERIAL_COMMS.PORT, (int)SERIAL_COMMS.BAUDRATE, aStr);

            if (result == "OK")
            {
                this.theMainForm.serialThread = new Thread(this.theMainForm.rxDataThread);
                this.theMainForm.serialThread.Start();

                aStr = ComportBox.Text + ", 57600bps";

                this.theMainForm.UpdateStatusStrip(aStr);
                // get FPC fw version
                this.theMainForm.BuildSerialMessage((int)PACKET.CMD_GET_VERSION);
            }
            else
            {
                this.theMainForm.UpdateStatusStrip("Disconnected");

                MessageBox.Show("Serial Port Open Failed",
                                "Serial Comm",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            try
            {
//                this.theMainForm.PulserSetup.p1 = Convert.ToUInt16(P1TextBox.Text);
  //              this.theMainForm.PulserSetup.p2 = Convert.ToUInt16(P2TextBox.Text);
    //            this.theMainForm.PulserSetup.p3 = Convert.ToUInt16(P3TextBox.Text);
      //          this.theMainForm.PulserSetup.p4 = Convert.ToUInt16(P4TextBox.Text);
         //       this.theMainForm.PulserSetup.p5 = Convert.ToUInt16(P5TextBox.Text);
           //     this.theMainForm.PulserSetup.p6 = Convert.ToUInt16(P6TextBox.Text);

                this.theMainForm.BuildSerialMessage((int)PACKET.CMD_SET_FTC_SETUP);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Table contains invalid entries",
                                "Exception Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
            }

        }

        private void button8_Click(object sender, EventArgs e)
        {
            try
            {
                this.theMainForm.Counter = Convert.ToUInt16(counterUpDown.Text);

                this.theMainForm.BuildSerialMessage((int)PACKET.CMD_GET_FTC_LOG);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Table contains invalid entries",
                                "Exception Error",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error
                               );
            }

        }
    }
}
