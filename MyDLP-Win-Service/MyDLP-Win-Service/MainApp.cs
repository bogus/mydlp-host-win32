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
