using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration.Install;
using System.Reflection;
using System.ServiceProcess;

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
                            break;
                        case "--uninstall":
                            ManagedInstallerClass.InstallHelper(new string[] { "/u", Assembly.GetExecutingAssembly().Location });
                            break;
                        case "--start":
                            foreach (ServiceController sc in ServiceController.GetServices())
                            {
                              if (sc.ServiceName == "MyDLP-Controller")
                                {
                                    if (sc.Status != ServiceControllerStatus.Running)
                                        sc.Start();
                                }
                            }
                            break;
                        case "--stop":
                            foreach (ServiceController sc in ServiceController.GetServices())
                            {
                                if (sc.ServiceName == "MyDLP-Controller")
                                {
                                    if (sc.Status != ServiceControllerStatus.Running)
                                        sc.Stop();
                                }
                            }
                            break;

                    }
                }
                catch (Exception ex)
                {
                    
                }
            }
            else
            {
                ServiceBase.Run(new MyDLPControllerService());
            }
        }

    }
}
