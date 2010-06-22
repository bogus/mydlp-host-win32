using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace MydlpWinGui
{
    public partial class DefineSensitiveData : UserControl
    {
        public DefineSensitiveData()
        {
            InitializeComponent();
        }

        private void UserControl1_Load(object sender, EventArgs e)
        {
            tabPage1.Text = Form1.resM.GetString("def.tabmd5");
            tabPage2.Text = Form1.resM.GetString("def.tabregex");
            tabPage3.Text = Form1.resM.GetString("def.tabpredefined");
        }
    }
}
