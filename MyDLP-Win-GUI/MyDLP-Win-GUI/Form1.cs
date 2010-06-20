using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CustomUIControls;

namespace SysTrayTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

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

            notifyIcon1.ContextMenuStrip = contextMenuStrip1;
        }

        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
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
            taskbarNotifier1.CloseClickable = true;
            taskbarNotifier1.TitleClickable = false;
            taskbarNotifier1.ContentClickable = true;
            taskbarNotifier1.EnableSelectionRectangle = false;
            taskbarNotifier1.KeepVisibleOnMousOver = true;	// Added Rev 002
            taskbarNotifier1.ReShowOnMouseOver = false;			// Added Rev 002
            taskbarNotifier1.Show("MyDLP Event Caught", e.Entry.Message, 300, 3000, 200);
        }

        void CloseClick(object obj, EventArgs ea)
        {
            MessageBox.Show("Closed was Clicked");
        }

        void TitleClick(object obj, EventArgs ea)
        {
            MessageBox.Show("Title was Clicked");
        }

        void ContentClick(object obj, EventArgs ea)
        {
            MessageBox.Show("Content was Clicked");
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            WindowState = FormWindowState.Minimized;
            Hide();
        }
    }
}
