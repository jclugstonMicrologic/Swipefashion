namespace WindowsFormsApplication5
{
    partial class PanelsFrm
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
            this.CommportBox = new System.Windows.Forms.GroupBox();
            this.ComDescLbl = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.ComportBox = new System.Windows.Forms.ComboBox();
            this.FpcParamBox = new System.Windows.Forms.GroupBox();
            this.button6 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.FpcGroupBox = new System.Windows.Forms.GroupBox();
            this.label21 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.P6TextBox = new System.Windows.Forms.TextBox();
            this.P5TextBox = new System.Windows.Forms.TextBox();
            this.P4TextBox = new System.Windows.Forms.TextBox();
            this.P3TextBox = new System.Windows.Forms.TextBox();
            this.P2TextBox = new System.Windows.Forms.TextBox();
            this.P1TextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.CounterBox = new System.Windows.Forms.GroupBox();
            this.counterUpDown = new System.Windows.Forms.NumericUpDown();
            this.button7 = new System.Windows.Forms.Button();
            this.button8 = new System.Windows.Forms.Button();
            this.CommportBox.SuspendLayout();
            this.FpcParamBox.SuspendLayout();
            this.FpcGroupBox.SuspendLayout();
            this.CounterBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.counterUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // CommportBox
            // 
            this.CommportBox.Controls.Add(this.ComDescLbl);
            this.CommportBox.Controls.Add(this.button2);
            this.CommportBox.Controls.Add(this.button1);
            this.CommportBox.Controls.Add(this.ComportBox);
            this.CommportBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CommportBox.Location = new System.Drawing.Point(20, 20);
            this.CommportBox.Name = "CommportBox";
            this.CommportBox.Size = new System.Drawing.Size(290, 146);
            this.CommportBox.TabIndex = 0;
            this.CommportBox.TabStop = false;
            this.CommportBox.Text = "Comm Select";
            this.CommportBox.Visible = false;
            // 
            // ComDescLbl
            // 
            this.ComDescLbl.AutoSize = true;
            this.ComDescLbl.Location = new System.Drawing.Point(20, 120);
            this.ComDescLbl.Name = "ComDescLbl";
            this.ComDescLbl.Size = new System.Drawing.Size(74, 16);
            this.ComDescLbl.TabIndex = 38;
            this.ComDescLbl.Text = "Comm Port";
            this.ComDescLbl.Visible = false;
            // 
            // button2
            // 
            this.button2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button2.Location = new System.Drawing.Point(85, 80);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(50, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "Cancel";
            this.button2.UseVisualStyleBackColor = true;

            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button1.Location = new System.Drawing.Point(20, 80);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(50, 23);
            this.button1.TabIndex = 3;
            this.button1.Text = "OK";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // ComportBox
            // 
            this.ComportBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ComportBox.FormattingEnabled = true;
            this.ComportBox.Location = new System.Drawing.Point(20, 28);
            this.ComportBox.Name = "ComportBox";
            this.ComportBox.Size = new System.Drawing.Size(70, 24);
            this.ComportBox.TabIndex = 2;
            // 
            // FpcParamBox
            // 
            this.FpcParamBox.Controls.Add(this.button6);
            this.FpcParamBox.Controls.Add(this.button5);
            this.FpcParamBox.Controls.Add(this.FpcGroupBox);
            this.FpcParamBox.Controls.Add(this.button3);
            this.FpcParamBox.Controls.Add(this.button4);
            this.FpcParamBox.Location = new System.Drawing.Point(411, 20);
            this.FpcParamBox.Name = "FpcParamBox";
            this.FpcParamBox.Size = new System.Drawing.Size(400, 200);
            this.FpcParamBox.TabIndex = 1;
            this.FpcParamBox.TabStop = false;
            this.FpcParamBox.Text = "FPC Setup";
            this.FpcParamBox.Visible = false;
            // 
            // button6
            // 
            this.button6.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button6.Location = new System.Drawing.Point(85, 145);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(50, 23);
            this.button6.TabIndex = 34;
            this.button6.Text = "Cancel";
            this.button6.UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            this.button5.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button5.Location = new System.Drawing.Point(20, 145);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(50, 23);
            this.button5.TabIndex = 33;
            this.button5.Text = "Send";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // FpcGroupBox
            // 
            this.FpcGroupBox.Controls.Add(this.label21);
            this.FpcGroupBox.Controls.Add(this.label20);
            this.FpcGroupBox.Controls.Add(this.label19);
            this.FpcGroupBox.Controls.Add(this.label18);
            this.FpcGroupBox.Controls.Add(this.label17);
            this.FpcGroupBox.Controls.Add(this.P6TextBox);
            this.FpcGroupBox.Controls.Add(this.P5TextBox);
            this.FpcGroupBox.Controls.Add(this.P4TextBox);
            this.FpcGroupBox.Controls.Add(this.P3TextBox);
            this.FpcGroupBox.Controls.Add(this.P2TextBox);
            this.FpcGroupBox.Controls.Add(this.P1TextBox);
            this.FpcGroupBox.Controls.Add(this.label8);
            this.FpcGroupBox.Location = new System.Drawing.Point(20, 20);
            this.FpcGroupBox.Name = "FpcGroupBox";
            this.FpcGroupBox.Size = new System.Drawing.Size(370, 110);
            this.FpcGroupBox.TabIndex = 32;
            this.FpcGroupBox.TabStop = false;
            this.FpcGroupBox.Text = "Pulser Setup";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(180, 78);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(20, 13);
            this.label21.TabIndex = 40;
            this.label21.Text = "P6";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(180, 53);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(20, 13);
            this.label20.TabIndex = 39;
            this.label20.Text = "P5";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(180, 28);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(20, 13);
            this.label19.TabIndex = 38;
            this.label19.Text = "P4";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(18, 78);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(20, 13);
            this.label18.TabIndex = 37;
            this.label18.Text = "P3";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(18, 53);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(20, 13);
            this.label17.TabIndex = 36;
            this.label17.Text = "P2";
            // 
            // P6TextBox
            // 
            this.P6TextBox.Location = new System.Drawing.Point(250, 75);
            this.P6TextBox.Name = "P6TextBox";
            this.P6TextBox.Size = new System.Drawing.Size(70, 20);
            this.P6TextBox.TabIndex = 35;
            // 
            // P5TextBox
            // 
            this.P5TextBox.Location = new System.Drawing.Point(250, 50);
            this.P5TextBox.Name = "P5TextBox";
            this.P5TextBox.Size = new System.Drawing.Size(70, 20);
            this.P5TextBox.TabIndex = 34;
            // 
            // P4TextBox
            // 
            this.P4TextBox.Location = new System.Drawing.Point(250, 25);
            this.P4TextBox.Name = "P4TextBox";
            this.P4TextBox.Size = new System.Drawing.Size(70, 20);
            this.P4TextBox.TabIndex = 33;
            // 
            // P3TextBox
            // 
            this.P3TextBox.Location = new System.Drawing.Point(80, 75);
            this.P3TextBox.Name = "P3TextBox";
            this.P3TextBox.Size = new System.Drawing.Size(70, 20);
            this.P3TextBox.TabIndex = 32;
            // 
            // P2TextBox
            // 
            this.P2TextBox.Location = new System.Drawing.Point(80, 50);
            this.P2TextBox.Name = "P2TextBox";
            this.P2TextBox.Size = new System.Drawing.Size(70, 20);
            this.P2TextBox.TabIndex = 31;
            // 
            // P1TextBox
            // 
            this.P1TextBox.Location = new System.Drawing.Point(80, 25);
            this.P1TextBox.Name = "P1TextBox";
            this.P1TextBox.Size = new System.Drawing.Size(70, 20);
            this.P1TextBox.TabIndex = 30;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(18, 28);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(20, 13);
            this.label8.TabIndex = 29;
            this.label8.Text = "P1";
            // 
            // button3
            // 
            this.button3.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button3.Location = new System.Drawing.Point(85, 91);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(50, 23);
            this.button3.TabIndex = 4;
            this.button3.Text = "Cancel";
            this.button3.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this.button4.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button4.Location = new System.Drawing.Point(20, 91);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(50, 23);
            this.button4.TabIndex = 3;
            this.button4.Text = "OK";
            this.button4.UseVisualStyleBackColor = true;
            // 
            // CounterBox
            // 
            this.CounterBox.Controls.Add(this.counterUpDown);
            this.CounterBox.Controls.Add(this.button7);
            this.CounterBox.Controls.Add(this.button8);
            this.CounterBox.Location = new System.Drawing.Point(20, 233);
            this.CounterBox.Name = "CounterBox";
            this.CounterBox.Size = new System.Drawing.Size(168, 135);
            this.CounterBox.TabIndex = 2;
            this.CounterBox.TabStop = false;
            this.CounterBox.Text = "Counter";
            this.CounterBox.Visible = false;
            // 
            // counterUpDown
            // 
            this.counterUpDown.Location = new System.Drawing.Point(20, 35);
            this.counterUpDown.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.counterUpDown.Name = "counterUpDown";
            this.counterUpDown.Size = new System.Drawing.Size(70, 20);
            this.counterUpDown.TabIndex = 3;
            // 
            // button7
            // 
            this.button7.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button7.Location = new System.Drawing.Point(85, 91);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(50, 23);
            this.button7.TabIndex = 4;
            this.button7.Text = "Cancel";
            this.button7.UseVisualStyleBackColor = true;
            // 
            // button8
            // 
            this.button8.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button8.Location = new System.Drawing.Point(20, 91);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(50, 23);
            this.button8.TabIndex = 3;
            this.button8.Text = "Send";
            this.button8.UseVisualStyleBackColor = true;

            // 
            // PanelsFrm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(646, 500);
            this.Controls.Add(this.CounterBox);
            this.Controls.Add(this.FpcParamBox);
            this.Controls.Add(this.CommportBox);
            this.Name = "PanelsFrm";
            this.CommportBox.ResumeLayout(false);
            this.CommportBox.PerformLayout();
            this.FpcParamBox.ResumeLayout(false);
            this.FpcGroupBox.ResumeLayout(false);
            this.FpcGroupBox.PerformLayout();
            this.CounterBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.counterUpDown)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox CommportBox;
        private System.Windows.Forms.ComboBox ComportBox;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.GroupBox FpcParamBox;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.GroupBox FpcGroupBox;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox P6TextBox;
        private System.Windows.Forms.TextBox P5TextBox;
        private System.Windows.Forms.TextBox P4TextBox;
        private System.Windows.Forms.TextBox P3TextBox;
        private System.Windows.Forms.TextBox P2TextBox;
        private System.Windows.Forms.TextBox P1TextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.GroupBox CounterBox;
        private System.Windows.Forms.NumericUpDown counterUpDown;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.Label ComDescLbl;
    }
}