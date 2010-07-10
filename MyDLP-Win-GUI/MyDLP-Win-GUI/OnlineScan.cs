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
            Fill();
            ReadRemovableLogFile();
            timer1.Start();

            Globalize();

        }

        public void Fill()
        {
            checkBoxEnableScan.Checked = Form1.deviceConf.enableRemovableOnlineScanning;
            checkBoxJustLog.Checked = Form1.deviceConf.justLogRemovableOnlineScanning;
            checkBox1.Checked = Form1.deviceConf.filterPSD;
            checkBox2.Checked = Form1.deviceConf.filterPSP;
            checkBox3.Checked = Form1.deviceConf.filterDWG;
            checkBox4.Checked = Form1.deviceConf.filterCDR;
        }

        public void Globalize()
        {
            tabOnlineOptions.Text = Form1.resM.GetString("online.tabonlinescanopt");
            tabOnlineScanHistory.Text = Form1.resM.GetString("online.tabonlinescanhist");

            // First tab
            groupBox1.Text = Form1.resM.GetString("online.tabonlinescanopt.001");
            groupBox2.Text = Form1.resM.GetString("online.tabonlinescanopt.005");
            label1.Text = Form1.resM.GetString("online.tabonlinescanopt.002");
            checkBoxEnableScan.Text = Form1.resM.GetString("online.tabonlinescanopt.003");
            checkBoxJustLog.Text = Form1.resM.GetString("online.tabonlinescanopt.004");
            button1.Text = Form1.resM.GetString("app.savechanges");
            // second tab
            label2.Text = Form1.resM.GetString("online.tabonlinescanhist.definition");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                MyDLPRemoteDeviceConf.GetInstance().enableRemovableOnlineScanning = checkBoxEnableScan.Checked;
                MyDLPRemoteDeviceConf.GetInstance().justLogRemovableOnlineScanning = checkBoxJustLog.Checked;

                MyDLPRemoteDeviceConf.GetInstance().filterPSD = checkBox1.Checked;
                MyDLPRemoteDeviceConf.GetInstance().filterPSP = checkBox2.Checked;
                MyDLPRemoteDeviceConf.GetInstance().filterCDR = checkBox4.Checked;
                MyDLPRemoteDeviceConf.GetInstance().filterDWG = checkBox3.Checked;

                MyDLPRemoteDeviceConf.Serialize();

                MessageBox.Show(Form1.resM.GetString("save.text"),
                    Form1.resM.GetString("save.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show(Form1.resM.GetString("save.error.text"),
                    Form1.resM.GetString("save.error.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
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
                fs.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message,
                    Form1.resM.GetString("error.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            ReadRemovableLogFile();
        }
    }
}
