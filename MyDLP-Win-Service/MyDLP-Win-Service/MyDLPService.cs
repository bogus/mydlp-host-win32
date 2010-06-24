using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Management;
using mydlpsf;
using System.Timers;

namespace MyDLPHost
{
	public class MyDLPService : System.ServiceProcess.ServiceBase
	{
        public System.Diagnostics.EventLog eventLogService;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public MyDLPService()
		{

			// This call is required by the Windows.Forms Component Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitComponent call
			if(!System.Diagnostics.EventLog.SourceExists("MyDLPLogs-Service"))
			{
                System.Diagnostics.EventLog.CreateEventSource("MyDLPLogs-Service", "MyDLPLog");
			}

            eventLogService.Source = "MyDLPLogs-Service";
			eventLogService.Log ="MyDLPLog";

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
			ServicesToRun = new System.ServiceProcess.ServiceBase[] { new MyDLPHost.MyDLPService()};

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
            this.ServiceName = "MyDLP-Host";
            ((System.ComponentModel.ISupportInitialize)(this.eventLogService)).EndInit();

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
            eventLogService.WriteEntry("MyDLP-Host Service Started");
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
			eventLogService.WriteEntry("MyDLP-Host Service Stopped");
		}
		protected override void OnContinue()
		{
			eventLogService.WriteEntry("MyDLP-Host Service Continues");
		}
	}
}
