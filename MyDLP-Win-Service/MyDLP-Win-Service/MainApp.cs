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
using System.Text;
using System.Configuration.Install;
using System.Reflection;
using System.ServiceProcess;
using Microsoft.Win32;
using mydlpsf;

namespace MyDLPHost
{
    class MainApp
    {
        static void Main(string[] args)
        {
            if (System.Environment.UserInteractive)
            {
                string parameter = string.Concat(args);
                try
                {
                    switch (parameter)
                    {
                        case "--install":
                            ManagedInstallerClass.InstallHelper(new string[] { Assembly.GetExecutingAssembly().Location });
                            try
                            {
                                RegistryKey ckey =
                                Registry.LocalMachine.OpenSubKey(@"SYSTEM\CurrentControlSet\Services\MyDLP-Host",
                                true);
                                // Good to always do error checking!

                                if (ckey != null)
                                {
                                    // Ok now lets make sure the "Type" value is there,

                                    //and then do our bitwise operation on it.

                                    if (ckey.GetValue("Type") != null)
                                    {
                                        ckey.SetValue("Type", ((int)ckey.GetValue("Type")|256));
                                    }
                                }
                            }
                            catch
                            {

                            }
                            break;
                        case "--uninstall":
                            ManagedInstallerClass.InstallHelper(new string[] { "/u", Assembly.GetExecutingAssembly().Location });
                            break;
                        case "--start":
                            foreach (ServiceController sc in ServiceController.GetServices())
                            {
                                if (sc.ServiceName == "MyDLP-Host")
                                {
                                    if (sc.Status != ServiceControllerStatus.Running)
                                        sc.Start();
                                } 
                                
                            }
                            break;
                        case "--stop":
                            foreach (ServiceController sc in ServiceController.GetServices())
                            {
                                if (sc.ServiceName == "MyDLP-Host")
                                {
                                    if (sc.Status == ServiceControllerStatus.Running)
                                        sc.Stop();
                                }
                            }
                            break;

                    }
                }
                catch (Exception ex)
                {
                    MyDLPEventLogger.GetInstance().LogService("Exception in service initialization: " + ex.StackTrace);
                }
            }
            else
            {
                ServiceBase.Run(new MyDLPService());
            }
        }

    }
}
