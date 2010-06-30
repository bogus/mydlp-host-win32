using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using mydlpsf;

namespace MydlpWinGui
{
    public partial class ScreenCapture : UserControl
    {
        public ScreenCapture()
        {
            InitializeComponent();
        }

        private void ScreenCapture_Load(object sender, EventArgs e)
        {

            checkBox1.Checked = Form1.screenCaptureConf.enableScreenCaptureFilter;
            checkBox2.Checked = Form1.screenCaptureConf.forbidMSOffice;
            checkBox3.Checked = Form1.screenCaptureConf.forbidOOOrg;
            checkBox4.Checked = Form1.screenCaptureConf.forbidAcrobatReader;
            checkBox5.Checked = Form1.screenCaptureConf.forbidPhotoshop;
            checkBox6.Checked = Form1.screenCaptureConf.forbidAutoCAD;

            Globalize();

        }

        public void Globalize()
        {
            tabPage1.Text = Form1.resM.GetString("screen.taboptions");
            groupBox1.Text = Form1.resM.GetString("screen.taboptions.002");
            label1.Text = Form1.resM.GetString("screen.taboptions.definition");
            checkBox1.Text = Form1.resM.GetString("screen.taboptions.001");
            button1.Text = Form1.resM.GetString("app.savechanges");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                MyDLPRemoteScreenCaptureConf.GetInstance().enableScreenCaptureFilter = checkBox1.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidMSOffice = checkBox2.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidOOOrg = checkBox3.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidAcrobatReader = checkBox4.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidPhotoshop = checkBox5.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidAcrobatReader = checkBox6.Checked;

                MyDLPRemoteScreenCaptureConf.Serialize();

                MessageBox.Show("Changes Saved", "Success");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Operation failed!", "Error");
            }
        }
    }
}
