using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace MydlpWinGui
{
    public partial class LocalScan : UserControl
    {
        public LocalScan()
        {
            InitializeComponent();
        }

        private void LocalScan_Load(object sender, EventArgs e)
        {
            tabPage1.Text = Form1.resM.GetString("scan.tabmanualscan");
            tabPage2.Text = Form1.resM.GetString("scan.tabscanoptions");
            tabPage3.Text = Form1.resM.GetString("scan.tabscanhistory");
        }
    }
}
