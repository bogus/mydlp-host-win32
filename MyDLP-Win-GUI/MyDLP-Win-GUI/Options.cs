using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using mydlpsf;
using System.Threading;
using System.Globalization;
using System.Resources;

namespace MydlpWinGui
{
    public partial class Options : UserControl
    {
        public Options()
        {
            InitializeComponent();
        }

        private void Options_Load(object sender, EventArgs e)
        {
            if (Form1.serviceConf.language == "tr-TR")
                radioButton2.Checked = true;
            else
                radioButton1.Checked = true;

            if (Form1.serviceConf.showPopupNotification)
                checkBox1.Checked = true;
            else
                checkBox1.Checked = false;

            Globalize();
        }

        public void Globalize()
        {
            tabAppOptions.Text = Form1.resM.GetString("options.taboptions");
            label3.Text = Form1.resM.GetString("options.taboptions.definition");
            label1.Text = Form1.resM.GetString("options.taboptions.001");
            label2.Text = Form1.resM.GetString("options.taboptions.002");
            groupBox1.Text = Form1.resM.GetString("options.taboptions.003");
            label4.Text = Form1.resM.GetString("options.taboptions.004");
            checkBox1.Text = Form1.resM.GetString("options.taboptions.005");
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton2.Checked)
            {
                MyDLPRemoteServiceConf.GetInstance().language = "tr-TR";
            }
            else
            {
                MyDLPRemoteServiceConf.GetInstance().language = "en-US";
            }

            Thread.CurrentThread.CurrentUICulture = new CultureInfo(Form1.serviceConf.language);
            Thread.CurrentThread.CurrentCulture = new CultureInfo(Form1.serviceConf.language);
            Form1.resM = new ResourceManager("MydlpWinGui.languages.Resource1", Type.GetType("MydlpWinGui.Form1").Assembly);
            Form1.form1Instance.ReloadAll();

            MyDLPRemoteServiceConf.Serialize();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
            {
                MyDLPRemoteServiceConf.GetInstance().showPopupNotification = true;
            }
            else
            {
                MyDLPRemoteServiceConf.GetInstance().showPopupNotification = false;
            }
            MyDLPRemoteServiceConf.Serialize();
        }
    }
}
