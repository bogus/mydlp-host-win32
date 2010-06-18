#pragma once

using namespace System;

namespace mydlpsf
{
	ref class MyDLPEventLogger
	{
	private:
		static MyDLPEventLogger ^eventLogger;
		System::Diagnostics::EventLog ^eventLogDevice;
		System::Diagnostics::EventLog ^eventLogSensFile;
		System::Threading::Mutex ^sensFileMutex;
		System::Threading::Mutex ^deviceMutex;
		MyDLPEventLogger(void);

	public:
		static MyDLPEventLogger ^GetInstance();
		void LogSensFile(String ^log);
		void LogDevice(String ^log);
		void LogLib(String ^log);
		
	};
}