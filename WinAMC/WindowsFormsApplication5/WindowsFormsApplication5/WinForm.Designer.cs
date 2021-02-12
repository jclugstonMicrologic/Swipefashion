namespace WindowsFormsApplication5
{
    partial class TheMainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle7 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle8 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle9 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button2 = new System.Windows.Forms.Button();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.GetPressBtn = new System.Windows.Forms.Button();
            this.StartPreviewBtn = new System.Windows.Forms.Button();
            this.CaptureBtn = new System.Windows.Forms.Button();
            this.StopCaptureBtn = new System.Windows.Forms.Button();
            this.UploadFilesBtn = new System.Windows.Forms.Button();
            this.OpCloseBtn = new System.Windows.Forms.Button();
            this.OpLoadBtn = new System.Windows.Forms.Button();
            this.OpLooseFitBtn = new System.Windows.Forms.Button();
            this.OpNormalFitBtn = new System.Windows.Forms.Button();
            this.OpTightFitBtn = new System.Windows.Forms.Button();
            this.BleDiscoverBtn = new System.Windows.Forms.Button();
            this.FwdLimitInd = new System.Windows.Forms.Button();
            this.RevLimitInd = new System.Windows.Forms.Button();
            this.DirFwdInd = new System.Windows.Forms.Button();
            this.DirRevInd = new System.Windows.Forms.Button();
            this.StartFitBtn1 = new System.Windows.Forms.Button();
            this.StartFitBtn2 = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.button8 = new System.Windows.Forms.Button();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.OpenSerialPortBtn = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.diagnosticToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.diagnosticToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.logToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.motorDiagnosticToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fPCParametersToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.counterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.downloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fPCToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.MainPanel = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this.OperationPanel = new System.Windows.Forms.Panel();
            this.CameraPortLbl = new System.Windows.Forms.Label();
            this.CameraStatusLbl = new System.Windows.Forms.Label();
            this.MotorGroupBox = new System.Windows.Forms.GroupBox();
            this.MotorStatusLbl = new System.Windows.Forms.Label();
            this.FwdBtn = new System.Windows.Forms.Button();
            this.RevBtn = new System.Windows.Forms.Button();
            this.StopBtn = new System.Windows.Forms.Button();
            this.CameraGroupBox = new System.Windows.Forms.GroupBox();
            this.CameraOperationLbl = new System.Windows.Forms.Label();
            this.FitGroupBox = new System.Windows.Forms.GroupBox();
            this.label26 = new System.Windows.Forms.Label();
            this.FitTypeLoadComboBox = new System.Windows.Forms.ComboBox();
            this.SnLoadTextBox = new System.Windows.Forms.TextBox();
            this.MotorDetectLbl = new System.Windows.Forms.Label();
            this.ControlPanel = new System.Windows.Forms.Panel();
            this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
            this.SpeedLbl = new System.Windows.Forms.Label();
            this.SpeedTextBox = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.LimitLbl = new System.Windows.Forms.Label();
            this.CurrentLbl = new System.Windows.Forms.Label();
            this.VoltageLbl = new System.Windows.Forms.Label();
            this.VoltageTextBox = new System.Windows.Forms.TextBox();
            this.CurrentTextBox = new System.Windows.Forms.TextBox();
            this.MotorSpeedUpDown = new System.Windows.Forms.NumericUpDown();
            this.label10 = new System.Windows.Forms.Label();
            this.CloseControlPanelBtn = new System.Windows.Forms.Button();
            this.MotorStopBtn = new System.Windows.Forms.Button();
            this.MotorRevBtn = new System.Windows.Forms.Button();
            this.MotorFwdBtn = new System.Windows.Forms.Button();
            this.ProfileGroupBox = new System.Windows.Forms.GroupBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.FitTypeComboBox = new System.Windows.Forms.ComboBox();
            this.label25 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.label27 = new System.Windows.Forms.Label();
            this.label28 = new System.Windows.Forms.Label();
            this.label30 = new System.Windows.Forms.Label();
            this.label31 = new System.Windows.Forms.Label();
            this.LeftLegUpDown = new System.Windows.Forms.NumericUpDown();
            this.LeftButtockUpDown = new System.Windows.Forms.NumericUpDown();
            this.LeftHipUpDown = new System.Windows.Forms.NumericUpDown();
            this.LeftCalfUpDown = new System.Windows.Forms.NumericUpDown();
            this.LeftBreastUpDown = new System.Windows.Forms.NumericUpDown();
            this.LeftArmUpDown = new System.Windows.Forms.NumericUpDown();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label18 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label29 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.RightLegUpDown = new System.Windows.Forms.NumericUpDown();
            this.RightButtockUpDown = new System.Windows.Forms.NumericUpDown();
            this.RightCalfUpDown = new System.Windows.Forms.NumericUpDown();
            this.RightHipUpDown = new System.Windows.Forms.NumericUpDown();
            this.BellyUpDown = new System.Windows.Forms.NumericUpDown();
            this.RightBreastUpDown = new System.Windows.Forms.NumericUpDown();
            this.RightArmUpDown = new System.Windows.Forms.NumericUpDown();
            this.TorsoUpDown = new System.Windows.Forms.NumericUpDown();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.DateTextBox = new System.Windows.Forms.TextBox();
            this.SizeTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ModelTextBox = new System.Windows.Forms.TextBox();
            this.SnTextBox = new System.Windows.Forms.TextBox();
            this.NameTextBox = new System.Windows.Forms.TextBox();
            this.CloseProfileBtn = new System.Windows.Forms.Button();
            this.LoadProfileBtn = new System.Windows.Forms.Button();
            this.ProfileGridView = new System.Windows.Forms.DataGridView();
            this.SaveProfileBtn = new System.Windows.Forms.Button();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.Controller2GridView = new System.Windows.Forms.DataGridView();
            this.BleGroupBox = new System.Windows.Forms.GroupBox();
            this.BleMgsTextClear = new System.Windows.Forms.Button();
            this.BleMsgTextBox = new System.Windows.Forms.RichTextBox();
            this.RxTextBox = new System.Windows.Forms.TextBox();
            this.BleDisconnectBtn = new System.Windows.Forms.Button();
            this.BleRssiBtn = new System.Windows.Forms.Button();
            this.BleSendBtn = new System.Windows.Forms.Button();
            this.edCharVal = new System.Windows.Forms.TextBox();
            this.lvDevices = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.BleConnectBtn = new System.Windows.Forms.Button();
            this.ControllerGridView = new System.Windows.Forms.DataGridView();
            this.BrdTypeLbl = new System.Windows.Forms.Label();
            this.CommSelectPnl = new System.Windows.Forms.Panel();
            this.label22 = new System.Windows.Forms.Label();
            this.button14 = new System.Windows.Forms.Button();
            this.button13 = new System.Windows.Forms.Button();
            this.CommportComboBox = new System.Windows.Forms.ComboBox();
            this.FwBrd1VersionLbl = new System.Windows.Forms.Label();
            this.TestBtn = new System.Windows.Forms.Button();
            this.FwBrd2VersionLbl = new System.Windows.Forms.Label();
            this.Brd2TypeLbl = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.radioButton3 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.V1RadioBtn = new System.Windows.Forms.RadioButton();
            this.button20 = new System.Windows.Forms.Button();
            this.button19 = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label11 = new System.Windows.Forms.Label();
            this.SenThresTextBox = new System.Windows.Forms.TextBox();
            this.TemperatureTextBox = new System.Windows.Forms.TextBox();
            this.PressureTextBox = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.SerialMonitorTimer = new System.Windows.Forms.Timer(this.components);
            this.MotorCntrlTimer = new System.Windows.Forms.Timer(this.components);
            this.FtcStatusStrip = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.FileSystemWatcher = new System.IO.FileSystemWatcher();
            this.FileMonitorTimer = new System.Windows.Forms.Timer(this.components);
            this.CompressorCntrlGridView = new System.Windows.Forms.DataGridView();
            this.FwBrd3VersionLbl = new System.Windows.Forms.Label();
            this.Brd3TypeLbl = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.MainPanel.SuspendLayout();
            this.OperationPanel.SuspendLayout();
            this.MotorGroupBox.SuspendLayout();
            this.CameraGroupBox.SuspendLayout();
            this.FitGroupBox.SuspendLayout();
            this.ControlPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.MotorSpeedUpDown)).BeginInit();
            this.ProfileGroupBox.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.LeftLegUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftButtockUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftHipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftCalfUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftBreastUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftArmUpDown)).BeginInit();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RightLegUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightButtockUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightCalfUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightHipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BellyUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightBreastUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightArmUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.TorsoUpDown)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ProfileGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Controller2GridView)).BeginInit();
            this.BleGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ControllerGridView)).BeginInit();
            this.CommSelectPnl.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.FtcStatusStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.FileSystemWatcher)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CompressorCntrlGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(560, 53);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 3;
            this.button2.Text = "Link DLL";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Visible = false;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // GetPressBtn
            // 
            this.GetPressBtn.Location = new System.Drawing.Point(326, 121);
            this.GetPressBtn.Name = "GetPressBtn";
            this.GetPressBtn.Size = new System.Drawing.Size(82, 23);
            this.GetPressBtn.TabIndex = 49;
            this.GetPressBtn.Text = "Get Press";
            this.toolTip1.SetToolTip(this.GetPressBtn, "Get Setup Information from FTC");
            this.GetPressBtn.UseVisualStyleBackColor = true;
            this.GetPressBtn.Visible = false;
            this.GetPressBtn.Click += new System.EventHandler(this.GetPressBtn_Click);
            // 
            // StartPreviewBtn
            // 
            this.StartPreviewBtn.Location = new System.Drawing.Point(10, 25);
            this.StartPreviewBtn.Name = "StartPreviewBtn";
            this.StartPreviewBtn.Size = new System.Drawing.Size(82, 23);
            this.StartPreviewBtn.TabIndex = 59;
            this.StartPreviewBtn.Text = "Start Preview";
            this.toolTip1.SetToolTip(this.StartPreviewBtn, "Start camera preveiw");
            this.StartPreviewBtn.UseVisualStyleBackColor = true;
            this.StartPreviewBtn.Click += new System.EventHandler(this.StartPreviewBtn_Click);
            // 
            // CaptureBtn
            // 
            this.CaptureBtn.BackColor = System.Drawing.Color.LightGreen;
            this.CaptureBtn.Enabled = false;
            this.CaptureBtn.Location = new System.Drawing.Point(105, 25);
            this.CaptureBtn.Name = "CaptureBtn";
            this.CaptureBtn.Size = new System.Drawing.Size(82, 23);
            this.CaptureBtn.TabIndex = 60;
            this.CaptureBtn.Text = "Capture";
            this.toolTip1.SetToolTip(this.CaptureBtn, "Capture camera data");
            this.CaptureBtn.UseVisualStyleBackColor = false;
            this.CaptureBtn.Click += new System.EventHandler(this.CaptureBtn_Click);
            // 
            // StopCaptureBtn
            // 
            this.StopCaptureBtn.Location = new System.Drawing.Point(195, 25);
            this.StopCaptureBtn.Name = "StopCaptureBtn";
            this.StopCaptureBtn.Size = new System.Drawing.Size(82, 23);
            this.StopCaptureBtn.TabIndex = 61;
            this.StopCaptureBtn.Text = "Stop";
            this.toolTip1.SetToolTip(this.StopCaptureBtn, "Stop camera");
            this.StopCaptureBtn.UseVisualStyleBackColor = true;
            this.StopCaptureBtn.Click += new System.EventHandler(this.StopCaptureBtn_Click);
            // 
            // UploadFilesBtn
            // 
            this.UploadFilesBtn.BackColor = System.Drawing.Color.SkyBlue;
            this.UploadFilesBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.UploadFilesBtn.Location = new System.Drawing.Point(129, 315);
            this.UploadFilesBtn.Name = "UploadFilesBtn";
            this.UploadFilesBtn.Size = new System.Drawing.Size(82, 23);
            this.UploadFilesBtn.TabIndex = 65;
            this.UploadFilesBtn.Text = "Upload";
            this.toolTip1.SetToolTip(this.UploadFilesBtn, "Upload video files to server");
            this.UploadFilesBtn.UseVisualStyleBackColor = false;
            this.UploadFilesBtn.Click += new System.EventHandler(this.UploadFilesBtn_Click);
            // 
            // OpCloseBtn
            // 
            this.OpCloseBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OpCloseBtn.Location = new System.Drawing.Point(32, 315);
            this.OpCloseBtn.Name = "OpCloseBtn";
            this.OpCloseBtn.Size = new System.Drawing.Size(82, 23);
            this.OpCloseBtn.TabIndex = 66;
            this.OpCloseBtn.Text = "Close";
            this.toolTip1.SetToolTip(this.OpCloseBtn, "Close screen");
            this.OpCloseBtn.UseVisualStyleBackColor = true;
            this.OpCloseBtn.Click += new System.EventHandler(this.button3_Click_1);
            // 
            // OpLoadBtn
            // 
            this.OpLoadBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OpLoadBtn.Location = new System.Drawing.Point(159, 78);
            this.OpLoadBtn.Name = "OpLoadBtn";
            this.OpLoadBtn.Size = new System.Drawing.Size(82, 23);
            this.OpLoadBtn.TabIndex = 70;
            this.OpLoadBtn.Text = "Load";
            this.toolTip1.SetToolTip(this.OpLoadBtn, "Stop camera");
            this.OpLoadBtn.UseVisualStyleBackColor = true;
            this.OpLoadBtn.Click += new System.EventHandler(this.OpLoadBtn_Click);
            // 
            // OpLooseFitBtn
            // 
            this.OpLooseFitBtn.Enabled = false;
            this.OpLooseFitBtn.Location = new System.Drawing.Point(10, 25);
            this.OpLooseFitBtn.Name = "OpLooseFitBtn";
            this.OpLooseFitBtn.Size = new System.Drawing.Size(82, 23);
            this.OpLooseFitBtn.TabIndex = 73;
            this.OpLooseFitBtn.Text = "Loose";
            this.toolTip1.SetToolTip(this.OpLooseFitBtn, "Stop camera");
            this.OpLooseFitBtn.UseVisualStyleBackColor = true;
            this.OpLooseFitBtn.Click += new System.EventHandler(this.OpLooseFitBtn_Click);
            // 
            // OpNormalFitBtn
            // 
            this.OpNormalFitBtn.Enabled = false;
            this.OpNormalFitBtn.Location = new System.Drawing.Point(105, 25);
            this.OpNormalFitBtn.Name = "OpNormalFitBtn";
            this.OpNormalFitBtn.Size = new System.Drawing.Size(82, 23);
            this.OpNormalFitBtn.TabIndex = 74;
            this.OpNormalFitBtn.Text = "Normal";
            this.toolTip1.SetToolTip(this.OpNormalFitBtn, "Stop camera");
            this.OpNormalFitBtn.UseVisualStyleBackColor = true;
            this.OpNormalFitBtn.Click += new System.EventHandler(this.OpNormalFitBtn_Click);
            // 
            // OpTightFitBtn
            // 
            this.OpTightFitBtn.Enabled = false;
            this.OpTightFitBtn.Location = new System.Drawing.Point(195, 25);
            this.OpTightFitBtn.Name = "OpTightFitBtn";
            this.OpTightFitBtn.Size = new System.Drawing.Size(82, 23);
            this.OpTightFitBtn.TabIndex = 75;
            this.OpTightFitBtn.Text = "Tight";
            this.toolTip1.SetToolTip(this.OpTightFitBtn, "Stop camera");
            this.OpTightFitBtn.UseVisualStyleBackColor = true;
            this.OpTightFitBtn.Click += new System.EventHandler(this.OpTightFitBtn_Click);
            // 
            // BleDiscoverBtn
            // 
            this.BleDiscoverBtn.BackColor = System.Drawing.Color.SkyBlue;
            this.BleDiscoverBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BleDiscoverBtn.Location = new System.Drawing.Point(6, 23);
            this.BleDiscoverBtn.Name = "BleDiscoverBtn";
            this.BleDiscoverBtn.Size = new System.Drawing.Size(130, 25);
            this.BleDiscoverBtn.TabIndex = 51;
            this.BleDiscoverBtn.Text = "Discover";
            this.toolTip1.SetToolTip(this.BleDiscoverBtn, "Search for bluetooth devices");
            this.BleDiscoverBtn.UseVisualStyleBackColor = false;
            this.BleDiscoverBtn.Click += new System.EventHandler(this.button1_Click);
            // 
            // FwdLimitInd
            // 
            this.FwdLimitInd.BackColor = System.Drawing.Color.LightGray;
            this.FwdLimitInd.Location = new System.Drawing.Point(130, 210);
            this.FwdLimitInd.Name = "FwdLimitInd";
            this.FwdLimitInd.Size = new System.Drawing.Size(20, 20);
            this.FwdLimitInd.TabIndex = 79;
            this.FwdLimitInd.UseVisualStyleBackColor = false;
            // 
            // RevLimitInd
            // 
            this.RevLimitInd.BackColor = System.Drawing.Color.LightGray;
            this.RevLimitInd.Location = new System.Drawing.Point(160, 210);
            this.RevLimitInd.Name = "RevLimitInd";
            this.RevLimitInd.Size = new System.Drawing.Size(20, 20);
            this.RevLimitInd.TabIndex = 80;
            this.RevLimitInd.UseVisualStyleBackColor = false;
            // 
            // DirFwdInd
            // 
            this.DirFwdInd.BackColor = System.Drawing.Color.LightGray;
            this.DirFwdInd.Location = new System.Drawing.Point(130, 180);
            this.DirFwdInd.Name = "DirFwdInd";
            this.DirFwdInd.Size = new System.Drawing.Size(20, 20);
            this.DirFwdInd.TabIndex = 84;
            this.DirFwdInd.UseVisualStyleBackColor = false;
            // 
            // DirRevInd
            // 
            this.DirRevInd.BackColor = System.Drawing.Color.LightGray;
            this.DirRevInd.Location = new System.Drawing.Point(160, 180);
            this.DirRevInd.Name = "DirRevInd";
            this.DirRevInd.Size = new System.Drawing.Size(20, 20);
            this.DirRevInd.TabIndex = 85;
            this.DirRevInd.UseVisualStyleBackColor = false;
            this.DirRevInd.Visible = false;
            // 
            // StartFitBtn1
            // 
            this.StartFitBtn1.Location = new System.Drawing.Point(326, 43);
            this.StartFitBtn1.Name = "StartFitBtn1";
            this.StartFitBtn1.Size = new System.Drawing.Size(82, 23);
            this.StartFitBtn1.TabIndex = 50;
            this.StartFitBtn1.Text = "Start Fit";
            this.StartFitBtn1.UseVisualStyleBackColor = true;
            this.StartFitBtn1.Click += new System.EventHandler(this.StartFitBtn_Click);
            // 
            // StartFitBtn2
            // 
            this.StartFitBtn2.Location = new System.Drawing.Point(268, 32);
            this.StartFitBtn2.Name = "StartFitBtn2";
            this.StartFitBtn2.Size = new System.Drawing.Size(82, 23);
            this.StartFitBtn2.TabIndex = 54;
            this.StartFitBtn2.Text = "Start Fit";
            this.StartFitBtn2.UseVisualStyleBackColor = true;
            this.StartFitBtn2.Click += new System.EventHandler(this.StartFitBtn2_Click);
            // 
            // timer1
            // 
            this.timer1.Interval = 500;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // button8
            // 
            this.button8.Location = new System.Drawing.Point(641, 51);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(75, 23);
            this.button8.TabIndex = 13;
            this.button8.Text = "New Email";
            this.button8.UseVisualStyleBackColor = true;
            this.button8.Visible = false;
            // 
            // OpenSerialPortBtn
            // 
            this.OpenSerialPortBtn.Location = new System.Drawing.Point(1153, 239);
            this.OpenSerialPortBtn.Name = "OpenSerialPortBtn";
            this.OpenSerialPortBtn.Size = new System.Drawing.Size(75, 23);
            this.OpenSerialPortBtn.TabIndex = 15;
            this.OpenSerialPortBtn.Text = "Open Port";
            this.OpenSerialPortBtn.UseVisualStyleBackColor = true;
            this.OpenSerialPortBtn.Visible = false;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.diagnosticToolStripMenuItem,
            this.setToolStripMenuItem,
            this.downloadToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1248, 24);
            this.menuStrip1.TabIndex = 21;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.connectToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Enabled = false;
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.openToolStripMenuItem.Text = "Open";
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.connectToolStripMenuItem.Text = "Connect";
            this.connectToolStripMenuItem.Click += new System.EventHandler(this.connectToolStripMenuItem_Click);
            // 
            // diagnosticToolStripMenuItem
            // 
            this.diagnosticToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.setupToolStripMenuItem,
            this.diagnosticToolStripMenuItem1,
            this.logToolStripMenuItem,
            this.motorDiagnosticToolStripMenuItem});
            this.diagnosticToolStripMenuItem.Name = "diagnosticToolStripMenuItem";
            this.diagnosticToolStripMenuItem.Size = new System.Drawing.Size(63, 20);
            this.diagnosticToolStripMenuItem.Text = "&Window";
            // 
            // setupToolStripMenuItem
            // 
            this.setupToolStripMenuItem.Name = "setupToolStripMenuItem";
            this.setupToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
            this.setupToolStripMenuItem.Text = "Open Profile";
            this.setupToolStripMenuItem.Click += new System.EventHandler(this.setupToolStripMenuItem_Click);
            // 
            // diagnosticToolStripMenuItem1
            // 
            this.diagnosticToolStripMenuItem1.Name = "diagnosticToolStripMenuItem1";
            this.diagnosticToolStripMenuItem1.Size = new System.Drawing.Size(166, 22);
            this.diagnosticToolStripMenuItem1.Text = "Diagnostic";
            this.diagnosticToolStripMenuItem1.Visible = false;
            this.diagnosticToolStripMenuItem1.Click += new System.EventHandler(this.diagnosticToolStripMenuItem1_Click);
            // 
            // logToolStripMenuItem
            // 
            this.logToolStripMenuItem.Name = "logToolStripMenuItem";
            this.logToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
            this.logToolStripMenuItem.Text = "Log";
            this.logToolStripMenuItem.Visible = false;
            // 
            // motorDiagnosticToolStripMenuItem
            // 
            this.motorDiagnosticToolStripMenuItem.Name = "motorDiagnosticToolStripMenuItem";
            this.motorDiagnosticToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
            this.motorDiagnosticToolStripMenuItem.Text = "Motor Diagnostic";
            this.motorDiagnosticToolStripMenuItem.Click += new System.EventHandler(this.motorDiagnosticToolStripMenuItem_Click);
            // 
            // setToolStripMenuItem
            // 
            this.setToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fPCParametersToolStripMenuItem,
            this.counterToolStripMenuItem});
            this.setToolStripMenuItem.Name = "setToolStripMenuItem";
            this.setToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.setToolStripMenuItem.Text = "&Set";
            this.setToolStripMenuItem.Visible = false;
            // 
            // fPCParametersToolStripMenuItem
            // 
            this.fPCParametersToolStripMenuItem.Name = "fPCParametersToolStripMenuItem";
            this.fPCParametersToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.fPCParametersToolStripMenuItem.Text = "FTC Parameters";
            this.fPCParametersToolStripMenuItem.Click += new System.EventHandler(this.fPCParametersToolStripMenuItem_Click);
            // 
            // counterToolStripMenuItem
            // 
            this.counterToolStripMenuItem.Name = "counterToolStripMenuItem";
            this.counterToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.counterToolStripMenuItem.Text = "Counter";
            this.counterToolStripMenuItem.Click += new System.EventHandler(this.counterToolStripMenuItem_Click);
            // 
            // downloadToolStripMenuItem
            // 
            this.downloadToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fPCToolStripMenuItem});
            this.downloadToolStripMenuItem.Name = "downloadToolStripMenuItem";
            this.downloadToolStripMenuItem.Size = new System.Drawing.Size(73, 20);
            this.downloadToolStripMenuItem.Text = "&Download";
            this.downloadToolStripMenuItem.Visible = false;
            this.downloadToolStripMenuItem.Click += new System.EventHandler(this.downloadToolStripMenuItem_Click);
            // 
            // fPCToolStripMenuItem
            // 
            this.fPCToolStripMenuItem.Name = "fPCToolStripMenuItem";
            this.fPCToolStripMenuItem.Size = new System.Drawing.Size(95, 22);
            this.fPCToolStripMenuItem.Text = "FTC";
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "&Help";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // MainPanel
            // 
            this.MainPanel.BackColor = System.Drawing.Color.WhiteSmoke;
            this.MainPanel.Controls.Add(this.Brd3TypeLbl);
            this.MainPanel.Controls.Add(this.FwBrd3VersionLbl);
            this.MainPanel.Controls.Add(this.CompressorCntrlGridView);
            this.MainPanel.Controls.Add(this.button1);
            this.MainPanel.Controls.Add(this.OperationPanel);
            this.MainPanel.Controls.Add(this.ControlPanel);
            this.MainPanel.Controls.Add(this.ProfileGroupBox);
            this.MainPanel.Controls.Add(this.StartFitBtn2);
            this.MainPanel.Controls.Add(this.Controller2GridView);
            this.MainPanel.Controls.Add(this.BleGroupBox);
            this.MainPanel.Controls.Add(this.StartFitBtn1);
            this.MainPanel.Controls.Add(this.GetPressBtn);
            this.MainPanel.Controls.Add(this.ControllerGridView);
            this.MainPanel.Controls.Add(this.BrdTypeLbl);
            this.MainPanel.Controls.Add(this.CommSelectPnl);
            this.MainPanel.Controls.Add(this.FwBrd1VersionLbl);
            this.MainPanel.Controls.Add(this.TestBtn);
            this.MainPanel.Controls.Add(this.FwBrd2VersionLbl);
            this.MainPanel.Controls.Add(this.Brd2TypeLbl);
            this.MainPanel.Location = new System.Drawing.Point(25, 40);
            this.MainPanel.Name = "MainPanel";
            this.MainPanel.Size = new System.Drawing.Size(1086, 650);
            this.MainPanel.TabIndex = 22;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(333, 11);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 64;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Visible = false;
            this.button1.Click += new System.EventHandler(this.button1_Click_3);
            // 
            // OperationPanel
            // 
            this.OperationPanel.BackColor = System.Drawing.Color.White;
            this.OperationPanel.Controls.Add(this.CameraPortLbl);
            this.OperationPanel.Controls.Add(this.CameraStatusLbl);
            this.OperationPanel.Controls.Add(this.MotorGroupBox);
            this.OperationPanel.Controls.Add(this.CameraGroupBox);
            this.OperationPanel.Controls.Add(this.FitGroupBox);
            this.OperationPanel.Controls.Add(this.label26);
            this.OperationPanel.Controls.Add(this.FitTypeLoadComboBox);
            this.OperationPanel.Controls.Add(this.OpLoadBtn);
            this.OperationPanel.Controls.Add(this.SnLoadTextBox);
            this.OperationPanel.Controls.Add(this.MotorDetectLbl);
            this.OperationPanel.Controls.Add(this.OpCloseBtn);
            this.OperationPanel.Controls.Add(this.UploadFilesBtn);
            this.OperationPanel.Location = new System.Drawing.Point(807, 217);
            this.OperationPanel.Name = "OperationPanel";
            this.OperationPanel.Size = new System.Drawing.Size(246, 341);
            this.OperationPanel.TabIndex = 62;
            this.OperationPanel.Visible = false;
            // 
            // CameraPortLbl
            // 
            this.CameraPortLbl.AutoSize = true;
            this.CameraPortLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CameraPortLbl.Location = new System.Drawing.Point(220, 40);
            this.CameraPortLbl.Name = "CameraPortLbl";
            this.CameraPortLbl.Size = new System.Drawing.Size(32, 16);
            this.CameraPortLbl.TabIndex = 82;
            this.CameraPortLbl.Text = "Port";
            // 
            // CameraStatusLbl
            // 
            this.CameraStatusLbl.AutoSize = true;
            this.CameraStatusLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CameraStatusLbl.Location = new System.Drawing.Point(15, 40);
            this.CameraStatusLbl.Name = "CameraStatusLbl";
            this.CameraStatusLbl.Size = new System.Drawing.Size(56, 16);
            this.CameraStatusLbl.TabIndex = 81;
            this.CameraStatusLbl.Text = "Camera";
            // 
            // MotorGroupBox
            // 
            this.MotorGroupBox.Controls.Add(this.MotorStatusLbl);
            this.MotorGroupBox.Controls.Add(this.FwdBtn);
            this.MotorGroupBox.Controls.Add(this.RevBtn);
            this.MotorGroupBox.Controls.Add(this.StopBtn);
            this.MotorGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MotorGroupBox.Location = new System.Drawing.Point(15, 244);
            this.MotorGroupBox.Name = "MotorGroupBox";
            this.MotorGroupBox.Size = new System.Drawing.Size(285, 80);
            this.MotorGroupBox.TabIndex = 80;
            this.MotorGroupBox.TabStop = false;
            this.MotorGroupBox.Text = "Motor Control";
            // 
            // MotorStatusLbl
            // 
            this.MotorStatusLbl.AutoSize = true;
            this.MotorStatusLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MotorStatusLbl.Location = new System.Drawing.Point(13, 55);
            this.MotorStatusLbl.Name = "MotorStatusLbl";
            this.MotorStatusLbl.Size = new System.Drawing.Size(43, 16);
            this.MotorStatusLbl.TabIndex = 71;
            this.MotorStatusLbl.Text = "status";
            // 
            // FwdBtn
            // 
            this.FwdBtn.Location = new System.Drawing.Point(10, 25);
            this.FwdBtn.Name = "FwdBtn";
            this.FwdBtn.Size = new System.Drawing.Size(82, 23);
            this.FwdBtn.TabIndex = 62;
            this.FwdBtn.Text = "FWD";
            this.FwdBtn.UseVisualStyleBackColor = true;
            this.FwdBtn.Click += new System.EventHandler(this.button3_Click);
            // 
            // RevBtn
            // 
            this.RevBtn.Location = new System.Drawing.Point(105, 25);
            this.RevBtn.Name = "RevBtn";
            this.RevBtn.Size = new System.Drawing.Size(82, 23);
            this.RevBtn.TabIndex = 63;
            this.RevBtn.Text = "REV";
            this.RevBtn.UseVisualStyleBackColor = true;
            this.RevBtn.Click += new System.EventHandler(this.RevBtn_Click);
            // 
            // StopBtn
            // 
            this.StopBtn.Location = new System.Drawing.Point(195, 25);
            this.StopBtn.Name = "StopBtn";
            this.StopBtn.Size = new System.Drawing.Size(82, 23);
            this.StopBtn.TabIndex = 64;
            this.StopBtn.Text = "STOP";
            this.StopBtn.UseVisualStyleBackColor = true;
            this.StopBtn.Click += new System.EventHandler(this.StopBtn_Click);
            // 
            // CameraGroupBox
            // 
            this.CameraGroupBox.Controls.Add(this.CameraOperationLbl);
            this.CameraGroupBox.Controls.Add(this.StartPreviewBtn);
            this.CameraGroupBox.Controls.Add(this.StopCaptureBtn);
            this.CameraGroupBox.Controls.Add(this.CaptureBtn);
            this.CameraGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CameraGroupBox.Location = new System.Drawing.Point(15, 163);
            this.CameraGroupBox.Name = "CameraGroupBox";
            this.CameraGroupBox.Size = new System.Drawing.Size(285, 80);
            this.CameraGroupBox.TabIndex = 79;
            this.CameraGroupBox.TabStop = false;
            this.CameraGroupBox.Text = "Camera Control";
            // 
            // CameraOperationLbl
            // 
            this.CameraOperationLbl.AutoSize = true;
            this.CameraOperationLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CameraOperationLbl.Location = new System.Drawing.Point(13, 55);
            this.CameraOperationLbl.Name = "CameraOperationLbl";
            this.CameraOperationLbl.Size = new System.Drawing.Size(43, 16);
            this.CameraOperationLbl.TabIndex = 70;
            this.CameraOperationLbl.Text = "status";
            // 
            // FitGroupBox
            // 
            this.FitGroupBox.Controls.Add(this.OpLooseFitBtn);
            this.FitGroupBox.Controls.Add(this.OpNormalFitBtn);
            this.FitGroupBox.Controls.Add(this.OpTightFitBtn);
            this.FitGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FitGroupBox.Location = new System.Drawing.Point(15, 100);
            this.FitGroupBox.Name = "FitGroupBox";
            this.FitGroupBox.Size = new System.Drawing.Size(285, 80);
            this.FitGroupBox.TabIndex = 77;
            this.FitGroupBox.TabStop = false;
            this.FitGroupBox.Text = "FIT";
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label26.Location = new System.Drawing.Point(15, 80);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(34, 16);
            this.label26.TabIndex = 76;
            this.label26.Text = "SN#";
            // 
            // FitTypeLoadComboBox
            // 
            this.FitTypeLoadComboBox.FormattingEnabled = true;
            this.FitTypeLoadComboBox.Items.AddRange(new object[] {
            "LOOSE",
            "NORMAL",
            "TIGHT"});
            this.FitTypeLoadComboBox.Location = new System.Drawing.Point(183, 11);
            this.FitTypeLoadComboBox.Name = "FitTypeLoadComboBox";
            this.FitTypeLoadComboBox.Size = new System.Drawing.Size(80, 21);
            this.FitTypeLoadComboBox.TabIndex = 72;
            this.FitTypeLoadComboBox.Visible = false;
            this.FitTypeLoadComboBox.SelectedIndexChanged += new System.EventHandler(this.FitTypeLoadComboBox_SelectedIndexChanged);
            this.FitTypeLoadComboBox.SelectionChangeCommitted += new System.EventHandler(this.FitTypeLoadComboBox_SelectionChangeCommitted);
            // 
            // SnLoadTextBox
            // 
            this.SnLoadTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SnLoadTextBox.Location = new System.Drawing.Point(54, 79);
            this.SnLoadTextBox.Name = "SnLoadTextBox";
            this.SnLoadTextBox.Size = new System.Drawing.Size(100, 22);
            this.SnLoadTextBox.TabIndex = 71;
            // 
            // MotorDetectLbl
            // 
            this.MotorDetectLbl.AutoSize = true;
            this.MotorDetectLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MotorDetectLbl.Location = new System.Drawing.Point(15, 15);
            this.MotorDetectLbl.Name = "MotorDetectLbl";
            this.MotorDetectLbl.Size = new System.Drawing.Size(42, 16);
            this.MotorDetectLbl.TabIndex = 69;
            this.MotorDetectLbl.Text = "Motor";
            // 
            // ControlPanel
            // 
            this.ControlPanel.BackColor = System.Drawing.Color.White;
            this.ControlPanel.Controls.Add(this.numericUpDown1);
            this.ControlPanel.Controls.Add(this.DirRevInd);
            this.ControlPanel.Controls.Add(this.DirFwdInd);
            this.ControlPanel.Controls.Add(this.SpeedLbl);
            this.ControlPanel.Controls.Add(this.SpeedTextBox);
            this.ControlPanel.Controls.Add(this.label19);
            this.ControlPanel.Controls.Add(this.RevLimitInd);
            this.ControlPanel.Controls.Add(this.FwdLimitInd);
            this.ControlPanel.Controls.Add(this.LimitLbl);
            this.ControlPanel.Controls.Add(this.CurrentLbl);
            this.ControlPanel.Controls.Add(this.VoltageLbl);
            this.ControlPanel.Controls.Add(this.VoltageTextBox);
            this.ControlPanel.Controls.Add(this.CurrentTextBox);
            this.ControlPanel.Controls.Add(this.MotorSpeedUpDown);
            this.ControlPanel.Controls.Add(this.label10);
            this.ControlPanel.Controls.Add(this.CloseControlPanelBtn);
            this.ControlPanel.Controls.Add(this.MotorStopBtn);
            this.ControlPanel.Controls.Add(this.MotorRevBtn);
            this.ControlPanel.Controls.Add(this.MotorFwdBtn);
            this.ControlPanel.Location = new System.Drawing.Point(822, 32);
            this.ControlPanel.Name = "ControlPanel";
            this.ControlPanel.Size = new System.Drawing.Size(264, 232);
            this.ControlPanel.TabIndex = 61;
            this.ControlPanel.Visible = false;
            // 
            // numericUpDown1
            // 
            this.numericUpDown1.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.numericUpDown1.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDown1.Location = new System.Drawing.Point(200, 60);
            this.numericUpDown1.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new System.Drawing.Size(60, 21);
            this.numericUpDown1.TabIndex = 86;
            this.numericUpDown1.Value = new decimal(new int[] {
            250,
            0,
            0,
            0});
            // 
            // SpeedLbl
            // 
            this.SpeedLbl.AutoSize = true;
            this.SpeedLbl.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SpeedLbl.Location = new System.Drawing.Point(20, 150);
            this.SpeedLbl.Name = "SpeedLbl";
            this.SpeedLbl.Size = new System.Drawing.Size(80, 16);
            this.SpeedLbl.TabIndex = 83;
            this.SpeedLbl.Text = "Speed: NA";
            // 
            // SpeedTextBox
            // 
            this.SpeedTextBox.Enabled = false;
            this.SpeedTextBox.Location = new System.Drawing.Point(208, 146);
            this.SpeedTextBox.Name = "SpeedTextBox";
            this.SpeedTextBox.Size = new System.Drawing.Size(60, 20);
            this.SpeedTextBox.TabIndex = 82;
            this.SpeedTextBox.Visible = false;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.Location = new System.Drawing.Point(20, 180);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(40, 16);
            this.label19.TabIndex = 81;
            this.label19.Text = "Dir:";
            // 
            // LimitLbl
            // 
            this.LimitLbl.AutoSize = true;
            this.LimitLbl.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LimitLbl.Location = new System.Drawing.Point(20, 210);
            this.LimitLbl.Name = "LimitLbl";
            this.LimitLbl.Size = new System.Drawing.Size(48, 16);
            this.LimitLbl.TabIndex = 78;
            this.LimitLbl.Text = "Limit";
            // 
            // CurrentLbl
            // 
            this.CurrentLbl.AutoSize = true;
            this.CurrentLbl.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CurrentLbl.Location = new System.Drawing.Point(20, 120);
            this.CurrentLbl.Name = "CurrentLbl";
            this.CurrentLbl.Size = new System.Drawing.Size(96, 16);
            this.CurrentLbl.TabIndex = 77;
            this.CurrentLbl.Text = "Current: NA";
            // 
            // VoltageLbl
            // 
            this.VoltageLbl.AutoSize = true;
            this.VoltageLbl.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.VoltageLbl.Location = new System.Drawing.Point(20, 90);
            this.VoltageLbl.Name = "VoltageLbl";
            this.VoltageLbl.Size = new System.Drawing.Size(96, 16);
            this.VoltageLbl.TabIndex = 76;
            this.VoltageLbl.Text = "Voltage: NA";
            // 
            // VoltageTextBox
            // 
            this.VoltageTextBox.Enabled = false;
            this.VoltageTextBox.Location = new System.Drawing.Point(208, 92);
            this.VoltageTextBox.Name = "VoltageTextBox";
            this.VoltageTextBox.Size = new System.Drawing.Size(60, 20);
            this.VoltageTextBox.TabIndex = 74;
            this.VoltageTextBox.Visible = false;
            // 
            // CurrentTextBox
            // 
            this.CurrentTextBox.Enabled = false;
            this.CurrentTextBox.Location = new System.Drawing.Point(230, 116);
            this.CurrentTextBox.Name = "CurrentTextBox";
            this.CurrentTextBox.Size = new System.Drawing.Size(60, 20);
            this.CurrentTextBox.TabIndex = 73;
            this.CurrentTextBox.Visible = false;
            // 
            // MotorSpeedUpDown
            // 
            this.MotorSpeedUpDown.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MotorSpeedUpDown.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.MotorSpeedUpDown.Location = new System.Drawing.Point(120, 60);
            this.MotorSpeedUpDown.Maximum = new decimal(new int[] {
            3200,
            0,
            0,
            0});
            this.MotorSpeedUpDown.Name = "MotorSpeedUpDown";
            this.MotorSpeedUpDown.Size = new System.Drawing.Size(60, 21);
            this.MotorSpeedUpDown.TabIndex = 72;
            this.MotorSpeedUpDown.Value = new decimal(new int[] {
            1600,
            0,
            0,
            0});
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(20, 60);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(96, 16);
            this.label10.TabIndex = 68;
            this.label10.Text = "Motor Speed";
            // 
            // CloseControlPanelBtn
            // 
            this.CloseControlPanelBtn.Location = new System.Drawing.Point(23, 270);
            this.CloseControlPanelBtn.Name = "CloseControlPanelBtn";
            this.CloseControlPanelBtn.Size = new System.Drawing.Size(75, 23);
            this.CloseControlPanelBtn.TabIndex = 66;
            this.CloseControlPanelBtn.Text = "CLOSE";
            this.CloseControlPanelBtn.UseVisualStyleBackColor = true;
            this.CloseControlPanelBtn.Click += new System.EventHandler(this.CloseControlPanelBtn_Click);
            // 
            // MotorStopBtn
            // 
            this.MotorStopBtn.Location = new System.Drawing.Point(182, 19);
            this.MotorStopBtn.Name = "MotorStopBtn";
            this.MotorStopBtn.Size = new System.Drawing.Size(75, 23);
            this.MotorStopBtn.TabIndex = 63;
            this.MotorStopBtn.Text = "STOP";
            this.MotorStopBtn.UseVisualStyleBackColor = true;
            this.MotorStopBtn.Click += new System.EventHandler(this.MotorStopBtn_Click);
            // 
            // MotorRevBtn
            // 
            this.MotorRevBtn.Location = new System.Drawing.Point(101, 19);
            this.MotorRevBtn.Name = "MotorRevBtn";
            this.MotorRevBtn.Size = new System.Drawing.Size(75, 23);
            this.MotorRevBtn.TabIndex = 62;
            this.MotorRevBtn.Text = "REV";
            this.MotorRevBtn.UseVisualStyleBackColor = true;
            this.MotorRevBtn.Click += new System.EventHandler(this.MotorRevBtn_Click);
            // 
            // MotorFwdBtn
            // 
            this.MotorFwdBtn.Location = new System.Drawing.Point(20, 19);
            this.MotorFwdBtn.Name = "MotorFwdBtn";
            this.MotorFwdBtn.Size = new System.Drawing.Size(75, 23);
            this.MotorFwdBtn.TabIndex = 61;
            this.MotorFwdBtn.Text = "FWD";
            this.MotorFwdBtn.UseVisualStyleBackColor = true;
            this.MotorFwdBtn.Click += new System.EventHandler(this.MotorFwdBtn_Click);
            // 
            // ProfileGroupBox
            // 
            this.ProfileGroupBox.BackColor = System.Drawing.Color.White;
            this.ProfileGroupBox.Controls.Add(this.panel2);
            this.ProfileGroupBox.Controls.Add(this.panel1);
            this.ProfileGroupBox.Controls.Add(this.groupBox3);
            this.ProfileGroupBox.Controls.Add(this.CloseProfileBtn);
            this.ProfileGroupBox.Controls.Add(this.LoadProfileBtn);
            this.ProfileGroupBox.Controls.Add(this.ProfileGridView);
            this.ProfileGroupBox.Controls.Add(this.SaveProfileBtn);
            this.ProfileGroupBox.Controls.Add(this.textBox3);
            this.ProfileGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ProfileGroupBox.Location = new System.Drawing.Point(18, 212);
            this.ProfileGroupBox.Name = "ProfileGroupBox";
            this.ProfileGroupBox.Size = new System.Drawing.Size(512, 520);
            this.ProfileGroupBox.TabIndex = 47;
            this.ProfileGroupBox.TabStop = false;
            this.ProfileGroupBox.Text = "Profile";
            this.ProfileGroupBox.Visible = false;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.FitTypeComboBox);
            this.panel2.Controls.Add(this.label25);
            this.panel2.Controls.Add(this.label21);
            this.panel2.Controls.Add(this.label23);
            this.panel2.Controls.Add(this.label27);
            this.panel2.Controls.Add(this.label28);
            this.panel2.Controls.Add(this.label30);
            this.panel2.Controls.Add(this.label31);
            this.panel2.Controls.Add(this.LeftLegUpDown);
            this.panel2.Controls.Add(this.LeftButtockUpDown);
            this.panel2.Controls.Add(this.LeftHipUpDown);
            this.panel2.Controls.Add(this.LeftCalfUpDown);
            this.panel2.Controls.Add(this.LeftBreastUpDown);
            this.panel2.Controls.Add(this.LeftArmUpDown);
            this.panel2.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panel2.Location = new System.Drawing.Point(230, 201);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(233, 252);
            this.panel2.TabIndex = 73;
            // 
            // FitTypeComboBox
            // 
            this.FitTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FitTypeComboBox.FormattingEnabled = true;
            this.FitTypeComboBox.Items.AddRange(new object[] {
            "LOOSE",
            "NORMAL",
            "TIGHT"});
            this.FitTypeComboBox.Location = new System.Drawing.Point(125, 201);
            this.FitTypeComboBox.Name = "FitTypeComboBox";
            this.FitTypeComboBox.Size = new System.Drawing.Size(88, 26);
            this.FitTypeComboBox.TabIndex = 19;
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label25.Location = new System.Drawing.Point(15, 204);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(60, 18);
            this.label25.TabIndex = 85;
            this.label25.Text = "Fit Type";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label21.Location = new System.Drawing.Point(15, 135);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(60, 18);
            this.label21.TabIndex = 84;
            this.label21.Text = "Left Leg";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label23.Location = new System.Drawing.Point(15, 105);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(87, 18);
            this.label23.TabIndex = 83;
            this.label23.Text = "Left Buttock";
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label27.Location = new System.Drawing.Point(15, 75);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(58, 18);
            this.label27.TabIndex = 82;
            this.label27.Text = "Left Hip";
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label28.Location = new System.Drawing.Point(15, 165);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(62, 18);
            this.label28.TabIndex = 81;
            this.label28.Text = "Left Calf";
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label30.Location = new System.Drawing.Point(15, 45);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(79, 18);
            this.label30.TabIndex = 79;
            this.label30.Text = "Left Breast";
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label31.Location = new System.Drawing.Point(15, 15);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(63, 18);
            this.label31.TabIndex = 78;
            this.label31.Text = "Left Arm";
            // 
            // LeftLegUpDown
            // 
            this.LeftLegUpDown.DecimalPlaces = 1;
            this.LeftLegUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.LeftLegUpDown.Location = new System.Drawing.Point(125, 134);
            this.LeftLegUpDown.Name = "LeftLegUpDown";
            this.LeftLegUpDown.Size = new System.Drawing.Size(79, 24);
            this.LeftLegUpDown.TabIndex = 17;
            // 
            // LeftButtockUpDown
            // 
            this.LeftButtockUpDown.DecimalPlaces = 1;
            this.LeftButtockUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.LeftButtockUpDown.Location = new System.Drawing.Point(125, 104);
            this.LeftButtockUpDown.Name = "LeftButtockUpDown";
            this.LeftButtockUpDown.Size = new System.Drawing.Size(79, 24);
            this.LeftButtockUpDown.TabIndex = 16;
            // 
            // LeftHipUpDown
            // 
            this.LeftHipUpDown.DecimalPlaces = 1;
            this.LeftHipUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.LeftHipUpDown.Location = new System.Drawing.Point(125, 74);
            this.LeftHipUpDown.Name = "LeftHipUpDown";
            this.LeftHipUpDown.Size = new System.Drawing.Size(79, 24);
            this.LeftHipUpDown.TabIndex = 15;
            // 
            // LeftCalfUpDown
            // 
            this.LeftCalfUpDown.DecimalPlaces = 1;
            this.LeftCalfUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.LeftCalfUpDown.Location = new System.Drawing.Point(125, 164);
            this.LeftCalfUpDown.Name = "LeftCalfUpDown";
            this.LeftCalfUpDown.Size = new System.Drawing.Size(79, 24);
            this.LeftCalfUpDown.TabIndex = 18;
            // 
            // LeftBreastUpDown
            // 
            this.LeftBreastUpDown.DecimalPlaces = 1;
            this.LeftBreastUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.LeftBreastUpDown.Location = new System.Drawing.Point(125, 44);
            this.LeftBreastUpDown.Name = "LeftBreastUpDown";
            this.LeftBreastUpDown.Size = new System.Drawing.Size(79, 24);
            this.LeftBreastUpDown.TabIndex = 14;
            // 
            // LeftArmUpDown
            // 
            this.LeftArmUpDown.DecimalPlaces = 1;
            this.LeftArmUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.LeftArmUpDown.Location = new System.Drawing.Point(125, 14);
            this.LeftArmUpDown.Name = "LeftArmUpDown";
            this.LeftArmUpDown.Size = new System.Drawing.Size(79, 24);
            this.LeftArmUpDown.TabIndex = 13;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label18);
            this.panel1.Controls.Add(this.label17);
            this.panel1.Controls.Add(this.label13);
            this.panel1.Controls.Add(this.label12);
            this.panel1.Controls.Add(this.label29);
            this.panel1.Controls.Add(this.label8);
            this.panel1.Controls.Add(this.label7);
            this.panel1.Controls.Add(this.label6);
            this.panel1.Controls.Add(this.RightLegUpDown);
            this.panel1.Controls.Add(this.RightButtockUpDown);
            this.panel1.Controls.Add(this.RightCalfUpDown);
            this.panel1.Controls.Add(this.RightHipUpDown);
            this.panel1.Controls.Add(this.BellyUpDown);
            this.panel1.Controls.Add(this.RightBreastUpDown);
            this.panel1.Controls.Add(this.RightArmUpDown);
            this.panel1.Controls.Add(this.TorsoUpDown);
            this.panel1.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panel1.Location = new System.Drawing.Point(10, 201);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(211, 256);
            this.panel1.TabIndex = 72;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label18.Location = new System.Drawing.Point(15, 195);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(70, 18);
            this.label18.TabIndex = 84;
            this.label18.Text = "Rigth Leg";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(15, 165);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(97, 18);
            this.label17.TabIndex = 83;
            this.label17.Text = "Rigth Buttock";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(15, 135);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(68, 18);
            this.label13.TabIndex = 82;
            this.label13.Text = "Right Hip";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(15, 105);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(39, 18);
            this.label12.TabIndex = 81;
            this.label12.Text = "Belly";
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label29.Location = new System.Drawing.Point(15, 225);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(72, 18);
            this.label29.TabIndex = 80;
            this.label29.Text = "Right Calf";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(15, 75);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(89, 18);
            this.label8.TabIndex = 80;
            this.label8.Text = "Right Breast";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(15, 45);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(73, 18);
            this.label7.TabIndex = 79;
            this.label7.Text = "Rigth Arm";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(15, 15);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(48, 18);
            this.label6.TabIndex = 78;
            this.label6.Text = "Torso";
            // 
            // RightLegUpDown
            // 
            this.RightLegUpDown.DecimalPlaces = 1;
            this.RightLegUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.RightLegUpDown.Location = new System.Drawing.Point(125, 194);
            this.RightLegUpDown.Name = "RightLegUpDown";
            this.RightLegUpDown.Size = new System.Drawing.Size(65, 24);
            this.RightLegUpDown.TabIndex = 11;
            // 
            // RightButtockUpDown
            // 
            this.RightButtockUpDown.DecimalPlaces = 1;
            this.RightButtockUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.RightButtockUpDown.Location = new System.Drawing.Point(125, 164);
            this.RightButtockUpDown.Name = "RightButtockUpDown";
            this.RightButtockUpDown.Size = new System.Drawing.Size(65, 24);
            this.RightButtockUpDown.TabIndex = 10;
            // 
            // RightCalfUpDown
            // 
            this.RightCalfUpDown.DecimalPlaces = 1;
            this.RightCalfUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.RightCalfUpDown.Location = new System.Drawing.Point(125, 224);
            this.RightCalfUpDown.Name = "RightCalfUpDown";
            this.RightCalfUpDown.Size = new System.Drawing.Size(65, 24);
            this.RightCalfUpDown.TabIndex = 12;
            // 
            // RightHipUpDown
            // 
            this.RightHipUpDown.DecimalPlaces = 1;
            this.RightHipUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.RightHipUpDown.Location = new System.Drawing.Point(125, 134);
            this.RightHipUpDown.Name = "RightHipUpDown";
            this.RightHipUpDown.Size = new System.Drawing.Size(65, 24);
            this.RightHipUpDown.TabIndex = 9;
            // 
            // BellyUpDown
            // 
            this.BellyUpDown.DecimalPlaces = 1;
            this.BellyUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.BellyUpDown.Location = new System.Drawing.Point(125, 104);
            this.BellyUpDown.Name = "BellyUpDown";
            this.BellyUpDown.Size = new System.Drawing.Size(65, 24);
            this.BellyUpDown.TabIndex = 8;
            // 
            // RightBreastUpDown
            // 
            this.RightBreastUpDown.DecimalPlaces = 1;
            this.RightBreastUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.RightBreastUpDown.Location = new System.Drawing.Point(125, 74);
            this.RightBreastUpDown.Name = "RightBreastUpDown";
            this.RightBreastUpDown.Size = new System.Drawing.Size(65, 24);
            this.RightBreastUpDown.TabIndex = 7;
            // 
            // RightArmUpDown
            // 
            this.RightArmUpDown.DecimalPlaces = 1;
            this.RightArmUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.RightArmUpDown.Location = new System.Drawing.Point(125, 44);
            this.RightArmUpDown.Name = "RightArmUpDown";
            this.RightArmUpDown.Size = new System.Drawing.Size(65, 24);
            this.RightArmUpDown.TabIndex = 6;
            // 
            // TorsoUpDown
            // 
            this.TorsoUpDown.DecimalPlaces = 1;
            this.TorsoUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.TorsoUpDown.Location = new System.Drawing.Point(125, 14);
            this.TorsoUpDown.Name = "TorsoUpDown";
            this.TorsoUpDown.Size = new System.Drawing.Size(65, 24);
            this.TorsoUpDown.TabIndex = 5;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.DateTextBox);
            this.groupBox3.Controls.Add(this.SizeTextBox);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.ModelTextBox);
            this.groupBox3.Controls.Add(this.SnTextBox);
            this.groupBox3.Controls.Add(this.NameTextBox);
            this.groupBox3.Location = new System.Drawing.Point(32, 20);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(224, 180);
            this.groupBox3.TabIndex = 69;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Set Up";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(15, 25);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 16);
            this.label4.TabIndex = 67;
            this.label4.Text = "Date";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(15, 145);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 16);
            this.label1.TabIndex = 60;
            this.label1.Text = "Name";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(15, 115);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(27, 16);
            this.label3.TabIndex = 64;
            this.label3.Text = "SN";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(15, 85);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(34, 16);
            this.label5.TabIndex = 68;
            this.label5.Text = "Size";
            // 
            // DateTextBox
            // 
            this.DateTextBox.Location = new System.Drawing.Point(65, 24);
            this.DateTextBox.Name = "DateTextBox";
            this.DateTextBox.Size = new System.Drawing.Size(150, 22);
            this.DateTextBox.TabIndex = 0;
            // 
            // SizeTextBox
            // 
            this.SizeTextBox.Location = new System.Drawing.Point(65, 84);
            this.SizeTextBox.Name = "SizeTextBox";
            this.SizeTextBox.Size = new System.Drawing.Size(130, 22);
            this.SizeTextBox.TabIndex = 2;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(15, 55);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(46, 16);
            this.label2.TabIndex = 63;
            this.label2.Text = "Model";
            // 
            // ModelTextBox
            // 
            this.ModelTextBox.Location = new System.Drawing.Point(65, 54);
            this.ModelTextBox.Name = "ModelTextBox";
            this.ModelTextBox.Size = new System.Drawing.Size(130, 22);
            this.ModelTextBox.TabIndex = 1;
            // 
            // SnTextBox
            // 
            this.SnTextBox.Location = new System.Drawing.Point(65, 114);
            this.SnTextBox.Name = "SnTextBox";
            this.SnTextBox.Size = new System.Drawing.Size(130, 22);
            this.SnTextBox.TabIndex = 3;
            // 
            // NameTextBox
            // 
            this.NameTextBox.Location = new System.Drawing.Point(65, 144);
            this.NameTextBox.Name = "NameTextBox";
            this.NameTextBox.Size = new System.Drawing.Size(130, 22);
            this.NameTextBox.TabIndex = 4;
            // 
            // CloseProfileBtn
            // 
            this.CloseProfileBtn.Location = new System.Drawing.Point(242, 480);
            this.CloseProfileBtn.Name = "CloseProfileBtn";
            this.CloseProfileBtn.Size = new System.Drawing.Size(82, 23);
            this.CloseProfileBtn.TabIndex = 47;
            this.CloseProfileBtn.Text = "Close";
            this.CloseProfileBtn.UseVisualStyleBackColor = true;
            this.CloseProfileBtn.Click += new System.EventHandler(this.CloseProfileBtn_Click);
            // 
            // LoadProfileBtn
            // 
            this.LoadProfileBtn.Location = new System.Drawing.Point(140, 480);
            this.LoadProfileBtn.Name = "LoadProfileBtn";
            this.LoadProfileBtn.Size = new System.Drawing.Size(82, 23);
            this.LoadProfileBtn.TabIndex = 46;
            this.LoadProfileBtn.Text = "Load";
            this.LoadProfileBtn.UseVisualStyleBackColor = true;
            this.LoadProfileBtn.Click += new System.EventHandler(this.LoadProfileBtn_Click);
            // 
            // ProfileGridView
            // 
            this.ProfileGridView.AllowUserToAddRows = false;
            this.ProfileGridView.AllowUserToDeleteRows = false;
            this.ProfileGridView.AllowUserToResizeColumns = false;
            this.ProfileGridView.AllowUserToResizeRows = false;
            this.ProfileGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.ProfileGridView.Cursor = System.Windows.Forms.Cursors.Default;
            this.ProfileGridView.Location = new System.Drawing.Point(5, 229);
            this.ProfileGridView.Name = "ProfileGridView";
            this.ProfileGridView.RowHeadersWidth = 120;
            this.ProfileGridView.Size = new System.Drawing.Size(125, 75);
            this.ProfileGridView.TabIndex = 45;
            this.ProfileGridView.Visible = false;
            // 
            // SaveProfileBtn
            // 
            this.SaveProfileBtn.Location = new System.Drawing.Point(32, 480);
            this.SaveProfileBtn.Name = "SaveProfileBtn";
            this.SaveProfileBtn.Size = new System.Drawing.Size(82, 23);
            this.SaveProfileBtn.TabIndex = 44;
            this.SaveProfileBtn.Text = "Save";
            this.SaveProfileBtn.UseVisualStyleBackColor = true;
            this.SaveProfileBtn.Click += new System.EventHandler(this.SaveProfileBtn_Click);
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(7075, 101);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(70, 22);
            this.textBox3.TabIndex = 33;
            // 
            // Controller2GridView
            // 
            this.Controller2GridView.AllowUserToAddRows = false;
            this.Controller2GridView.AllowUserToDeleteRows = false;
            this.Controller2GridView.AllowUserToResizeColumns = false;
            this.Controller2GridView.AllowUserToResizeRows = false;
            this.Controller2GridView.BackgroundColor = System.Drawing.Color.Black;
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Info;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.Color.LightGray;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Controller2GridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle4;
            this.Controller2GridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Controller2GridView.Cursor = System.Windows.Forms.Cursors.Default;
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle5.ForeColor = System.Drawing.Color.Black;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.Color.White;
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.Controller2GridView.DefaultCellStyle = dataGridViewCellStyle5;
            this.Controller2GridView.Enabled = false;
            this.Controller2GridView.EnableHeadersVisualStyles = false;
            this.Controller2GridView.Location = new System.Drawing.Point(18, 121);
            this.Controller2GridView.Name = "Controller2GridView";
            this.Controller2GridView.ReadOnly = true;
            dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle6.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle6.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle6.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle6.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle6.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Controller2GridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle6;
            this.Controller2GridView.RowHeadersWidth = 120;
            this.Controller2GridView.Size = new System.Drawing.Size(240, 75);
            this.Controller2GridView.TabIndex = 53;
            this.Controller2GridView.Visible = false;
            // 
            // BleGroupBox
            // 
            this.BleGroupBox.BackColor = System.Drawing.Color.WhiteSmoke;
            this.BleGroupBox.Controls.Add(this.BleMgsTextClear);
            this.BleGroupBox.Controls.Add(this.BleMsgTextBox);
            this.BleGroupBox.Controls.Add(this.RxTextBox);
            this.BleGroupBox.Controls.Add(this.BleDisconnectBtn);
            this.BleGroupBox.Controls.Add(this.BleRssiBtn);
            this.BleGroupBox.Controls.Add(this.BleSendBtn);
            this.BleGroupBox.Controls.Add(this.edCharVal);
            this.BleGroupBox.Controls.Add(this.lvDevices);
            this.BleGroupBox.Controls.Add(this.BleConnectBtn);
            this.BleGroupBox.Controls.Add(this.BleDiscoverBtn);
            this.BleGroupBox.Location = new System.Drawing.Point(460, 15);
            this.BleGroupBox.Name = "BleGroupBox";
            this.BleGroupBox.Size = new System.Drawing.Size(341, 620);
            this.BleGroupBox.TabIndex = 52;
            this.BleGroupBox.TabStop = false;
            this.BleGroupBox.Text = "BLE";
            // 
            // BleMgsTextClear
            // 
            this.BleMgsTextClear.Location = new System.Drawing.Point(244, 585);
            this.BleMgsTextClear.Name = "BleMgsTextClear";
            this.BleMgsTextClear.Size = new System.Drawing.Size(82, 23);
            this.BleMgsTextClear.TabIndex = 60;
            this.BleMgsTextClear.Text = "Clear";
            this.BleMgsTextClear.UseVisualStyleBackColor = true;
            this.BleMgsTextClear.Click += new System.EventHandler(this.BleMgsTextClear_Click);
            // 
            // BleMsgTextBox
            // 
            this.BleMsgTextBox.BackColor = System.Drawing.Color.WhiteSmoke;
            this.BleMsgTextBox.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BleMsgTextBox.Location = new System.Drawing.Point(6, 410);
            this.BleMsgTextBox.Name = "BleMsgTextBox";
            this.BleMsgTextBox.ReadOnly = true;
            this.BleMsgTextBox.Size = new System.Drawing.Size(320, 170);
            this.BleMsgTextBox.TabIndex = 59;
            this.BleMsgTextBox.Text = "";
            // 
            // RxTextBox
            // 
            this.RxTextBox.Enabled = false;
            this.RxTextBox.Location = new System.Drawing.Point(142, 295);
            this.RxTextBox.Name = "RxTextBox";
            this.RxTextBox.Size = new System.Drawing.Size(130, 20);
            this.RxTextBox.TabIndex = 58;
            // 
            // BleDisconnectBtn
            // 
            this.BleDisconnectBtn.Location = new System.Drawing.Point(142, 250);
            this.BleDisconnectBtn.Name = "BleDisconnectBtn";
            this.BleDisconnectBtn.Size = new System.Drawing.Size(130, 25);
            this.BleDisconnectBtn.TabIndex = 57;
            this.BleDisconnectBtn.Text = "Disconnect";
            this.BleDisconnectBtn.UseVisualStyleBackColor = true;
            this.BleDisconnectBtn.Click += new System.EventHandler(this.BleDisconnectBtn_Click);
            // 
            // BleRssiBtn
            // 
            this.BleRssiBtn.Location = new System.Drawing.Point(6, 370);
            this.BleRssiBtn.Name = "BleRssiBtn";
            this.BleRssiBtn.Size = new System.Drawing.Size(130, 25);
            this.BleRssiBtn.TabIndex = 56;
            this.BleRssiBtn.Text = "Get RSSI";
            this.BleRssiBtn.UseVisualStyleBackColor = true;
            this.BleRssiBtn.Click += new System.EventHandler(this.BleRssiBtn_Click);
            // 
            // BleSendBtn
            // 
            this.BleSendBtn.Location = new System.Drawing.Point(6, 320);
            this.BleSendBtn.Name = "BleSendBtn";
            this.BleSendBtn.Size = new System.Drawing.Size(130, 25);
            this.BleSendBtn.TabIndex = 55;
            this.BleSendBtn.Text = "Send";
            this.BleSendBtn.UseVisualStyleBackColor = true;
            this.BleSendBtn.Click += new System.EventHandler(this.BleSendBtn_Click);
            // 
            // edCharVal
            // 
            this.edCharVal.Location = new System.Drawing.Point(6, 295);
            this.edCharVal.Name = "edCharVal";
            this.edCharVal.Size = new System.Drawing.Size(130, 20);
            this.edCharVal.TabIndex = 54;
            this.edCharVal.Text = "48656c6c6f20524e343837310a0d";
            // 
            // lvDevices
            // 
            this.lvDevices.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
            this.lvDevices.Cursor = System.Windows.Forms.Cursors.Default;
            this.lvDevices.FullRowSelect = true;
            this.lvDevices.HideSelection = false;
            this.lvDevices.Location = new System.Drawing.Point(6, 54);
            this.lvDevices.Name = "lvDevices";
            this.lvDevices.Size = new System.Drawing.Size(320, 175);
            this.lvDevices.TabIndex = 53;
            this.lvDevices.UseCompatibleStateImageBehavior = false;
            this.lvDevices.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Address";
            this.columnHeader1.Width = 100;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Name";
            this.columnHeader2.Width = 100;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Device Type";
            this.columnHeader3.Width = 100;
            // 
            // BleConnectBtn
            // 
            this.BleConnectBtn.Location = new System.Drawing.Point(6, 250);
            this.BleConnectBtn.Name = "BleConnectBtn";
            this.BleConnectBtn.Size = new System.Drawing.Size(130, 25);
            this.BleConnectBtn.TabIndex = 52;
            this.BleConnectBtn.Text = "Connect";
            this.BleConnectBtn.UseVisualStyleBackColor = true;
            this.BleConnectBtn.Click += new System.EventHandler(this.BleConnectBtn_Click);
            // 
            // ControllerGridView
            // 
            this.ControllerGridView.AllowUserToAddRows = false;
            this.ControllerGridView.AllowUserToDeleteRows = false;
            this.ControllerGridView.AllowUserToResizeColumns = false;
            this.ControllerGridView.AllowUserToResizeRows = false;
            this.ControllerGridView.BackgroundColor = System.Drawing.Color.Black;
            dataGridViewCellStyle7.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle7.BackColor = System.Drawing.SystemColors.Info;
            dataGridViewCellStyle7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle7.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle7.SelectionBackColor = System.Drawing.Color.LightGray;
            dataGridViewCellStyle7.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle7.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.ControllerGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle7;
            this.ControllerGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.ControllerGridView.Cursor = System.Windows.Forms.Cursors.Default;
            dataGridViewCellStyle8.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle8.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle8.ForeColor = System.Drawing.Color.Black;
            dataGridViewCellStyle8.SelectionBackColor = System.Drawing.Color.White;
            dataGridViewCellStyle8.SelectionForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle8.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.ControllerGridView.DefaultCellStyle = dataGridViewCellStyle8;
            this.ControllerGridView.Enabled = false;
            this.ControllerGridView.EnableHeadersVisualStyles = false;
            this.ControllerGridView.Location = new System.Drawing.Point(18, 32);
            this.ControllerGridView.Name = "ControllerGridView";
            this.ControllerGridView.ReadOnly = true;
            dataGridViewCellStyle9.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle9.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle9.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle9.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle9.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle9.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle9.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.ControllerGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle9;
            this.ControllerGridView.RowHeadersWidth = 120;
            this.ControllerGridView.Size = new System.Drawing.Size(240, 75);
            this.ControllerGridView.TabIndex = 48;
            this.ControllerGridView.Visible = false;
            // 
            // BrdTypeLbl
            // 
            this.BrdTypeLbl.AutoSize = true;
            this.BrdTypeLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BrdTypeLbl.Location = new System.Drawing.Point(112, 0);
            this.BrdTypeLbl.Name = "BrdTypeLbl";
            this.BrdTypeLbl.Size = new System.Drawing.Size(96, 16);
            this.BrdTypeLbl.TabIndex = 46;
            this.BrdTypeLbl.Text = "Board type NA";
            // 
            // CommSelectPnl
            // 
            this.CommSelectPnl.BackColor = System.Drawing.SystemColors.ControlLight;
            this.CommSelectPnl.Controls.Add(this.label22);
            this.CommSelectPnl.Controls.Add(this.button14);
            this.CommSelectPnl.Controls.Add(this.button13);
            this.CommSelectPnl.Controls.Add(this.CommportComboBox);
            this.CommSelectPnl.Location = new System.Drawing.Point(1004, 502);
            this.CommSelectPnl.Name = "CommSelectPnl";
            this.CommSelectPnl.Size = new System.Drawing.Size(156, 70);
            this.CommSelectPnl.TabIndex = 37;
            this.CommSelectPnl.Visible = false;
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(16, 24);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(46, 13);
            this.label22.TabIndex = 40;
            this.label22.Text = "Comport";
            // 
            // button14
            // 
            this.button14.Location = new System.Drawing.Point(92, 65);
            this.button14.Name = "button14";
            this.button14.Size = new System.Drawing.Size(50, 23);
            this.button14.TabIndex = 39;
            this.button14.Text = "Cancel";
            this.button14.UseVisualStyleBackColor = true;
            this.button14.Click += new System.EventHandler(this.button14_Click);
            // 
            // button13
            // 
            this.button13.Location = new System.Drawing.Point(17, 65);
            this.button13.Name = "button13";
            this.button13.Size = new System.Drawing.Size(50, 23);
            this.button13.TabIndex = 38;
            this.button13.Text = "OK";
            this.button13.UseVisualStyleBackColor = true;
            this.button13.Click += new System.EventHandler(this.button13_Click);
            // 
            // CommportComboBox
            // 
            this.CommportComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.CommportComboBox.FormattingEnabled = true;
            this.CommportComboBox.Location = new System.Drawing.Point(70, 21);
            this.CommportComboBox.Name = "CommportComboBox";
            this.CommportComboBox.Size = new System.Drawing.Size(75, 21);
            this.CommportComboBox.TabIndex = 37;
            // 
            // FwBrd1VersionLbl
            // 
            this.FwBrd1VersionLbl.AutoSize = true;
            this.FwBrd1VersionLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FwBrd1VersionLbl.Location = new System.Drawing.Point(0, 0);
            this.FwBrd1VersionLbl.Name = "FwBrd1VersionLbl";
            this.FwBrd1VersionLbl.Size = new System.Drawing.Size(78, 16);
            this.FwBrd1VersionLbl.TabIndex = 28;
            this.FwBrd1VersionLbl.Text = "FW Version";
            // 
            // TestBtn
            // 
            this.TestBtn.Location = new System.Drawing.Point(398, 32);
            this.TestBtn.Name = "TestBtn";
            this.TestBtn.Size = new System.Drawing.Size(75, 23);
            this.TestBtn.TabIndex = 63;
            this.TestBtn.Text = "button1";
            this.TestBtn.UseVisualStyleBackColor = true;
            this.TestBtn.Click += new System.EventHandler(this.button1_Click_2);
            // 
            // FwBrd2VersionLbl
            // 
            this.FwBrd2VersionLbl.AutoSize = true;
            this.FwBrd2VersionLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FwBrd2VersionLbl.Location = new System.Drawing.Point(257, 0);
            this.FwBrd2VersionLbl.Name = "FwBrd2VersionLbl";
            this.FwBrd2VersionLbl.Size = new System.Drawing.Size(78, 16);
            this.FwBrd2VersionLbl.TabIndex = 55;
            this.FwBrd2VersionLbl.Text = "FW Version";
            // 
            // Brd2TypeLbl
            // 
            this.Brd2TypeLbl.AutoSize = true;
            this.Brd2TypeLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Brd2TypeLbl.Location = new System.Drawing.Point(377, 0);
            this.Brd2TypeLbl.Name = "Brd2TypeLbl";
            this.Brd2TypeLbl.Size = new System.Drawing.Size(96, 16);
            this.Brd2TypeLbl.TabIndex = 65;
            this.Brd2TypeLbl.Text = "Board type NA";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.radioButton3);
            this.groupBox2.Controls.Add(this.radioButton2);
            this.groupBox2.Controls.Add(this.V1RadioBtn);
            this.groupBox2.Controls.Add(this.button20);
            this.groupBox2.Controls.Add(this.button19);
            this.groupBox2.Location = new System.Drawing.Point(1162, 330);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(205, 110);
            this.groupBox2.TabIndex = 33;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Solenoids";
            this.groupBox2.Visible = false;
            // 
            // radioButton3
            // 
            this.radioButton3.AutoSize = true;
            this.radioButton3.Location = new System.Drawing.Point(132, 29);
            this.radioButton3.Name = "radioButton3";
            this.radioButton3.Size = new System.Drawing.Size(38, 17);
            this.radioButton3.TabIndex = 44;
            this.radioButton3.Text = "V3";
            this.radioButton3.UseVisualStyleBackColor = true;
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Location = new System.Drawing.Point(65, 27);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(38, 17);
            this.radioButton2.TabIndex = 43;
            this.radioButton2.Text = "V2";
            this.radioButton2.UseVisualStyleBackColor = true;
            // 
            // V1RadioBtn
            // 
            this.V1RadioBtn.AutoSize = true;
            this.V1RadioBtn.Checked = true;
            this.V1RadioBtn.Location = new System.Drawing.Point(15, 29);
            this.V1RadioBtn.Name = "V1RadioBtn";
            this.V1RadioBtn.Size = new System.Drawing.Size(38, 17);
            this.V1RadioBtn.TabIndex = 42;
            this.V1RadioBtn.TabStop = true;
            this.V1RadioBtn.Text = "V1";
            this.V1RadioBtn.UseVisualStyleBackColor = true;
            // 
            // button20
            // 
            this.button20.Location = new System.Drawing.Point(112, 78);
            this.button20.Name = "button20";
            this.button20.Size = new System.Drawing.Size(82, 23);
            this.button20.TabIndex = 41;
            this.button20.Text = "Close Valve";
            this.button20.UseVisualStyleBackColor = true;
            this.button20.Click += new System.EventHandler(this.button20_Click);
            // 
            // button19
            // 
            this.button19.Location = new System.Drawing.Point(15, 78);
            this.button19.Name = "button19";
            this.button19.Size = new System.Drawing.Size(82, 23);
            this.button19.TabIndex = 40;
            this.button19.Text = "Open Valve";
            this.button19.UseVisualStyleBackColor = true;
            this.button19.Click += new System.EventHandler(this.button19_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label11);
            this.groupBox1.Controls.Add(this.SenThresTextBox);
            this.groupBox1.Controls.Add(this.TemperatureTextBox);
            this.groupBox1.Controls.Add(this.PressureTextBox);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Location = new System.Drawing.Point(1083, 40);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(205, 99);
            this.groupBox1.TabIndex = 32;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Operation";
            this.groupBox1.Visible = false;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(18, 53);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(106, 13);
            this.label11.TabIndex = 37;
            this.label11.Text = "Temperature (Deg C)";
            // 
            // SenThresTextBox
            // 
            this.SenThresTextBox.Location = new System.Drawing.Point(7075, 101);
            this.SenThresTextBox.Name = "SenThresTextBox";
            this.SenThresTextBox.Size = new System.Drawing.Size(70, 20);
            this.SenThresTextBox.TabIndex = 33;
            // 
            // TemperatureTextBox
            // 
            this.TemperatureTextBox.Location = new System.Drawing.Point(130, 50);
            this.TemperatureTextBox.Name = "TemperatureTextBox";
            this.TemperatureTextBox.Size = new System.Drawing.Size(60, 20);
            this.TemperatureTextBox.TabIndex = 31;
            // 
            // PressureTextBox
            // 
            this.PressureTextBox.Location = new System.Drawing.Point(130, 25);
            this.PressureTextBox.Name = "PressureTextBox";
            this.PressureTextBox.Size = new System.Drawing.Size(60, 20);
            this.PressureTextBox.TabIndex = 30;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(18, 28);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(76, 13);
            this.label9.TabIndex = 29;
            this.label9.Text = "Pressure (kPa)";
            // 
            // SerialMonitorTimer
            // 
            this.SerialMonitorTimer.Interval = 2000;
            this.SerialMonitorTimer.Tick += new System.EventHandler(this.SerialMonitorTimer_Tick);
            // 
            // MotorCntrlTimer
            // 
            this.MotorCntrlTimer.Interval = 1000;
            this.MotorCntrlTimer.Tick += new System.EventHandler(this.MotorCntrlTimer_Tick);
            // 
            // FtcStatusStrip
            // 
            this.FtcStatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.FtcStatusStrip.Location = new System.Drawing.Point(0, 694);
            this.FtcStatusStrip.Name = "FtcStatusStrip";
            this.FtcStatusStrip.Size = new System.Drawing.Size(1248, 22);
            this.FtcStatusStrip.TabIndex = 26;
            this.FtcStatusStrip.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(118, 17);
            this.toolStripStatusLabel1.Text = "toolStripStatusLabel1";
            // 
            // FileSystemWatcher
            // 
            this.FileSystemWatcher.EnableRaisingEvents = true;
            this.FileSystemWatcher.Filter = "\"*.h264\"";
            this.FileSystemWatcher.SynchronizingObject = this;
            this.FileSystemWatcher.Changed += new System.IO.FileSystemEventHandler(this.FileSystemWatcher_Changed);
            // 
            // FileMonitorTimer
            // 
            this.FileMonitorTimer.Interval = 500;
            this.FileMonitorTimer.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // CompressorCntrlGridView
            // 
            this.CompressorCntrlGridView.AllowUserToAddRows = false;
            this.CompressorCntrlGridView.AllowUserToDeleteRows = false;
            this.CompressorCntrlGridView.AllowUserToResizeColumns = false;
            this.CompressorCntrlGridView.AllowUserToResizeRows = false;
            this.CompressorCntrlGridView.BackgroundColor = System.Drawing.Color.Black;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Info;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.Color.LightGray;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.CompressorCntrlGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.CompressorCntrlGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.CompressorCntrlGridView.Cursor = System.Windows.Forms.Cursors.Default;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.Black;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.Color.White;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.CompressorCntrlGridView.DefaultCellStyle = dataGridViewCellStyle2;
            this.CompressorCntrlGridView.Enabled = false;
            this.CompressorCntrlGridView.EnableHeadersVisualStyles = false;
            this.CompressorCntrlGridView.Location = new System.Drawing.Point(268, 157);
            this.CompressorCntrlGridView.Name = "CompressorCntrlGridView";
            this.CompressorCntrlGridView.ReadOnly = true;
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.CompressorCntrlGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.CompressorCntrlGridView.RowHeadersWidth = 120;
            this.CompressorCntrlGridView.Size = new System.Drawing.Size(82, 41);
            this.CompressorCntrlGridView.TabIndex = 66;
            this.CompressorCntrlGridView.Visible = false;
            // 
            // FwBrd3VersionLbl
            // 
            this.FwBrd3VersionLbl.AutoSize = true;
            this.FwBrd3VersionLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FwBrd3VersionLbl.Location = new System.Drawing.Point(518, 0);
            this.FwBrd3VersionLbl.Name = "FwBrd3VersionLbl";
            this.FwBrd3VersionLbl.Size = new System.Drawing.Size(78, 16);
            this.FwBrd3VersionLbl.TabIndex = 61;
            this.FwBrd3VersionLbl.Text = "FW Version";
            // 
            // Brd3TypeLbl
            // 
            this.Brd3TypeLbl.AutoSize = true;
            this.Brd3TypeLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Brd3TypeLbl.Location = new System.Drawing.Point(613, 0);
            this.Brd3TypeLbl.Name = "Brd3TypeLbl";
            this.Brd3TypeLbl.Size = new System.Drawing.Size(96, 16);
            this.Brd3TypeLbl.TabIndex = 67;
            this.Brd3TypeLbl.Text = "Board type NA";
            // 
            // TheMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(1248, 716);
            this.Controls.Add(this.FtcStatusStrip);
            this.Controls.Add(this.MainPanel);
            this.Controls.Add(this.OpenSerialPortBtn);
            this.Controls.Add(this.button8);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "TheMainForm";
            this.Text = "WinAMC version 1.00";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TheMainForm_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.MainPanel.ResumeLayout(false);
            this.MainPanel.PerformLayout();
            this.OperationPanel.ResumeLayout(false);
            this.OperationPanel.PerformLayout();
            this.MotorGroupBox.ResumeLayout(false);
            this.MotorGroupBox.PerformLayout();
            this.CameraGroupBox.ResumeLayout(false);
            this.CameraGroupBox.PerformLayout();
            this.FitGroupBox.ResumeLayout(false);
            this.ControlPanel.ResumeLayout(false);
            this.ControlPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.MotorSpeedUpDown)).EndInit();
            this.ProfileGroupBox.ResumeLayout(false);
            this.ProfileGroupBox.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.LeftLegUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftButtockUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftHipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftCalfUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftBreastUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LeftArmUpDown)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RightLegUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightButtockUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightCalfUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightHipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BellyUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightBreastUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RightArmUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.TorsoUpDown)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ProfileGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Controller2GridView)).EndInit();
            this.BleGroupBox.ResumeLayout(false);
            this.BleGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ControllerGridView)).EndInit();
            this.CommSelectPnl.ResumeLayout(false);
            this.CommSelectPnl.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.FtcStatusStrip.ResumeLayout(false);
            this.FtcStatusStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.FileSystemWatcher)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CompressorCntrlGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button button8;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button OpenSerialPortBtn;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem diagnosticToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem diagnosticToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem downloadToolStripMenuItem;
        private System.Windows.Forms.Panel MainPanel;
        private System.Windows.Forms.ToolStripMenuItem setupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.Label FwBrd1VersionLbl;
        private System.Windows.Forms.ToolStripMenuItem fPCToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox SenThresTextBox;
        private System.Windows.Forms.TextBox PressureTextBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Timer SerialMonitorTimer;
        private System.Windows.Forms.Panel CommSelectPnl;
        private System.Windows.Forms.Button button14;
        private System.Windows.Forms.Button button13;
        private System.Windows.Forms.ComboBox CommportComboBox;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fPCParametersToolStripMenuItem;
        private System.Windows.Forms.Timer MotorCntrlTimer;
        private System.Windows.Forms.ToolStripMenuItem counterToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem logToolStripMenuItem;
        private System.Windows.Forms.StatusStrip FtcStatusStrip;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.Button button20;
        private System.Windows.Forms.Button button19;
        private System.Windows.Forms.Label BrdTypeLbl;
        private System.Windows.Forms.RadioButton radioButton3;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.RadioButton V1RadioBtn;
        private System.Windows.Forms.GroupBox ProfileGroupBox;
        private System.Windows.Forms.Button SaveProfileBtn;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.DataGridView ProfileGridView;
        private System.Windows.Forms.Button LoadProfileBtn;
        private System.Windows.Forms.TextBox TemperatureTextBox;
        private System.Windows.Forms.DataGridView ControllerGridView;
        private System.Windows.Forms.Button StartFitBtn1;
        private System.Windows.Forms.Button GetPressBtn;
        private System.Windows.Forms.Button BleDiscoverBtn;
        private System.Windows.Forms.GroupBox BleGroupBox;
        private System.Windows.Forms.Button BleConnectBtn;
        private System.Windows.Forms.ListView lvDevices;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.Button BleSendBtn;
        private System.Windows.Forms.TextBox edCharVal;
        private System.Windows.Forms.Button BleRssiBtn;
        private System.Windows.Forms.Button BleDisconnectBtn;
        private System.Windows.Forms.TextBox RxTextBox;
        private System.Windows.Forms.DataGridView Controller2GridView;
        private System.Windows.Forms.Button StartFitBtn2;
        private System.Windows.Forms.Button CloseProfileBtn;
        private System.Windows.Forms.Label FwBrd2VersionLbl;
        private System.Windows.Forms.RichTextBox BleMsgTextBox;
        private System.Windows.Forms.Button BleMgsTextClear;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox NameTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox SizeTextBox;
        private System.Windows.Forms.TextBox DateTextBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox SnTextBox;
        private System.Windows.Forms.TextBox ModelTextBox;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.NumericUpDown RightBreastUpDown;
        private System.Windows.Forms.NumericUpDown RightArmUpDown;
        private System.Windows.Forms.NumericUpDown TorsoUpDown;
        private System.Windows.Forms.NumericUpDown RightLegUpDown;
        private System.Windows.Forms.NumericUpDown RightButtockUpDown;
        private System.Windows.Forms.NumericUpDown RightHipUpDown;
        private System.Windows.Forms.NumericUpDown BellyUpDown;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.NumericUpDown LeftLegUpDown;
        private System.Windows.Forms.NumericUpDown LeftButtockUpDown;
        private System.Windows.Forms.NumericUpDown LeftHipUpDown;
        private System.Windows.Forms.NumericUpDown LeftCalfUpDown;
        private System.Windows.Forms.NumericUpDown LeftBreastUpDown;
        private System.Windows.Forms.NumericUpDown LeftArmUpDown;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.NumericUpDown RightCalfUpDown;
        private System.IO.FileSystemWatcher FileSystemWatcher;
        private System.Windows.Forms.Panel ControlPanel;
        private System.Windows.Forms.Button MotorFwdBtn;
        private System.Windows.Forms.Button MotorStopBtn;
        private System.Windows.Forms.Button MotorRevBtn;
        private System.Windows.Forms.Button CloseControlPanelBtn;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown MotorSpeedUpDown;
        private System.Windows.Forms.TextBox VoltageTextBox;
        private System.Windows.Forms.TextBox CurrentTextBox;
        private System.Windows.Forms.Label LimitLbl;
        private System.Windows.Forms.Label CurrentLbl;
        private System.Windows.Forms.Label VoltageLbl;
        private System.Windows.Forms.Button RevLimitInd;
        private System.Windows.Forms.Button FwdLimitInd;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label SpeedLbl;
        private System.Windows.Forms.TextBox SpeedTextBox;
        private System.Windows.Forms.Button DirFwdInd;
        private System.Windows.Forms.Button DirRevInd;
        private System.Windows.Forms.NumericUpDown numericUpDown1;
        private System.Windows.Forms.ToolStripMenuItem motorDiagnosticToolStripMenuItem;
        private System.Windows.Forms.Panel OperationPanel;
        private System.Windows.Forms.Button StartPreviewBtn;
        private System.Windows.Forms.Button TestBtn;
        private System.Windows.Forms.Button CaptureBtn;
        private System.Windows.Forms.Button StopCaptureBtn;
        private System.Windows.Forms.Button StopBtn;
        private System.Windows.Forms.Button RevBtn;
        private System.Windows.Forms.Button FwdBtn;
        private System.Windows.Forms.Button UploadFilesBtn;
        private System.Windows.Forms.Button OpCloseBtn;
        private System.Windows.Forms.Label MotorDetectLbl;
        private System.Windows.Forms.ComboBox FitTypeComboBox;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.TextBox SnLoadTextBox;
        private System.Windows.Forms.Button OpLoadBtn;
        private System.Windows.Forms.ComboBox FitTypeLoadComboBox;
        private System.Windows.Forms.Button OpTightFitBtn;
        private System.Windows.Forms.Button OpNormalFitBtn;
        private System.Windows.Forms.Button OpLooseFitBtn;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.GroupBox FitGroupBox;
        private System.Windows.Forms.GroupBox CameraGroupBox;
        private System.Windows.Forms.GroupBox MotorGroupBox;
        private System.Windows.Forms.Timer FileMonitorTimer;
        private System.Windows.Forms.Label CameraOperationLbl;
        private System.Windows.Forms.Label CameraStatusLbl;
        private System.Windows.Forms.Label MotorStatusLbl;
        private System.Windows.Forms.Label CameraPortLbl;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label Brd2TypeLbl;
        private System.Windows.Forms.DataGridView CompressorCntrlGridView;
        private System.Windows.Forms.Label Brd3TypeLbl;
        private System.Windows.Forms.Label FwBrd3VersionLbl;
        //private Gigasoft.ProEssentials.Pesgo GenericPlot;
    }
}

