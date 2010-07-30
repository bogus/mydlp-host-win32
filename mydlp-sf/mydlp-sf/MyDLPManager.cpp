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
#include "stdio.h"
#include "MyDLPFSMFListener.h"
#include "MyDLPWMIDeviceListener.h"
#include "MyDLPScreenCaptureFilter.h"
#include "MyDLPEventLogger.h"
#include "MyDLPManager.h"
#include "MyDLPRemoteConf.h"
#include "MyDLPSensFilePool.h"
#include "MyDLPSOAPUpdater.h"
#include "MyDLPConfigurationUpdateListener.h"

namespace mydlpsf
{
	MyDLPManager::MyDLPManager(void)
	{
	}

	MyDLPManager ^MyDLPManager::GetInstance()
	{
		if(manager == nullptr)
		{
			manager = gcnew MyDLPManager();
		}

		return manager;
	}
	
	void MyDLPManager::Configure()
	{
		
	}

	void MyDLPManager::Start()
	{
		try
		{
			MyDLPRemoteDeviceConf::GetInstance()->Deserialize();
			MyDLPRemoteScreenCaptureConf::GetInstance()->Deserialize();
			MyDLPRemoteSensFileConf::GetInstance()->Deserialize();
			MyDLPRemoteServiceConf::GetInstance()->Deserialize();

			MyDLPConfigurationUpdateListener::GetInstance()->StartListener();

			MyDLPSensFilePool::GetInstance()->InitPool();

			// TODO : Disabled until proper testing
			//MyDLPWMIDeviceListener::GetInstance()->AddRemoveUSBHandler();
			//MyDLPWMIDeviceListener::GetInstance()->AddInsertUSBHandler();
			//MyDLPWMIDeviceListener::GetInstance()->AddInsertLogicalDeviceHandler();

			if(MyDLPRemoteDeviceConf::GetInstance()->enableRemovableOnlineScanning.Equals(TRUE))
				MyDLPFSMFListener::RunFilter();

			MyDLPSOAPUpdater::GetInstance()->StartUpdater();
			
		}
		catch(Exception ^ex)
		{
			MyDLPEventLogger::GetInstance()->LogError("MyDLPManager::Start" + ex->InnerException->StackTrace);
			throw gcnew Exception("MyDLPManager::Start", ex);
		}
	}

	void MyDLPManager::Update(String ^filename)
	{
		try
		{
			if(filename->Equals((String ^)MyDLPRemoteDeviceConf::confFileName)) {
				MyDLPRemoteDeviceConf::GetInstance()->Deserialize();
				if(MyDLPRemoteDeviceConf::GetInstance()->enableRemovableOnlineScanning.Equals(TRUE))
					MyDLPFSMFListener::RunFilter();
				else
					MyDLPFSMFListener::StopFilter();
			}
			
			else if(filename->Equals((String ^)MyDLPRemoteSensFileConf::confFileName)) {
				MyDLPFSMFListener::StopFilter();
				MyDLPRemoteSensFileConf::GetInstance()->Deserialize();
				MyDLPSensFilePool::GetInstance()->UpdatePool();
				MyDLPFSMFListener::RunFilter();
			}

			else if(filename->Equals((String ^)MyDLPRemoteServiceConf::confFileName)) 
				MyDLPRemoteServiceConf::GetInstance()->Deserialize();

			else if(filename->Equals((String ^)MyDLPRemoteScreenCaptureConf::confFileName)) 
				MyDLPRemoteScreenCaptureConf::GetInstance()->Deserialize();
		
		}
		catch(Exception ^ex)
		{
			MyDLPEventLogger::GetInstance()->LogError("MyDLPManager::Update " + ex->InnerException->StackTrace);
			throw gcnew Exception("MyDLPManager::Update", ex);
		}
		
	}

	void MyDLPManager::Stop()
	{
		try
		{
			// TODO: Disabled until proper testing
			//MyDLPWMIDeviceListener::GetInstance()->StopWatchers();
			MyDLPConfigurationUpdateListener::GetInstance()->StopListener();
			MyDLPFSMFListener::StopFilter();
			MyDLPDirectoryTraverse::StopAllScans();
			MyDLPSOAPUpdater::GetInstance()->StopUpdater();
		}
		catch(Exception ^ex)
		{
			MyDLPEventLogger::GetInstance()->LogError("MyDLPManager::Stop" + ex->InnerException->StackTrace);
			throw gcnew Exception("MyDLPManager::Stop", ex);
		}
	}
}
