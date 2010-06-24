using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace MydlpWinGui
{
    public partial class SensitiveData : UserControl
    {
        public SensitiveData()
        {
            InitializeComponent();
        }

        private void buttonMd5Browse_Click(object sender, EventArgs e)
        {
            //openFileDialog1.InitialDirectory = System.Environment.SpecialFolder.Desktop;
            openFileDialog1.Multiselect = false;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog1.FileName;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {

        }

        private void button4_Click(object sender, EventArgs e)
        {

        }

        private void button5_Click(object sender, EventArgs e)
        {

        }

        private void buttonSaveAll_Click(object sender, EventArgs e)
        {

        }

        private void SensitiveData_Load(object sender, EventArgs e)
        {
            tabPage1.Text = Form1.resM.GetString("def.tabmd5");
            tabPage2.Text = Form1.resM.GetString("def.tabregex");
            tabPage3.Text = Form1.resM.GetString("def.tabpredefined");
        }
    }
}
