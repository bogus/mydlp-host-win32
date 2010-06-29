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
    public partial class Options : UserControl
    {
        public Options()
        {
            InitializeComponent();
        }

        private void Options_Load(object sender, EventArgs e)
        {
            tabAppOptions.Text = Form1.resM.GetString("options.taboptions");
            label3.Text = Form1.resM.GetString("options.taboptions.definition");
            label1.Text = Form1.resM.GetString("options.taboptions.001");
            label2.Text = Form1.resM.GetString("options.taboptions.002");
            groupBox1.Text = Form1.resM.GetString("options.taboptions.003");

            if (Form1.serviceConf.language == "tr-TR")
                radioButton2.Checked = true;
            else
                radioButton1.Checked = true;
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

            MyDLPRemoteServiceConf.Serialize();
        }
    }
}
