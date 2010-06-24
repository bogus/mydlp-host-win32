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
    }
}
