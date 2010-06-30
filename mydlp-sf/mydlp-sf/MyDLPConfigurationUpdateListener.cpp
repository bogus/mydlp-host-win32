#include "StdAfx.h"
#include "MyDLPConfigurationUpdateListener.h"

using namespace System;
using namespace System::IO;
using namespace Microsoft::Win32;
using namespace System::Security::Permissions;


namespace mydlpsf
{
	MyDLPConfigurationUpdateListener::MyDLPConfigurationUpdateListener(void)
	{
	}

	[PermissionSet(SecurityAction::Demand, Name="FullTrust")]
	MyDLPConfigurationUpdateListener ^MyDLPConfigurationUpdateListener::GetInstance()
	{
		if(listener == nullptr)
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			listener = gcnew MyDLPConfigurationUpdateListener();
			listener->watcher = gcnew FileSystemWatcher((String ^)key->GetValue("Config_Dir"), "*.conf");
			listener->watcher->NotifyFilter = NotifyFilters::LastWrite;
			listener->watcher->Changed += gcnew FileSystemEventHandler(listener, &OnChanged);
		}

		return listener;
	}
	
	[PermissionSet(SecurityAction::Demand, Name="FullTrust")]
	void MyDLPConfigurationUpdateListener::StartListener()
	{
		watcher->EnableRaisingEvents = true;
	}
	
	void MyDLPConfigurationUpdateListener::OnChanged(Object^ o,FileSystemEventArgs^ e)
	{
		Console::WriteLine(e->FullPath + " - " + e->Name);
	}

}
