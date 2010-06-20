using CustomUIControls;
using System.Windows.Forms;

namespace SysTrayTest
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
            ((System.ComponentModel.ISupportInitialize)(this.eventLogSensFile)).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
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
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "Form1";
            this.WindowState = System.Windows.Forms.FormWindowState.Minimized;
            this.Shown += new System.EventHandler(this.Form1_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.eventLogSensFile)).EndInit();
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        TaskbarNotifier taskbarNotifier1; 
        private System.Diagnostics.EventLog eventLogSensFile;
        private ContextMenuStrip contextMenuStrip1;
        private ToolStripMenuItem openConfigurationMenuToolStripMenuItem;
        private ToolStripMenuItem disableOnLineScanningToolStripMenuItem;
        private ToolStripMenuItem enableOnLineScannigToolStripMenuItem;
    }
}

