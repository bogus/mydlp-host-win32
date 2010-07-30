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
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using Microsoft.Win32;
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
			ServicesToRun = new System.ServiceProcess.ServiceBase[] { new MyDLPService() };

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
            //eventLogService.WriteEntry("MyDLP-Host Service Started");
            try
            {
                MyDLPManager.GetInstance().Start();
            }
            catch (Exception ex)
            {
                eventLogService.WriteEntry(ex.StackTrace);
            }
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
			//eventLogService.WriteEntry("MyDLP-Host Service Stopped");
            try
            {
                MyDLPManager.GetInstance().Stop();
            }
            catch (Exception ex)
            {
                eventLogService.WriteEntry(ex.StackTrace);
            }
		}
		protected override void OnContinue()
		{
			eventLogService.WriteEntry("MyDLP-Host Service Continues");
		}
	}
}
