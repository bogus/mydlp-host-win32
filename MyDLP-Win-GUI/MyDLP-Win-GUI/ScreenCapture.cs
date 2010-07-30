/* 
 *  Copyright (C) 2010 Medra Teknoloji
 *
 *  Authors: Burak OGUZ <burak@medra.com.tr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */
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
    public partial class ScreenCapture : UserControl
    {
        public ScreenCapture()
        {
            if (MyDLPRemoteScreenCaptureConf.GetInstance().enableScreenCaptureFilter)
                MyDLPScreenCaptureFilter.GetInstance().StartHook();
            
            InitializeComponent();
        }

        private void ScreenCapture_Load(object sender, EventArgs e)
        {

            Fill();

            Globalize();
        }

        public void Fill()
        {
            checkBox1.Checked = Form1.screenCaptureConf.enableScreenCaptureFilter;
            checkBox2.Checked = Form1.screenCaptureConf.forbidMSOffice;
            checkBox3.Checked = Form1.screenCaptureConf.forbidOOOrg;
            checkBox4.Checked = Form1.screenCaptureConf.forbidAcrobatReader;
            checkBox5.Checked = Form1.screenCaptureConf.forbidPhotoshop;
            checkBox6.Checked = Form1.screenCaptureConf.forbidAutoCAD;
        }

        public void Globalize()
        {
            tabPage1.Text = Form1.resM.GetString("screen.taboptions");
            groupBox1.Text = Form1.resM.GetString("screen.taboptions.002");
            label1.Text = Form1.resM.GetString("screen.taboptions.definition");
            checkBox1.Text = Form1.resM.GetString("screen.taboptions.001");
            button1.Text = Form1.resM.GetString("app.savechanges");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                MyDLPRemoteScreenCaptureConf.GetInstance().enableScreenCaptureFilter = checkBox1.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidMSOffice = checkBox2.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidOOOrg = checkBox3.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidAcrobatReader = checkBox4.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidPhotoshop = checkBox5.Checked;
                MyDLPRemoteScreenCaptureConf.GetInstance().forbidAcrobatReader = checkBox6.Checked;

                MyDLPRemoteScreenCaptureConf.Serialize();

                Form1.form1Instance.checkServiceStatus(button1);

                MessageBox.Show(Form1.resM.GetString("save.text"),
                    Form1.resM.GetString("save.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                if (MyDLPRemoteScreenCaptureConf.GetInstance().enableScreenCaptureFilter)
                    MyDLPScreenCaptureFilter.GetInstance().StartHook();
                else
                    MyDLPScreenCaptureFilter.GetInstance().StopHook();
            }
            catch (Exception ex)
            {
                MessageBox.Show(Form1.resM.GetString("save.error.text"),
                    Form1.resM.GetString("save.error.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
