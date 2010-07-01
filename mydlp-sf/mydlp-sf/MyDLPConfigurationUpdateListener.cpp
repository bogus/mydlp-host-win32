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
#include "MyDLPConfigurationUpdateListener.h"
#include "MyDLPManager.h"

using namespace System;
using namespace System::IO;
using namespace Microsoft::Win32;
using namespace System::Security::Permissions;
using namespace System::Threading;


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
		Thread::Sleep(1);
		MyDLPManager::GetInstance()->Update(e->Name);
	}

}
