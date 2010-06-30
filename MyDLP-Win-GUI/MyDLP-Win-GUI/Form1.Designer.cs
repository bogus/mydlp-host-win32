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
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem(new string[] {
            "Define Sensitive Data"}, 4, System.Drawing.Color.Empty, System.Drawing.Color.Empty, new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem(new string[] {
            "Local Scan"}, 0, System.Drawing.Color.Empty, System.Drawing.Color.Empty, new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem(new string[] {
            "Online Scan"}, 2, System.Drawing.Color.Empty, System.Drawing.Color.Empty, new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem(new string[] {
            "Screen Capture"}, 3, System.Drawing.Color.Empty, System.Drawing.Color.Empty, new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem(new string[] {
            "Options",
            ""}, 1, System.Drawing.Color.Empty, System.Drawing.Color.Empty, new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem(new string[] {
            "About Us"}, 5, System.Drawing.Color.Empty, System.Drawing.Color.Empty, new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.eventLogSensFile = new System.Diagnostics.EventLog();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.openConfigurationMenuToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.disableOnLineScanningToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.enableOnLineScannigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
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
            this.panel1.BackColor = System.Drawing.SystemColors.Window;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.listView1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(178, 466);
            this.panel1.TabIndex = 1;
            // 
            // listView1
            // 
            this.listView1.AutoArrange = false;
            this.listView1.BackColor = System.Drawing.SystemColors.Window;
            this.listView1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            listViewItem1.IndentCount = 1;
            listViewItem2.IndentCount = 2;
            listViewItem3.IndentCount = 3;
            listViewItem4.IndentCount = 4;
            listViewItem5.IndentCount = 5;
            this.listView1.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2,
            listViewItem3,
            listViewItem4,
            listViewItem5,
            listViewItem6});
            this.listView1.LargeImageList = this.imageList1;
            this.listView1.Location = new System.Drawing.Point(12, 13);
            this.listView1.Margin = new System.Windows.Forms.Padding(10);
            this.listView1.MultiSelect = false;
            this.listView1.Name = "listView1";
            this.listView1.Scrollable = false;
            this.listView1.Size = new System.Drawing.Size(156, 403);
            this.listView1.TabIndex = 5;
            this.listView1.TileSize = new System.Drawing.Size(150, 50);
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Tile;
            this.listView1.SelectedIndexChanged += new System.EventHandler(this.listView1_SelectedIndexChanged);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "localscan.png");
            this.imageList1.Images.SetKeyName(1, "options.png");
            this.imageList1.Images.SetKeyName(2, "removable.png");
            this.imageList1.Images.SetKeyName(3, "screencapt.png");
            this.imageList1.Images.SetKeyName(4, "sensfile.png");
            this.imageList1.Images.SetKeyName(5, "access.png");
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
        private Panel panel2;
        private Label panelTitle;
        private ListView listView1;
        private ImageList imageList1;
        private ColumnHeader columnHeader1;
    }
}

