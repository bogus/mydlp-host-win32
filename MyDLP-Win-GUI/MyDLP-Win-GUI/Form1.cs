/* 
 *  Copyright (C) 2010 Medra Teknoloji
 *
 *  Authors: Burak OGUZ <burak@medra.com.tr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Resources;
using System.Threading;
using System.Globalization;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.ServiceProcess;
using CustomUIControls;
using mydlpsf;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;

namespace MydlpWinGui
{
    public partial class Form1 : Form
    {

        public static ResourceManager resM = new ResourceManager("MydlpWinGui.languages.Resource1", Type.GetType("MydlpWinGui.Form1").Assembly);
        public static MyDLPRemoteSensFileConf sensFileConf;
        public static MyDLPRemoteDeviceConf deviceConf;
        public static MyDLPRemoteScreenCaptureConf screenCaptureConf;
        public static MyDLPRemoteServiceConf serviceConf;
        public static Form1 form1Instance;
        private SensitiveData defineSensitiveData = new SensitiveData();
        private LocalScan localScan;
        private OnlineScan onlineScan;
        private ScreenCapture screenCapture;
        private Options options;
        private About aboutUs;
        private Control currentPanelControl;
        private Button saveButton;

        public Form1()
        {
            form1Instance = this;

            // Load sensitive data configuration
            MyDLPRemoteSensFileConf.Deserialize();
            MyDLPRemoteDeviceConf.Deserialize();
            MyDLPRemoteScreenCaptureConf.Deserialize();
            MyDLPRemoteServiceConf.Deserialize();
            sensFileConf = MyDLPRemoteSensFileConf.GetInstance();
            deviceConf = MyDLPRemoteDeviceConf.GetInstance();
            screenCaptureConf = MyDLPRemoteScreenCaptureConf.GetInstance();
            serviceConf = MyDLPRemoteServiceConf.GetInstance();
            
            localScan = new LocalScan();
            onlineScan = new OnlineScan();
            screenCapture = new ScreenCapture();
            options = new Options();
            aboutUs = new About();

            if(!EventLog.SourceExists("MyDLPLogs-Warnings"))
			{
				EventLog.CreateEventSource("MyDLPLogs-Warnings", "MyDLPLog");
			}

            InitializeComponent();

            if (!IsAdministrator())
            {
                defineSensitiveData.Enabled = false;
                localScan.Enabled = false;
                onlineScan.Enabled = false;
                screenCapture.Enabled = false;
                options.Enabled = false;
                exitToolStripMenuItem.Enabled = false;
            }

            taskbarNotifier1 = new TaskbarNotifier();
            taskbarNotifier1.SetBackgroundBitmap(new Bitmap(GetType(), "images.skin.bmp"), Color.FromArgb(255, 0, 255));
            taskbarNotifier1.SetCloseBitmap(new Bitmap(GetType(), "images.close.bmp"), Color.FromArgb(255, 0, 255), new Point(127, 8));
            taskbarNotifier1.TitleRectangle = new Rectangle(40, 9, 70, 25);
            taskbarNotifier1.ContentRectangle = new Rectangle(8, 41, 133, 68);

            /*
            taskbarNotifier1.TitleClick += new EventHandler(TitleClick);
            taskbarNotifier1.ContentClick += new EventHandler(ContentClick);
            taskbarNotifier1.CloseClick += new EventHandler(CloseClick);
            */

            notifyIcon1.ContextMenuStrip = eventsMenuStrip;

            button1.Enabled = false;
            button1.Visible = false;
            button2.Enabled = false;
            button2.Visible = false;

            timer2.Start();
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Program.NativeMethods.WM_SHOWME)
            {
                ShowMe();
            }
            base.WndProc(ref m);
        }
        private void ShowMe()
        {
            if (WindowState == FormWindowState.Minimized)
            {
                Show();
                WindowState = FormWindowState.Normal;
            }
            
            bool top = TopMost;
            TopMost = true;
            TopMost = top;
        }


        private void notifyIcon1_MouseDoubleClick(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (WindowState == FormWindowState.Minimized)
            {
                Show();
                WindowState = FormWindowState.Normal;
            }
            else
            {
                WindowState = FormWindowState.Minimized;
                Hide();
            }
        }

        private void eventLogSensFile_EntryWritten(object sender, System.Diagnostics.EntryWrittenEventArgs e)
        {
            if (serviceConf.showPopupNotification)
            {
                taskbarNotifier1.CloseClickable = true;
                taskbarNotifier1.TitleClickable = false;
                taskbarNotifier1.ContentClickable = true;
                taskbarNotifier1.EnableSelectionRectangle = false;
                taskbarNotifier1.KeepVisibleOnMousOver = true;	// Added Rev 002
                taskbarNotifier1.ReShowOnMouseOver = false;			// Added Rev 002
                taskbarNotifier1.Show(String.Empty, e.Entry.Message, 300, 3000, 200);
            }
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            WindowState = FormWindowState.Minimized;
            Hide();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            WindowState = FormWindowState.Minimized;
            Hide();

            Thread.CurrentThread.CurrentUICulture = new CultureInfo(serviceConf.language);
            Thread.CurrentThread.CurrentCulture = new CultureInfo(serviceConf.language);

            this.Text = resM.GetString("app.title");

            listView1.Items[0].Text = resM.GetString("menu.defsensdata");
            listView1.Items[1].Text = resM.GetString("menu.localscan");
            listView1.Items[2].Text = resM.GetString("menu.onlinescan");
            listView1.Items[3].Text = resM.GetString("menu.screencapture");
            listView1.Items[4].Text = resM.GetString("menu.options");
            listView1.Items[5].Text = resM.GetString("menu.aboutus");

            panelTitle.Text = resM.GetString("menu.defsensdata");
            openConfigurationMenuToolStripMenuItem.Text = resM.GetString("rightclick.show");
            exitToolStripMenuItem.Text = resM.GetString("rightclick.exit");
            button1.Text = resM.GetString("button.runasadmin");
            button2.Text = resM.GetString("button.restart");

            panel2.Controls.Add(defineSensitiveData);
            currentPanelControl = defineSensitiveData;

            Show();
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView1.SelectedIndices.Count == 0)
                return;

            if (listView1.SelectedIndices[0] == 0)
            {
                panelTitle.Text = listView1.SelectedItems[0].Text;
                panel2.Controls.Remove(currentPanelControl);
                panel2.Controls.Add(defineSensitiveData);
                currentPanelControl = defineSensitiveData;
            }
            else if (listView1.SelectedIndices[0] == 1)
            {
                panelTitle.Text = listView1.SelectedItems[0].Text;
                panel2.Controls.Remove(currentPanelControl);
                panel2.Controls.Add(localScan);
                currentPanelControl = localScan;
            }
            else if (listView1.SelectedIndices[0] == 2)
            {
                panelTitle.Text = listView1.SelectedItems[0].Text;
                panel2.Controls.Remove(currentPanelControl);
                panel2.Controls.Add(onlineScan);
                currentPanelControl = onlineScan;
            }
            else if (listView1.SelectedIndices[0] == 3)
            {
                panelTitle.Text = listView1.SelectedItems[0].Text;
                panel2.Controls.Remove(currentPanelControl);
                panel2.Controls.Add(screenCapture);
                currentPanelControl = screenCapture;
            }
            else if (listView1.SelectedIndices[0] == 4)
            {
                panelTitle.Text = listView1.SelectedItems[0].Text;
                panel2.Controls.Remove(currentPanelControl);
                panel2.Controls.Add(options);
                currentPanelControl = options;
            }
            else if (listView1.SelectedIndices[0] == 5)
            {
                panelTitle.Text = listView1.SelectedItems[0].Text;
                panel2.Controls.Remove(currentPanelControl);
                panel2.Controls.Add(aboutUs);
                currentPanelControl = aboutUs;
            }
        }

        public void ReloadAll()
        {
            this.Text = resM.GetString("app.title");

            listView1.Items[0].Text = resM.GetString("menu.defsensdata");
            listView1.Items[1].Text = resM.GetString("menu.localscan");
            listView1.Items[2].Text = resM.GetString("menu.onlinescan");
            listView1.Items[3].Text = resM.GetString("menu.screencapture");
            listView1.Items[4].Text = resM.GetString("menu.options");
            listView1.Items[5].Text = resM.GetString("menu.aboutus");

            panelTitle.Text = resM.GetString("menu.options");

            openConfigurationMenuToolStripMenuItem.Text = resM.GetString("rightclick.show");
            exitToolStripMenuItem.Text = resM.GetString("rightclick.exit");
            button1.Text = resM.GetString("button.runasadmin");
            button2.Text = resM.GetString("button.restart");

            defineSensitiveData.Globalize();
            localScan.Globalize();
            onlineScan.Globalize();
            screenCapture.Globalize();
            options.Globalize();
            aboutUs.Globalize();
        }

        public void ReloadConfiguration()
        {
            MyDLPRemoteSensFileConf.Deserialize();
            MyDLPRemoteDeviceConf.Deserialize();
            MyDLPRemoteScreenCaptureConf.Deserialize();
            MyDLPRemoteServiceConf.Deserialize();
            sensFileConf = MyDLPRemoteSensFileConf.GetInstance();
            deviceConf = MyDLPRemoteDeviceConf.GetInstance();
            screenCaptureConf = MyDLPRemoteScreenCaptureConf.GetInstance();
            serviceConf = MyDLPRemoteServiceConf.GetInstance();

            defineSensitiveData.Fill();
            localScan.Fill();
            onlineScan.Fill();
            screenCapture.Fill();
            options.Fill();
        }

        private void openConfigurationMenuToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;    
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing 
                || e.CloseReason == CloseReason.TaskManagerClosing)
            {
                this.Hide();
                e.Cancel = true;
            }
        }

        internal bool IsAdministrator()
        {
            WindowsIdentity identity = WindowsIdentity.GetCurrent();
            WindowsPrincipal principal = new WindowsPrincipal(identity);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(DialogResult.Yes == MessageBox.Show(Form1.resM.GetString("exit.text"),
                    Form1.resM.GetString("exit.caption"),
                    MessageBoxButtons.YesNo, MessageBoxIcon.Question))
                Application.Exit();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int processId = Process.GetCurrentProcess().Id;
            String args = "--restart-admin " + Convert.ToString(processId);
            Process.Start(Application.ExecutablePath, args);
            Application.Exit();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            int processId = Process.GetCurrentProcess().Id;
            String args = "--restart " + Convert.ToString(processId);
            Process.Start(Application.ExecutablePath, args);
            Application.Exit();
        }

        public void checkServiceStatus(Button button)
        {
            timer1.Enabled = true;
            timer1.Start();
            saveButton = button;
            saveButton.Enabled = false;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Stop();
            timer1.Enabled = false;
            saveButton.Enabled = true;
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            /*foreach (ServiceController sc in ServiceController.GetServices())
            {
                if (sc.ServiceName.Contains("MyDLP"))
                {
                    if (sc.Status != ServiceControllerStatus.Running)
                        sc.Start();
                }
            }*/
        }
    }
}
