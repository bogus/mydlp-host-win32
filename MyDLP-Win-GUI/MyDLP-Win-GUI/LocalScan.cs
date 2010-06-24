using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.IO;
using System.Windows.Forms;
using mydlpsf;

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

            // first tab
            groupBox1.Text = Form1.resM.GetString("scan.tabmanualscan.001");
            groupBox2.Text = Form1.resM.GetString("scan.tabmanualscan.002");
            label1.Text = Form1.resM.GetString("scan.tabmanualscan.003");
            label2.Text = Form1.resM.GetString("scan.tabmanualscan.004");
            buttonScanAll.Text = Form1.resM.GetString("scan.tabmanualscan.001");
            buttonScanDir.Text = Form1.resM.GetString("scan.tabmanualscan.002");
            buttonDir.Text = Form1.resM.GetString("scan.tabmanualscan.005");
            buttonStopScan.Text = Form1.resM.GetString("scan.tabmanualscan.006");
            label3.Text = Form1.resM.GetString("scan.tabmanualscan.007");

            //second tab
            groupBox3.Text = Form1.resM.GetString("scan.tabscanoptions.001");
            groupBox4.Text = Form1.resM.GetString("scan.tabscanoptions.007");
            label4.Text = Form1.resM.GetString("scan.tabscanoptions.002");
            button1.Text = Form1.resM.GetString("scan.tabscanoptions.003");
            buttonAddExcluded.Text = Form1.resM.GetString("scan.tabscanoptions.004");
            button2.Text = Form1.resM.GetString("scan.tabscanoptions.005");
            listView1.Columns[0].Text = Form1.resM.GetString("scan.tabscanoptions.006");
            label5.Text = Form1.resM.GetString("scan.tabscanoptions.008");
            checkBoxScanUSB.Text = Form1.resM.GetString("scan.tabscanoptions.009");
            checkBoxScanCD.Text = Form1.resM.GetString("scan.tabscanoptions.010");
            buttonSave.Text = Form1.resM.GetString("app.savechanges");

            // third tab
            label6.Text = Form1.resM.GetString("scan.tabscanhistory.definition");
            listViewLog.Columns[0].Text = Form1.resM.GetString("scan.tabscanhistory.001");
            button3.Text = Form1.resM.GetString("scan.tabscanhistory.002");

            FillExcludedList();

            checkBoxScanUSB.Checked = Form1.deviceConf.scanPluggedInRemovableDevices;
            checkBoxScanCD.Checked = Form1.deviceConf.scanInsertedLogical;
        }

        private void FillExcludedList()
        {
            if (Form1.deviceConf.excludedDirs.Count != 0)
            {
                foreach (String str in Form1.deviceConf.excludedDirs)
                {
                    ListViewItem item = new ListViewItem();
                    item.Text = str;
                    listView1.Items.Add(item);
                }
            }
        }

        private void buttonScanAll_Click(object sender, EventArgs e)
        {
            // Add service routine for local drives scanning
            MyDLPSensFilePool.GetInstance().UpdatePool();

            progressBar1.Style = ProgressBarStyle.Marquee;
            progressBar1.MarqueeAnimationSpeed = 50;
            progressBar1.Value = 0;

            MyDLPDirectoryTraverse dirTraverse = new MyDLPDirectoryTraverse();
            dirTraverse.TraverseAllDrives();
            textBox4.DataBindings.Clear();
            textBox4.DataBindings.Add("Text", dirTraverse.detected, null);
        }

        private void buttonScanDir_Click(object sender, EventArgs e)
        {
            MyDLPSensFilePool.GetInstance().UpdatePool();

            progressBar1.Style = ProgressBarStyle.Marquee;
            progressBar1.MarqueeAnimationSpeed = 50;
            progressBar1.Value = 0;

            // Add service routine for directory scanning
            if (textBox1.Text.Length != 0 && Directory.Exists(textBox1.Text))
            {
                MyDLPDirectoryTraverse dirTraverse = new MyDLPDirectoryTraverse();
                dirTraverse.TraverseDir(textBox1.Text);
                textBox4.DataBindings.Clear();
                textBox4.DataBindings.Add("Text", dirTraverse.detected, null);
            }
            else
            {
                MessageBox.Show("Not a valid directory or empty field", "Warning");
            }
        }

        private void buttonDir_Click(object sender, EventArgs e)
        {
            this.folderBrowserDialog.RootFolder = System.Environment.SpecialFolder.Desktop;
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = folderBrowserDialog.SelectedPath;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.folderBrowserDialog2.RootFolder = System.Environment.SpecialFolder.Desktop;
            if (folderBrowserDialog2.ShowDialog() == DialogResult.OK)
            {
                textBox2.Text = folderBrowserDialog2.SelectedPath;
            }
        }

        private void buttonAddExcluded_Click(object sender, EventArgs e)
        {
            if (textBox2.Text.Length != 0)
            {
                ListViewItem item = new ListViewItem();
                item.Text = textBox2.Text;
                listView1.Items.Add(item);
                Form1.deviceConf.excludedDirs.Add(textBox2.Text);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (listView1.SelectedIndices.Count > 0)
            {
                if (Form1.deviceConf.excludedDirs.Count != 0)
                {
                    foreach (String str in Form1.deviceConf.excludedDirs)
                    {
                        if (str == listView1.SelectedItems[0].Text)
                        {
                            Form1.deviceConf.excludedDirs.Remove(str);
                            break;
                        }
                    }
                }
                listView1.Items.RemoveAt(listView1.SelectedIndices[0]);
            }
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            try
            {
                MyDLPRemoteDeviceConf.GetInstance().scanInsertedLogical = checkBoxScanCD.Checked;
                MyDLPRemoteDeviceConf.GetInstance().scanPluggedInRemovableDevices = checkBoxScanUSB.Checked;

                MyDLPRemoteDeviceConf.Serialize();

                MessageBox.Show("Changes Saved", "Success");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Operation Failed");
            }

        }

        private void buttonStopScan_Click(object sender, EventArgs e)
        {
            progressBar1.Style = ProgressBarStyle.Blocks;
            progressBar1.MarqueeAnimationSpeed = 0;
            progressBar1.Value = 0;

        }
    }
}
