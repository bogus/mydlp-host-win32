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
using System.IO;
using System.Security.Cryptography;
using mydlpsf;

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
            if (textBox1.Text.Length != 0 && new FileInfo(textBox1.Text).Exists &&
                textBox2.Text.Length > 4 && textBox2.Text.Length < 15 &&
                !textBox2.Text.Contains(" ") && !textBox2.Text.Contains(":"))
            {
                String md5Value = GetMD5HashFromFile(textBox1.Text);
                String desc = textBox2.Text.ToUpper();
                String fileSize = new FileInfo(textBox1.Text).Length.ToString();
                ListViewItem item = new ListViewItem();
                item.Text = desc;
                item.SubItems.Add(fileSize);
                item.SubItems.Add(md5Value);
                listView1.Items.Add(item);
                md5Value += ":" + fileSize + ":" + desc;
                if (Form1.sensFileConf.md5Val.Length != 0)
                    Form1.sensFileConf.md5Val += "\n";
                Form1.sensFileConf.md5Val += md5Value;
                textBox1.Text = "";
                textBox2.Text = "";
            }
            else
            {
                MessageBox.Show(Form1.resM.GetString("input.warn.text"),
                    Form1.resM.GetString("input.warn.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count > 0)
            {
                String[] md5List = Form1.sensFileConf.md5Val.Split(new char[] { '\n' });
                String selectedMd5 = listView1.SelectedItems[0].SubItems[1].Text;
                String newMd5List = String.Empty;
                foreach (String str in md5List)
                {
                    if (str.Contains(selectedMd5))
                    {
                        continue;
                    }
                    else
                    {
                        newMd5List += str + "\n";
                    }
                }
                listView1.Items.RemoveAt(listView1.SelectedIndices[0]);
                Form1.sensFileConf.md5Val = newMd5List;
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (textBox3.Text.Length != 0)
            {
                MyDLPClamRegex clamRegex = new MyDLPClamRegex();
                clamRegex.regex = textBox3.Text;
                clamRegex.id = (uint)new Random().Next();
                Form1.sensFileConf.regexVal.Add(clamRegex);
                ListViewItem item = new ListViewItem();
                item.Text = clamRegex.regex;
                item.SubItems.Add(clamRegex.id.ToString());
                listViewRegex.Items.Add(item);
                textBox3.Text = "";
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (listViewRegex.SelectedItems.Count > 0)
            {
                MyDLPClamRegex clamRegex = new MyDLPClamRegex();
                clamRegex.id = UInt32.Parse(listViewRegex.SelectedItems[0].SubItems[1].Text);
                
                foreach (MyDLPClamRegex tmpClamRegex in Form1.sensFileConf.regexVal)
                {
                    if (tmpClamRegex.id == clamRegex.id)
                    {
                        Form1.sensFileConf.regexVal.Remove(tmpClamRegex);
                        break; 
                    }
                }
                listViewRegex.Items.RemoveAt(listViewRegex.SelectedIndices[0]);
            }
        }

        private void buttonSaveAll_Click(object sender, EventArgs e)
        {
            try
            {
                MyDLPRemoteSensFileConf.GetInstance().enableCC = checkBoxCC.Checked;
                MyDLPRemoteSensFileConf.GetInstance().enableSSN = checkBoxSSN.Checked;
                MyDLPRemoteSensFileConf.GetInstance().enableIBAN = checkBoxIBAN.Checked;
                MyDLPRemoteSensFileConf.GetInstance().enableTRId = checkBoxTRid.Checked;
                MyDLPRemoteSensFileConf.GetInstance().maxCCCount = UInt32.Parse(maskedTextBoxCC.Text);
                MyDLPRemoteSensFileConf.GetInstance().maxSSNCount = UInt32.Parse(maskedTextBoxSSN.Text);
                MyDLPRemoteSensFileConf.GetInstance().maxTRIdCount = UInt32.Parse(maskedTextBoxTRid.Text);
                MyDLPRemoteSensFileConf.GetInstance().maxIBANCount = UInt32.Parse(maskedTextBoxIBAN.Text);
                MyDLPRemoteSensFileConf.GetInstance().enableRegex = checkBox1.Checked;
                MyDLPRemoteSensFileConf.GetInstance().blockEncrypted = checkBox2.Checked;

                MyDLPRemoteSensFileConf.Serialize();

                MessageBox.Show(Form1.resM.GetString("save.text"),
                    Form1.resM.GetString("save.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show(Form1.resM.GetString("save.error.text"),
                    Form1.resM.GetString("save.error.caption"),
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void SensitiveData_Load(object sender, EventArgs e)
        {
            Fill();
            Globalize();
        }

        public void Fill()
        {
            openFileDialog1.FileName = "";

            FillMd5List();
            FillRegexList();

            checkBoxCC.Checked = Form1.sensFileConf.enableCC;
            checkBoxSSN.Checked = Form1.sensFileConf.enableSSN;
            checkBoxTRid.Checked = Form1.sensFileConf.enableTRId;
            checkBoxIBAN.Checked = Form1.sensFileConf.enableIBAN;
            maskedTextBoxCC.Text = Form1.sensFileConf.maxCCCount.ToString();
            maskedTextBoxSSN.Text = Form1.sensFileConf.maxSSNCount.ToString();
            maskedTextBoxTRid.Text = Form1.sensFileConf.maxTRIdCount.ToString();
            maskedTextBoxIBAN.Text = Form1.sensFileConf.maxIBANCount.ToString();

            checkBox2.Checked = Form1.sensFileConf.blockEncrypted;
        }

        public void Globalize()
        {
            tabPage1.Text = Form1.resM.GetString("def.tabmd5");
            tabPage2.Text = Form1.resM.GetString("def.tabregex");
            tabPage3.Text = Form1.resM.GetString("def.tabpredefined");
            buttonSaveAll.Text = Form1.resM.GetString("app.savechanges");

            // first tab
            groupAddMd5.Text = Form1.resM.GetString("def.tabmd5.001");
            label1.Text = Form1.resM.GetString("def.tabmd5.definition");
            label2.Text = Form1.resM.GetString("def.tabmd5.005");
            label3.Text = Form1.resM.GetString("def.tabmd5.003");
            buttonMd5Browse.Text = Form1.resM.GetString("def.tabmd5.002");
            button2.Text = Form1.resM.GetString("def.tabmd5.004");
            label4.Text = Form1.resM.GetString("def.tabmd5.006");
            listView1.Columns[0].Text = Form1.resM.GetString("def.tabmd5.007");
            listView1.Columns[1].Text = Form1.resM.GetString("def.tabmd5.008");
            listView1.Columns[2].Text = Form1.resM.GetString("def.tabmd5.009");
            button3.Text = Form1.resM.GetString("def.tabmd5.010");

            // second tab
            groupRegex.Text = Form1.resM.GetString("def.tabregex.006");
            label5.Text = Form1.resM.GetString("def.tabregex.definition");
            label6.Text = Form1.resM.GetString("def.tabregex.001");
            label7.Text = Form1.resM.GetString("def.tabregex.004");
            button4.Text = Form1.resM.GetString("def.tabregex.002");
            button5.Text = Form1.resM.GetString("def.tabregex.003");
            listViewRegex.Columns[0].Text = Form1.resM.GetString("def.tabregex.005");
            checkBox1.Text = Form1.resM.GetString("def.tabregex.007");

            // third tab
            label12.Text = Form1.resM.GetString("def.tabpredefined.definition");
            checkBoxCC.Text = Form1.resM.GetString("def.tabpredefined.001");
            checkBoxSSN.Text = Form1.resM.GetString("def.tabpredefined.002");
            checkBoxTRid.Text = Form1.resM.GetString("def.tabpredefined.003");
            checkBoxIBAN.Text = Form1.resM.GetString("def.tabpredefined.004");
            groupBoxPredefined.Text = Form1.resM.GetString("def.tabpredefined.009");
            label8.Text = Form1.resM.GetString("def.tabpredefined.005");
            label9.Text = Form1.resM.GetString("def.tabpredefined.006");
            label10.Text = Form1.resM.GetString("def.tabpredefined.007");
            label11.Text = Form1.resM.GetString("def.tabpredefined.008");
            groupBox1.Text = Form1.resM.GetString("def.tabpredefined.010");
            checkBox2.Text = Form1.resM.GetString("def.tabpredefined.011");
        }

        private void FillMd5List()
        {
            listView1.Items.Clear();
            String[] md5List = Form1.sensFileConf.md5Val.Split(new char[] { '\n' });

            foreach (String str in md5List)
            {
                String[] md5InnerArr = str.Split(new char[] { ':' });
                if (md5InnerArr.Length == 3)
                {
                    ListViewItem item = new ListViewItem();
                    item.Text = md5InnerArr[2];
                    item.SubItems.Add(md5InnerArr[1]);
                    item.SubItems.Add(md5InnerArr[0]);
                    listView1.Items.Add(item);
                }
            }
        }

        private void FillRegexList()
        {
            listViewRegex.Items.Clear();
            foreach (MyDLPClamRegex clamRegex in Form1.sensFileConf.regexVal)
            {
                ListViewItem item = new ListViewItem();
                item.Text = clamRegex.regex;
                item.SubItems.Add(clamRegex.id.ToString());
                listViewRegex.Items.Add(item);
            }

            checkBox1.Checked = Form1.sensFileConf.enableRegex;

        }

        protected string GetMD5HashFromFile(string fileName)
        {
            FileStream file = new FileStream(fileName, FileMode.Open);
            MD5 md5 = new MD5CryptoServiceProvider();
            byte[] retVal = md5.ComputeHash(file);
            file.Close();

            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < retVal.Length; i++)
            {
                sb.Append(retVal[i].ToString("x2"));
            }
            return sb.ToString();
        }
    }
}
