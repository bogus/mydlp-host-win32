using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using mydlpsf;
using System.IO;

namespace MydlpWinGui
{
    public partial class OnlineScan : UserControl
    {
        public OnlineScan()
        {
            InitializeComponent();
        }

        private void OnlineScan_Load(object sender, EventArgs e)
        {
            tabOnlineOptions.Text = Form1.resM.GetString("online.tabonlinescanopt");
            tabOnlineScanHistory.Text = Form1.resM.GetString("online.tabonlinescanhist");

            // First tab
            groupBox1.Text = Form1.resM.GetString("online.tabonlinescanopt.001");
            label1.Text = Form1.resM.GetString("online.tabonlinescanopt.002");
            checkBoxEnableScan.Text = Form1.resM.GetString("online.tabonlinescanopt.003");
            checkBoxJustLog.Text = Form1.resM.GetString("online.tabonlinescanopt.004");
            button1.Text = Form1.resM.GetString("app.savechanges");
            // second tab
            label2.Text = Form1.resM.GetString("online.tabonlinescanhist.definition");

            checkBoxEnableScan.Checked = Form1.deviceConf.enableRemovableOnlineScanning;
            checkBoxJustLog.Checked = Form1.deviceConf.justLogRemovableOnlineScanning;

            ReadRemovableLogFile();
            timer1.Start();

        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                MyDLPRemoteDeviceConf.GetInstance().enableRemovableOnlineScanning = checkBoxEnableScan.Checked;
                MyDLPRemoteDeviceConf.GetInstance().justLogRemovableOnlineScanning = checkBoxJustLog.Checked;

                MyDLPRemoteDeviceConf.Serialize();

                MessageBox.Show("Changes Saved", "Success");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Operation failed!", "Error");
            }
        }

        private void ReadRemovableLogFile()
        {
        
            try
            {
                String str = MyDLPEventLogger.GetInstance().removableLogPath;
                FileStream fs = File.Open(str, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                StreamReader reader = new StreamReader(fs);
                //StreamReader reader = new StreamReader(File.OpenRead(str));
                textBox1.Clear();

                String curLine;
                for (int i = 0; i < 100 && (curLine = reader.ReadLine()) != null; i++)
                {
                    textBox1.Text += curLine + Environment.NewLine;
                }

                reader.Close();
                //fs.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            ReadRemovableLogFile();
        }
    }
}
