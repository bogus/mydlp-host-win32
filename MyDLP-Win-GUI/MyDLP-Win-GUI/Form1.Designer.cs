using CustomUIControls;
using System.Windows.Forms;
using System.Resources;
using System.Threading;
using System.Globalization;

namespace MydlpWinGui
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.eventLogSensFile = new System.Diagnostics.EventLog();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.openConfigurationMenuToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.disableOnLineScanningToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.enableOnLineScannigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.menuOptions = new System.Windows.Forms.LinkLabel();
            this.menuOnlineScan = new System.Windows.Forms.LinkLabel();
            this.menuScan = new System.Windows.Forms.LinkLabel();
            this.menuDefSensData = new System.Windows.Forms.LinkLabel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panelTitle = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.eventLogSensFile)).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
            this.notifyIcon1.Text = "SysTray Test Application by MyDLP";
            this.notifyIcon1.Visible = true;
            this.notifyIcon1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIcon1_MouseDoubleClick);
            // 
            // eventLogSensFile
            // 
            this.eventLogSensFile.EnableRaisingEvents = true;
            this.eventLogSensFile.Log = "MyDLPLog";
            this.eventLogSensFile.Source = "MyDLPLogs-SF";
            this.eventLogSensFile.SynchronizingObject = this;
            this.eventLogSensFile.EntryWritten += new System.Diagnostics.EntryWrittenEventHandler(this.eventLogSensFile_EntryWritten);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openConfigurationMenuToolStripMenuItem,
            this.disableOnLineScanningToolStripMenuItem,
            this.enableOnLineScannigToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(209, 70);
            // 
            // openConfigurationMenuToolStripMenuItem
            // 
            this.openConfigurationMenuToolStripMenuItem.Name = "openConfigurationMenuToolStripMenuItem";
            this.openConfigurationMenuToolStripMenuItem.Size = new System.Drawing.Size(208, 22);
            this.openConfigurationMenuToolStripMenuItem.Text = "Open Configuration Menu";
            // 
            // disableOnLineScanningToolStripMenuItem
            // 
            this.disableOnLineScanningToolStripMenuItem.Name = "disableOnLineScanningToolStripMenuItem";
            this.disableOnLineScanningToolStripMenuItem.Size = new System.Drawing.Size(208, 22);
            this.disableOnLineScanningToolStripMenuItem.Text = "Disable On-Line Scanning";
            // 
            // enableOnLineScannigToolStripMenuItem
            // 
            this.enableOnLineScannigToolStripMenuItem.Name = "enableOnLineScannigToolStripMenuItem";
            this.enableOnLineScannigToolStripMenuItem.Size = new System.Drawing.Size(208, 22);
            this.enableOnLineScannigToolStripMenuItem.Text = "Enable On-Line Scannig";
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.White;
            this.panel1.Controls.Add(this.menuOptions);
            this.panel1.Controls.Add(this.menuOnlineScan);
            this.panel1.Controls.Add(this.menuScan);
            this.panel1.Controls.Add(this.menuDefSensData);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(178, 466);
            this.panel1.TabIndex = 1;
            // 
            // menuOptions
            // 
            this.menuOptions.AutoSize = true;
            this.menuOptions.Location = new System.Drawing.Point(13, 113);
            this.menuOptions.Name = "menuOptions";
            this.menuOptions.Size = new System.Drawing.Size(43, 13);
            this.menuOptions.TabIndex = 3;
            this.menuOptions.TabStop = true;
            this.menuOptions.Text = "Options";
            // 
            // menuOnlineScan
            // 
            this.menuOnlineScan.AutoSize = true;
            this.menuOnlineScan.Location = new System.Drawing.Point(13, 84);
            this.menuOnlineScan.Name = "menuOnlineScan";
            this.menuOnlineScan.Size = new System.Drawing.Size(126, 13);
            this.menuOnlineScan.TabIndex = 2;
            this.menuOnlineScan.TabStop = true;
            this.menuOnlineScan.Text = "Removable Device Scan";
            // 
            // menuScan
            // 
            this.menuScan.AutoSize = true;
            this.menuScan.Location = new System.Drawing.Point(13, 55);
            this.menuScan.Name = "menuScan";
            this.menuScan.Size = new System.Drawing.Size(131, 13);
            this.menuScan.TabIndex = 1;
            this.menuScan.TabStop = true;
            this.menuScan.Text = "Scan Directory or Devices";
            this.menuScan.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.menuScan_LinkClicked);
            // 
            // menuDefSensData
            // 
            this.menuDefSensData.AutoEllipsis = true;
            this.menuDefSensData.AutoSize = true;
            this.menuDefSensData.Location = new System.Drawing.Point(13, 26);
            this.menuDefSensData.Name = "menuDefSensData";
            this.menuDefSensData.Size = new System.Drawing.Size(110, 13);
            this.menuDefSensData.TabIndex = 0;
            this.menuDefSensData.TabStop = true;
            this.menuDefSensData.Text = "Define Sensitive Data";
            this.menuDefSensData.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.menuDefSensData_LinkClicked);
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.SystemColors.Control;
            this.panel2.Location = new System.Drawing.Point(184, 42);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(546, 424);
            this.panel2.TabIndex = 2;
            // 
            // panelTitle
            // 
            this.panelTitle.AutoSize = true;
            this.panelTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panelTitle.Location = new System.Drawing.Point(184, 14);
            this.panelTitle.Name = "panelTitle";
            this.panelTitle.Size = new System.Drawing.Size(51, 16);
            this.panelTitle.TabIndex = 3;
            this.panelTitle.Text = "label1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(742, 466);
            this.Controls.Add(this.panelTitle);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.panel2);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(750, 500);
            this.MinimumSize = new System.Drawing.Size(750, 500);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Shown += new System.EventHandler(this.Form1_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.eventLogSensFile)).EndInit();
            this.contextMenuStrip1.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        TaskbarNotifier taskbarNotifier1; 
        private System.Diagnostics.EventLog eventLogSensFile;
        private ContextMenuStrip contextMenuStrip1;
        private ToolStripMenuItem openConfigurationMenuToolStripMenuItem;
        private ToolStripMenuItem disableOnLineScanningToolStripMenuItem;
        private ToolStripMenuItem enableOnLineScannigToolStripMenuItem;
        private Panel panel1;
        private LinkLabel menuDefSensData;
        private LinkLabel menuScan;
        private LinkLabel menuOptions;
        private LinkLabel menuOnlineScan;
        private Panel panel2;
        private Label panelTitle;
    }
}

