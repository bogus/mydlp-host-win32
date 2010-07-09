using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Management;
using System.Timers;
using System.IO;
using Microsoft.Win32;

namespace MyDLPHost
{
	public class MyDLPControllerService : System.ServiceProcess.ServiceBase
	{
        public System.Diagnostics.EventLog eventLogService;
        private FileSystemWatcher watcher;
        private static Timer timer;
        private static String lastFileName;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

        public MyDLPControllerService()
		{
			InitializeComponent();

            if (!System.Diagnostics.EventLog.SourceExists("MyDLPLogs-Service"))
            {
                System.Diagnostics.EventLog.CreateEventSource("MyDLPLogs-Service", "MyDLPLog");
            }

            eventLogService.Source = "MyDLPLogs-Service";
            eventLogService.Log = "MyDLPLog";
            eventLogService.ModifyOverflowPolicy(OverflowAction.OverwriteAsNeeded, 1);
		}

        // The main entry point for the process
        static void Main()
        {
            System.ServiceProcess.ServiceBase[] ServicesToRun;

            // More than one user Service may run within the same process. To add
            // another service to this process, change the following line to
            // create a second service object. For example,
            //
            //   ServicesToRun = New System.ServiceProcess.ServiceBase[] {new Service1(), new MySecondUserService()};
            //
            ServicesToRun = new System.ServiceProcess.ServiceBase[] { new MyDLPControllerService() };

            System.ServiceProcess.ServiceBase.Run(ServicesToRun);
        }

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.eventLogService = new System.Diagnostics.EventLog();
            ((System.ComponentModel.ISupportInitialize)(this.eventLogService)).BeginInit();
            // 
            // eventLogService
            // 
            this.eventLogService.Log = "MyDLPLog";
            this.eventLogService.Source = "MyDLPLogs-Service";
            // 
            // Service1
            // 
            this.ServiceName = "MyDLP-Controller";
            ((System.ComponentModel.ISupportInitialize)(this.eventLogService)).EndInit(); 

            RegistryKey key = Registry.LocalMachine.OpenSubKey( "Software\\MyDLP" );
			watcher = new FileSystemWatcher((String )key.GetValue("Config_Dir"), "*.conf");
			watcher.NotifyFilter = NotifyFilters.LastWrite;
			watcher.Changed += new FileSystemEventHandler(OnChanged);

			timer = new System.Timers.Timer();
			timer.Elapsed += new ElapsedEventHandler(ReleaseFileName);
			timer.Interval = 1000;
			lastFileName = String.Empty;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		protected override void OnStart(string[] args)
		{
            try
            {
                watcher.EnableRaisingEvents = true;
            }
            catch (Exception ex)
            {
                //MyDLPEventLogger.GetInstance().LogService(ex.StackTrace);
            }
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
            try
            {
                watcher.EnableRaisingEvents = false;
            }
            catch (Exception ex)
            {
                //MyDLPEventLogger.GetInstance().LogService(ex.StackTrace);
            }
		}
		protected override void OnContinue()
		{
			
		}

        private static void OnChanged(Object o,FileSystemEventArgs e)
	    {
		    if(lastFileName == String.Empty) {
			    
			    foreach (ServiceController sc in ServiceController.GetServices())
                {
                    if (sc.ServiceName.Equals("MyDLP-Host"))
                    {
					    if (sc.Status == ServiceControllerStatus.Running)
                            sc.Stop();
                    }
                }

			    foreach (ServiceController sc in ServiceController.GetServices())
                {
                    if (sc.ServiceName.Equals("MyDLP-Host"))
                    {
					    while(!(sc.Status == ServiceControllerStatus.Stopped));
					    if(sc.Status == ServiceControllerStatus.Stopped)
						    sc.Start();
                    }
                }
    			
		    }
	    }

	    private static void ReleaseFileName( object source, ElapsedEventArgs e )
	    {
		    timer.Stop();
		    lastFileName = String.Empty;	
	    }
	}
}
