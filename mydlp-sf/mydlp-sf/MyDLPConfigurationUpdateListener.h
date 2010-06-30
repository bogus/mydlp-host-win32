#pragma once

using namespace System;

namespace mydlpsf
{
	public ref class MyDLPConfigurationUpdateListener
	{
	private:
		static MyDLPConfigurationUpdateListener ^listener;
		System::IO::FileSystemWatcher ^watcher;
		MyDLPConfigurationUpdateListener(void);

	public:
		static MyDLPConfigurationUpdateListener ^GetInstance();
		void StartListener();
		void OnChanged(Object^ o, System::IO::FileSystemEventArgs^ e);
	};
}
