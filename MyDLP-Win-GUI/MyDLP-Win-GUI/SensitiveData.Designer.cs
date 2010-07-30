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
namespace MydlpWinGui
{
    partial class SensitiveData
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.button3 = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
            this.groupAddMd5 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.button2 = new System.Windows.Forms.Button();
            this.buttonMd5Browse = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.button5 = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.listViewRegex = new System.Windows.Forms.ListView();
            this.columnRegex = new System.Windows.Forms.ColumnHeader();
            this.columnRegexId = new System.Windows.Forms.ColumnHeader();
            this.groupRegex = new System.Windows.Forms.GroupBox();
            this.button4 = new System.Windows.Forms.Button();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.label12 = new System.Windows.Forms.Label();
            this.groupBoxPredefined = new System.Windows.Forms.GroupBox();
            this.maskedTextBoxIBAN = new System.Windows.Forms.MaskedTextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.checkBoxIBAN = new System.Windows.Forms.CheckBox();
            this.maskedTextBoxTRid = new System.Windows.Forms.MaskedTextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.checkBoxTRid = new System.Windows.Forms.CheckBox();
            this.maskedTextBoxSSN = new System.Windows.Forms.MaskedTextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.checkBoxSSN = new System.Windows.Forms.CheckBox();
            this.maskedTextBoxCC = new System.Windows.Forms.MaskedTextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.checkBoxCC = new System.Windows.Forms.CheckBox();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.buttonSaveAll = new System.Windows.Forms.Button();
            this.label13 = new System.Windows.Forms.Label();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.columnName = new System.Windows.Forms.ColumnHeader();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupAddMd5.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupRegex.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBoxPredefined.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Location = new System.Drawing.Point(4, 3);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(539, 388);
            this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.button3);
            this.tabPage1.Controls.Add(this.label4);
            this.tabPage1.Controls.Add(this.listView1);
            this.tabPage1.Controls.Add(this.groupAddMd5);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(531, 362);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "MD5 File Recognition";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(435, 336);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 4;
            this.button3.Text = "Delete";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(18, 150);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(35, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "label4";
            // 
            // listView1
            // 
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            this.listView1.Location = new System.Drawing.Point(11, 169);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(509, 163);
            this.listView1.TabIndex = 2;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "File";
            this.columnHeader1.Width = 120;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "File Size";
            this.columnHeader2.Width = 100;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Hash";
            this.columnHeader3.Width = 285;
            // 
            // groupAddMd5
            // 
            this.groupAddMd5.Controls.Add(this.label3);
            this.groupAddMd5.Controls.Add(this.textBox2);
            this.groupAddMd5.Controls.Add(this.button2);
            this.groupAddMd5.Controls.Add(this.buttonMd5Browse);
            this.groupAddMd5.Controls.Add(this.label2);
            this.groupAddMd5.Controls.Add(this.textBox1);
            this.groupAddMd5.Location = new System.Drawing.Point(8, 34);
            this.groupAddMd5.Name = "groupAddMd5";
            this.groupAddMd5.Size = new System.Drawing.Size(509, 108);
            this.groupAddMd5.TabIndex = 1;
            this.groupAddMd5.TabStop = false;
            this.groupAddMd5.Text = "Add New File";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 53);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(35, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "label3";
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(109, 49);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(143, 20);
            this.textBox2.TabIndex = 2;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(424, 79);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(79, 23);
            this.button2.TabIndex = 3;
            this.button2.Text = "Add";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // buttonMd5Browse
            // 
            this.buttonMd5Browse.Location = new System.Drawing.Point(322, 21);
            this.buttonMd5Browse.Name = "buttonMd5Browse";
            this.buttonMd5Browse.Size = new System.Drawing.Size(75, 23);
            this.buttonMd5Browse.TabIndex = 2;
            this.buttonMd5Browse.Text = "Browse";
            this.buttonMd5Browse.UseVisualStyleBackColor = true;
            this.buttonMd5Browse.Click += new System.EventHandler(this.buttonMd5Browse_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 26);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "label2";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(109, 22);
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(207, 20);
            this.textBox1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(18, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "label1";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.checkBox1);
            this.tabPage2.Controls.Add(this.button5);
            this.tabPage2.Controls.Add(this.label7);
            this.tabPage2.Controls.Add(this.listViewRegex);
            this.tabPage2.Controls.Add(this.groupRegex);
            this.tabPage2.Controls.Add(this.label5);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(531, 362);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Regular Expressions";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(21, 35);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(158, 17);
            this.checkBox1.TabIndex = 5;
            this.checkBox1.Text = "Enable Regular Expressions";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(450, 334);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(75, 23);
            this.button5.TabIndex = 4;
            this.button5.Text = "Delete";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(22, 163);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 13);
            this.label7.TabIndex = 3;
            this.label7.Text = "label7";
            // 
            // listViewRegex
            // 
            this.listViewRegex.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName,
            this.columnRegex,
            this.columnRegexId});
            this.listViewRegex.FullRowSelect = true;
            this.listViewRegex.GridLines = true;
            this.listViewRegex.Location = new System.Drawing.Point(6, 180);
            this.listViewRegex.Name = "listViewRegex";
            this.listViewRegex.Size = new System.Drawing.Size(519, 148);
            this.listViewRegex.TabIndex = 2;
            this.listViewRegex.UseCompatibleStateImageBehavior = false;
            this.listViewRegex.View = System.Windows.Forms.View.Details;
            this.listViewRegex.VirtualListSize = 5;
            // 
            // columnRegex
            // 
            this.columnRegex.Text = "Regular Expression";
            this.columnRegex.Width = 395;
            // 
            // columnRegexId
            // 
            this.columnRegexId.Width = 0;
            // 
            // groupRegex
            // 
            this.groupRegex.Controls.Add(this.textBox4);
            this.groupRegex.Controls.Add(this.label13);
            this.groupRegex.Controls.Add(this.button4);
            this.groupRegex.Controls.Add(this.textBox3);
            this.groupRegex.Controls.Add(this.label6);
            this.groupRegex.Location = new System.Drawing.Point(6, 58);
            this.groupRegex.Name = "groupRegex";
            this.groupRegex.Size = new System.Drawing.Size(519, 100);
            this.groupRegex.TabIndex = 1;
            this.groupRegex.TabStop = false;
            this.groupRegex.Text = "Add Regular Expression";
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(438, 71);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(75, 23);
            this.button4.TabIndex = 2;
            this.button4.Text = "Add";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(69, 46);
            this.textBox3.Multiline = true;
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(363, 48);
            this.textBox3.TabIndex = 1;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 46);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 13);
            this.label6.TabIndex = 0;
            this.label6.Text = "label6";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(18, 14);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(501, 27);
            this.label5.TabIndex = 0;
            this.label5.Text = "label5";
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.groupBox1);
            this.tabPage3.Controls.Add(this.label12);
            this.tabPage3.Controls.Add(this.groupBoxPredefined);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(531, 362);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Predefined Expressions";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.checkBox2);
            this.groupBox1.Location = new System.Drawing.Point(21, 245);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(481, 51);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Other";
            // 
            // checkBox2
            // 
            this.checkBox2.AutoSize = true;
            this.checkBox2.Location = new System.Drawing.Point(19, 20);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(183, 17);
            this.checkBox2.TabIndex = 0;
            this.checkBox2.Text = "Enable encrypted archive filtering";
            this.checkBox2.UseVisualStyleBackColor = true;
            // 
            // label12
            // 
            this.label12.Location = new System.Drawing.Point(18, 14);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(484, 41);
            this.label12.TabIndex = 1;
            this.label12.Text = "label12";
            // 
            // groupBoxPredefined
            // 
            this.groupBoxPredefined.Controls.Add(this.maskedTextBoxIBAN);
            this.groupBoxPredefined.Controls.Add(this.label11);
            this.groupBoxPredefined.Controls.Add(this.checkBoxIBAN);
            this.groupBoxPredefined.Controls.Add(this.maskedTextBoxTRid);
            this.groupBoxPredefined.Controls.Add(this.label10);
            this.groupBoxPredefined.Controls.Add(this.checkBoxTRid);
            this.groupBoxPredefined.Controls.Add(this.maskedTextBoxSSN);
            this.groupBoxPredefined.Controls.Add(this.label9);
            this.groupBoxPredefined.Controls.Add(this.checkBoxSSN);
            this.groupBoxPredefined.Controls.Add(this.maskedTextBoxCC);
            this.groupBoxPredefined.Controls.Add(this.label8);
            this.groupBoxPredefined.Controls.Add(this.checkBoxCC);
            this.groupBoxPredefined.Location = new System.Drawing.Point(21, 58);
            this.groupBoxPredefined.Name = "groupBoxPredefined";
            this.groupBoxPredefined.Size = new System.Drawing.Size(481, 180);
            this.groupBoxPredefined.TabIndex = 0;
            this.groupBoxPredefined.TabStop = false;
            this.groupBoxPredefined.Text = "Predefined Expressions";
            // 
            // maskedTextBoxIBAN
            // 
            this.maskedTextBoxIBAN.Location = new System.Drawing.Point(422, 131);
            this.maskedTextBoxIBAN.Mask = "00";
            this.maskedTextBoxIBAN.Name = "maskedTextBoxIBAN";
            this.maskedTextBoxIBAN.Size = new System.Drawing.Size(22, 20);
            this.maskedTextBoxIBAN.TabIndex = 12;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(274, 134);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(107, 13);
            this.label11.TabIndex = 11;
            this.label11.Text = "Minimum IBAN Count";
            // 
            // checkBoxIBAN
            // 
            this.checkBoxIBAN.AutoSize = true;
            this.checkBoxIBAN.Location = new System.Drawing.Point(19, 133);
            this.checkBoxIBAN.Name = "checkBoxIBAN";
            this.checkBoxIBAN.Size = new System.Drawing.Size(124, 17);
            this.checkBoxIBAN.TabIndex = 10;
            this.checkBoxIBAN.Text = "Enable IBAN Search";
            this.checkBoxIBAN.UseVisualStyleBackColor = true;
            // 
            // maskedTextBoxTRid
            // 
            this.maskedTextBoxTRid.Location = new System.Drawing.Point(422, 97);
            this.maskedTextBoxTRid.Mask = "00";
            this.maskedTextBoxTRid.Name = "maskedTextBoxTRid";
            this.maskedTextBoxTRid.Size = new System.Drawing.Size(22, 20);
            this.maskedTextBoxTRid.TabIndex = 9;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(274, 100);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(128, 13);
            this.label10.TabIndex = 8;
            this.label10.Text = "Minimum TR ID No Count";
            // 
            // checkBoxTRid
            // 
            this.checkBoxTRid.AutoSize = true;
            this.checkBoxTRid.Location = new System.Drawing.Point(19, 99);
            this.checkBoxTRid.Name = "checkBoxTRid";
            this.checkBoxTRid.Size = new System.Drawing.Size(145, 17);
            this.checkBoxTRid.TabIndex = 7;
            this.checkBoxTRid.Text = "Enable TR ID No Search";
            this.checkBoxTRid.UseVisualStyleBackColor = true;
            // 
            // maskedTextBoxSSN
            // 
            this.maskedTextBoxSSN.Location = new System.Drawing.Point(422, 62);
            this.maskedTextBoxSSN.Mask = "00";
            this.maskedTextBoxSSN.Name = "maskedTextBoxSSN";
            this.maskedTextBoxSSN.Size = new System.Drawing.Size(22, 20);
            this.maskedTextBoxSSN.TabIndex = 6;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(274, 65);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(104, 13);
            this.label9.TabIndex = 5;
            this.label9.Text = "Minimum SSN Count";
            // 
            // checkBoxSSN
            // 
            this.checkBoxSSN.AutoSize = true;
            this.checkBoxSSN.Location = new System.Drawing.Point(19, 64);
            this.checkBoxSSN.Name = "checkBoxSSN";
            this.checkBoxSSN.Size = new System.Drawing.Size(121, 17);
            this.checkBoxSSN.TabIndex = 4;
            this.checkBoxSSN.Text = "Enable SSN Search";
            this.checkBoxSSN.UseVisualStyleBackColor = true;
            // 
            // maskedTextBoxCC
            // 
            this.maskedTextBoxCC.Location = new System.Drawing.Point(422, 28);
            this.maskedTextBoxCC.Mask = "00";
            this.maskedTextBoxCC.Name = "maskedTextBoxCC";
            this.maskedTextBoxCC.Size = new System.Drawing.Size(22, 20);
            this.maskedTextBoxCC.TabIndex = 3;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(274, 31);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(134, 13);
            this.label8.TabIndex = 1;
            this.label8.Text = "Minimum Credit Card Count";
            // 
            // checkBoxCC
            // 
            this.checkBoxCC.AutoSize = true;
            this.checkBoxCC.Location = new System.Drawing.Point(19, 30);
            this.checkBoxCC.Name = "checkBoxCC";
            this.checkBoxCC.Size = new System.Drawing.Size(151, 17);
            this.checkBoxCC.TabIndex = 0;
            this.checkBoxCC.Text = "Enable Credit Card Search";
            this.checkBoxCC.UseVisualStyleBackColor = true;
            // 
            // buttonSaveAll
            // 
            this.buttonSaveAll.Location = new System.Drawing.Point(8, 393);
            this.buttonSaveAll.Name = "buttonSaveAll";
            this.buttonSaveAll.Size = new System.Drawing.Size(140, 23);
            this.buttonSaveAll.TabIndex = 1;
            this.buttonSaveAll.Text = "Save Changes";
            this.buttonSaveAll.UseVisualStyleBackColor = true;
            this.buttonSaveAll.Click += new System.EventHandler(this.buttonSaveAll_Click);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(12, 20);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(41, 13);
            this.label13.TabIndex = 3;
            this.label13.Text = "label13";
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(69, 17);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(243, 20);
            this.textBox4.TabIndex = 4;
            // 
            // columnName
            // 
            this.columnName.Width = 100;
            // 
            // SensitiveData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.buttonSaveAll);
            this.Controls.Add(this.tabControl1);
            this.Name = "SensitiveData";
            this.Size = new System.Drawing.Size(546, 424);
            this.Load += new System.EventHandler(this.SensitiveData_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.groupAddMd5.ResumeLayout(false);
            this.groupAddMd5.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.groupRegex.ResumeLayout(false);
            this.groupRegex.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBoxPredefined.ResumeLayout(false);
            this.groupBoxPredefined.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupAddMd5;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button buttonMd5Browse;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupRegex;
        private System.Windows.Forms.ListView listViewRegex;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.ColumnHeader columnRegex;
        private System.Windows.Forms.ColumnHeader columnRegexId;
        private System.Windows.Forms.GroupBox groupBoxPredefined;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox checkBoxCC;
        private System.Windows.Forms.MaskedTextBox maskedTextBoxCC;
        private System.Windows.Forms.MaskedTextBox maskedTextBoxTRid;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.CheckBox checkBoxTRid;
        private System.Windows.Forms.MaskedTextBox maskedTextBoxSSN;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.CheckBox checkBoxSSN;
        private System.Windows.Forms.MaskedTextBox maskedTextBoxIBAN;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.CheckBox checkBoxIBAN;
        private System.Windows.Forms.Button buttonSaveAll;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox checkBox2;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ColumnHeader columnName;
    }
}
