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