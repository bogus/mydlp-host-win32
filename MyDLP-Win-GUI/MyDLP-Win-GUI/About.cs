using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace MydlpWinGui
{
    public partial class About : UserControl
    {
        public About()
        {
            InitializeComponent();
        }

        public void Globalize()
        {
            label4.Text = Form1.resM.GetString("about.001");
            label1.Text = Form1.resM.GetString("about.002");
        }

        private void About_Load(object sender, EventArgs e)
        {
            Globalize();
        }
    }
}
