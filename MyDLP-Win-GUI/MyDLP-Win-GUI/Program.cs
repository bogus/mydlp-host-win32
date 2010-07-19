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
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace MydlpWinGui
{
    static class Program
    {
        static Mutex mutex = new Mutex(true, "MyDLP-Win-GUI");

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length == 2)
            {
                if (System.Environment.UserInteractive)
                {
                    try
                    {
                        ProcessStartInfo startInfo = new ProcessStartInfo();
                        Process p = new Process();
                        Process old;
                        int test;
                        switch (args[0])
                        {
                            case "--restart":
                                MessageBox.Show("Just restart");
                                if (Int32.TryParse(args[1], out test))
                                {
                                    //old = Process.GetProcessById(Int32.Parse(args[1]));
                                    //old.Kill();
                                    //Thread.Sleep(3000);
                                    //old.WaitForExit(3000);
                                    startInfo.FileName = Application.ExecutablePath;
                                    p.StartInfo = startInfo;
                                    p.Start();
                                }
                                break;
                            case "--restart-admin":
                                MessageBox.Show(args[1]);
                                if(Int32.TryParse(args[1], out test)) {
                                    //old = Process.GetProcessById(Int32.Parse(args[1]));
                                    //old.Kill();
                                    //Thread.Sleep(3000);
                                    //old.WaitForExit(3000);
                                    startInfo.FileName = Application.ExecutablePath;
                                    startInfo.Verb = "runas";
                                    startInfo.Arguments = "administrator"; //feel free to lose this line
                                    p.StartInfo = startInfo;
                                    p.Start();
                                }
                                break; 
                        }

                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }
                }
            }
            else
            {
                if (mutex.WaitOne(TimeSpan.Zero, true))
                {
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);
                    Application.Run(new Form1());
                    mutex.ReleaseMutex();
                }
                else
                {
                    NativeMethods.PostMessage(
                   (IntPtr)NativeMethods.HWND_BROADCAST,
                   NativeMethods.WM_SHOWME,
                   IntPtr.Zero,
                   IntPtr.Zero);

                }
            }
        }

        internal class NativeMethods
        {
            public const int HWND_BROADCAST = 0xffff;
            public static readonly int WM_SHOWME = RegisterWindowMessage("WM_SHOWME");
            [DllImport("user32")]
            public static extern bool PostMessage(IntPtr hwnd, int msg, IntPtr wparam, IntPtr lparam);
            [DllImport("user32")]
            public static extern int RegisterWindowMessage(string message);
        }
    }
}
