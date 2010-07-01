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
#include <windows.h>
#include "MyDLPWMIDeviceListener.h"
#include "MyDLPEventLogger.h"
#include "MyDLPRemoteConf.h"

using namespace System::Runtime::InteropServices;

namespace mydlpsf
{
	MyDLPWMIDeviceListener::MyDLPWMIDeviceListener(void)
	{
		sensFileSearch = gcnew System::Collections::Generic::Dictionary<String ^, MyDLPDirectoryTraverse ^>();
		removableDrives = gcnew System::Collections::Generic::List<String ^>();
		watcherList = gcnew List<ManagementEventWatcher ^>();

		array<String ^> ^allDrives = Environment::GetLogicalDrives();

		for (int i = 0; i < allDrives->Length; i++)
        {
			IntPtr ptr = Marshal::StringToHGlobalUni(allDrives[i]);
			if(DRIVE_REMOVABLE == GetDriveType((LPCWSTR)ptr.ToPointer()))
			{
				removableDrives->Add(allDrives[i]);
			}
			Marshal::FreeHGlobal(ptr);
        }
	}

	MyDLPWMIDeviceListener ^MyDLPWMIDeviceListener::GetInstance()
	{
		if(deviceListener == nullptr)
		{
			deviceListener = gcnew MyDLPWMIDeviceListener();
		}

		return deviceListener;
	}

	void MyDLPWMIDeviceListener::AddRemoveUSBHandler()
	{
		WqlEventQuery ^q;
		ManagementEventWatcher ^w;
        ManagementScope ^scope = gcnew ManagementScope("root\\CIMV2");
        scope->Options->EnablePrivileges = true;

        try
        {
            q = gcnew WqlEventQuery();
            q->EventClassName = "__InstanceDeletionEvent";
            q->WithinInterval = TimeSpan(0, 0, 3);
            q->Condition = "TargetInstance ISA 'Win32_USBControllerdevice'";
            w = gcnew ManagementEventWatcher(scope, q);
            w->EventArrived += gcnew EventArrivedEventHandler(USBRemoved);
            w->Start();
			watcherList->Add(w);
        }
        catch (Exception ^ex)
        {
            if (w != nullptr)
                w->Stop();

			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
        }

	}

	void MyDLPWMIDeviceListener::AddInsertUSBHandler()
	{
		WqlEventQuery ^q;
		ManagementEventWatcher ^w;
        ManagementScope ^scope = gcnew ManagementScope("root\\CIMV2");
        scope->Options->EnablePrivileges = true;

        try
        {
            q = gcnew WqlEventQuery();
            q->EventClassName = "__InstanceCreationEvent";
            q->WithinInterval = TimeSpan(0, 0, 3);
            q->Condition = "TargetInstance ISA 'Win32_USBControllerdevice'";
            w = gcnew ManagementEventWatcher(scope, q);
            w->EventArrived += gcnew EventArrivedEventHandler(USBAdded);
            w->Start();
			watcherList->Add(w);
        }
        catch (Exception ^ex)
        {
            if (w != nullptr)
                w->Stop();

			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
        }
	}

	void MyDLPWMIDeviceListener::AddInsertLogicalDeviceHandler()
	{
		WqlEventQuery ^q;
		ManagementEventWatcher ^w;
        ManagementScope ^scope = gcnew ManagementScope("root\\CIMV2");
        scope->Options->EnablePrivileges = true;

        try
        {
            q = gcnew WqlEventQuery();
            q->EventClassName = "__InstanceModificationEvent";
            q->WithinInterval = TimeSpan(0, 0, 3);
			// DriveType - 5: CDROM
            q->Condition = "TargetInstance ISA 'Win32_LogicalDisk' and TargetInstance.DriveType = 5";
            w = gcnew ManagementEventWatcher(scope, q);
            w->EventArrived += gcnew EventArrivedEventHandler(LogicalInserted);
            w->Start();
			watcherList->Add(w);
        }
        catch (Exception ^ex)
        {
            if (w != nullptr)
                w->Stop();

			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
        }
	}

	void MyDLPWMIDeviceListener::USBAdded(Object ^sender, EventArrivedEventArgs ^e)
	{
		if(MyDLPRemoteDeviceConf::GetInstance()->scanPluggedInRemovableDevices)
		{
			PropertyData ^pd = e->NewEvent->Properties["TargetInstance"];
			if (pd != nullptr)
			{
				ManagementBaseObject ^mbo = dynamic_cast<ManagementBaseObject ^>(pd->Value);
				if (mbo->Properties["Antecedent"]->Value != nullptr)
				{
					bool found = false;
					for each(KeyValuePair<String ^, MyDLPDirectoryTraverse ^> pair in sensFileSearch)
					{
						if(pair.Key == mbo->Properties["Antecedent"]->Value)
							found = true;
					}

					if(!found) {
						array<String ^> ^allDrives = Environment::GetLogicalDrives();

						for (int i = 0; i < allDrives->Length; i++)
						{
							IntPtr ptr = Marshal::StringToHGlobalUni(allDrives[i]);
							if(DRIVE_REMOVABLE == GetDriveType((LPCWSTR)ptr.ToPointer()))
							{
								if(!removableDrives->Contains(allDrives[i])) {
									MyDLPDirectoryTraverse ^dirTraverse = gcnew MyDLPDirectoryTraverse();
									sensFileSearch->Add((String ^)mbo->Properties["Antecedent"]->Value, dirTraverse);
									dirTraverse->TraverseDir(allDrives[i]);
								}
							}
						}
					}

				}
			}
		}
	}
	
	void MyDLPWMIDeviceListener::USBRemoved(Object ^sender, EventArrivedEventArgs ^e)
	{
		PropertyData ^pd = e->NewEvent->Properties["TargetInstance"];
		if (pd != nullptr)
		{
			ManagementBaseObject ^mbo = dynamic_cast<ManagementBaseObject ^>(pd->Value);
			if (mbo->Properties["Antecedent"]->Value != nullptr)
			{
				for each(KeyValuePair<String ^, MyDLPDirectoryTraverse ^> pair in sensFileSearch)
				{
					if(pair.Key == mbo->Properties["Antecedent"]->Value)
					{
						((MyDLPDirectoryTraverse ^)pair.Value)->StopScan();
					}
				}
				sensFileSearch->Remove((String ^)mbo->Properties["Antecedent"]->Value);
			}
		}
	}

	void MyDLPWMIDeviceListener::LogicalInserted(Object ^sender, EventArrivedEventArgs ^e)
	{
		PropertyData ^pd = e->NewEvent->Properties["TargetInstance"];
        if (pd != nullptr)
        {
            ManagementBaseObject ^mbo = dynamic_cast<ManagementBaseObject ^>(pd->Value);
            if (mbo->Properties["VolumeName"]->Value != nullptr)
            {
				if(MyDLPRemoteDeviceConf::GetInstance()->scanInsertedLogical)
				{
					MyDLPDirectoryTraverse ^dirTraverse = gcnew MyDLPDirectoryTraverse();
					sensFileSearch->Add(("CD-" + (String ^)mbo->Properties["Name"]->Value), dirTraverse);
					dirTraverse->TraverseDir(mbo->Properties["Name"]->Value + "\\");
				}
            }
            else
            {
				MyDLPDirectoryTraverse ^dirTraverse = sensFileSearch[("CD-" + (String ^)mbo->Properties["Name"]->Value)];
				if(dirTraverse != nullptr) {
					dirTraverse->StopScan();
					sensFileSearch->Remove(("CD-" + (String ^)mbo->Properties["Name"]->Value));
				}
            }
        }		
	}

	void MyDLPWMIDeviceListener::StopWatchers()
	{
		for each(ManagementEventWatcher ^w in watcherList)
		{
			w->Stop();
		}
		watcherList->Clear();
	}
}