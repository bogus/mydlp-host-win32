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
                Form1.sensFileConf.md5Val += "\n" + md5Value;
            }
            else
            {
                MessageBox.Show("Input is not validated!!");
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
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (listViewRegex.SelectedItems.Count > 0)
            {
                MyDLPClamRegex clamRegex = new MyDLPClamRegex();
                clamRegex.id = UInt32.Parse(listViewRegex.SelectedItems[0].SubItems[2].Text);
                
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

        }

        private void SensitiveData_Load(object sender, EventArgs e)
        {
            tabPage1.Text = Form1.resM.GetString("def.tabmd5");
            tabPage2.Text = Form1.resM.GetString("def.tabregex");
            tabPage3.Text = Form1.resM.GetString("def.tabpredefined");

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

        }

        private void FillMd5List()
        {
            String[] md5List = Form1.sensFileConf.md5Val.Split(new char[] { '\n' });

            foreach (String str in md5List)
            {
                String[] md5InnerArr = str.Split(new char[] { ':' });
                ListViewItem item = new ListViewItem();
                item.Text = md5InnerArr[2];
                item.SubItems.Add(md5InnerArr[1]);
                item.SubItems.Add(md5InnerArr[0]);
                listView1.Items.Add(item);
            }
        }

        private void FillRegexList()
        {
            foreach (MyDLPClamRegex clamRegex in Form1.sensFileConf.regexVal)
            {
                ListViewItem item = new ListViewItem();
                item.Text = clamRegex.regex;
                item.SubItems.Add(clamRegex.id.ToString());
                listViewRegex.Items.Add(item);
            }
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
