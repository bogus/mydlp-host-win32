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
            Fill();
            Globalize();
        }

        public void Fill()
        {
            if (Form1.serviceConf.language == "tr-TR")
                radioButton2.Checked = true;
            else
                radioButton1.Checked = true;

            if (Form1.serviceConf.showPopupNotification)
                checkBox1.Checked = true;
            else
                checkBox1.Checked = false;
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

            groupBox2.Text = Form1.resM.GetString("options.taboptions.006");
            label5.Text = Form1.resM.GetString("options.taboptions.007");
            label6.Text = Form1.resM.GetString("options.taboptions.008");
            button2.Text = Form1.resM.GetString("options.taboptions.009");
            button4.Text =  Form1.resM.GetString("options.taboptions.009");
            button3.Text = Form1.resM.GetString("options.taboptions.011");
            button1.Text = Form1.resM.GetString("options.taboptions.010"); 
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

        private void button4_Click(object sender, EventArgs e)
        {
            saveFileDialog1.ShowDialog();
            if (saveFileDialog1.FileName != "")
            {
                textBox2.Text = saveFileDialog1.FileName;
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                if (textBox2.Text.Length != 0)
                {
                    ZipUtil.ZipFiles(textBox2.Text);
                    MessageBox.Show(Form1.resM.GetString("options.taboptions.014"),
                        Form1.resM.GetString("options.taboptions.012"),
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(Form1.resM.GetString("options.taboptions.015"),
                    Form1.resM.GetString("options.taboptions.012"),
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog1.FileName;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                if (textBox1.Text.Length != 0)
                {
                    ZipUtil.UnZipFiles(textBox1.Text);
                    Form1.form1Instance.ReloadConfiguration();
                    MessageBox.Show(Form1.resM.GetString("options.taboptions.013"),
                        Form1.resM.GetString("options.taboptions.012"),
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(Form1.resM.GetString("options.taboptions.015") + ex.StackTrace,
                    Form1.resM.GetString("options.taboptions.012"),
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }
    }
}
