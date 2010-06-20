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

#include "StdAfx.h"
#include "MyDLPEventLogger.h"

namespace mydlpsf
{
	MyDLPEventLogger::MyDLPEventLogger(void)
	{
	}

	MyDLPEventLogger ^MyDLPEventLogger::GetInstance()
	{
		if(eventLogger == nullptr)
		{
			eventLogger = gcnew MyDLPEventLogger();

			eventLogger->eventLogDevice = gcnew System::Diagnostics::EventLog();
			((System::ComponentModel::ISupportInitialize ^)(eventLogger->eventLogDevice))->BeginInit();
            eventLogger->eventLogDevice->Log = "MyDLPLog";
            eventLogger->eventLogDevice->Source = "MyDLPLogs-Device";
            ((System::ComponentModel::ISupportInitialize ^)(eventLogger->eventLogDevice))->EndInit();

            eventLogger->eventLogSensFile = gcnew System::Diagnostics::EventLog();
			((System::ComponentModel::ISupportInitialize ^)(eventLogger->eventLogSensFile))->BeginInit();
            eventLogger->eventLogSensFile->Log = "MyDLPLog";
            eventLogger->eventLogSensFile->Source = "MyDLPLogs-SF";           
			((System::ComponentModel::ISupportInitialize ^)(eventLogger->eventLogSensFile))->EndInit();

			if (!System::Diagnostics::EventLog::SourceExists("MyDLPLogs-Device"))
            {
                System::Diagnostics::EventLog::CreateEventSource("MyDLPLogs-Device", "MyDLPLog");
            }
            if (!System::Diagnostics::EventLog::SourceExists("MyDLPLogs-SF"))
            {
                System::Diagnostics::EventLog::CreateEventSource("MyDLPLogs-SF", "MyDLPLog");
            }

			eventLogger->eventLogDevice->Source = "MyDLPLogs-Device";
            eventLogger->eventLogDevice->Log = "MyDLPLog";

            eventLogger->eventLogSensFile->Source = "MyDLPLogs-SF";
            eventLogger->eventLogSensFile->Log = "MyDLPLog";

			eventLogger->deviceMutex = gcnew System::Threading::Mutex();
			eventLogger->sensFileMutex = gcnew System::Threading::Mutex();
		}

		return eventLogger;
	}
	
	void MyDLPEventLogger::LogSensFile(String ^log)
	{
		eventLogger->sensFileMutex->WaitOne();

		eventLogger->eventLogSensFile->WriteEntry(log);

		eventLogger->sensFileMutex->ReleaseMutex();
	}

	void MyDLPEventLogger::LogDevice(String ^log)
	{
		eventLogger->deviceMutex->WaitOne();

		eventLogger->eventLogDevice->WriteEntry(log);

		eventLogger->deviceMutex->ReleaseMutex();
	}

	void MyDLPEventLogger::LogLib(String ^log)
	{

	}
}