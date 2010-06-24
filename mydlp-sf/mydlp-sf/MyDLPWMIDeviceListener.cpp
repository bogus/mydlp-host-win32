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
#include "MyDLPWMIDeviceListener.h"

namespace mydlpsf
{
	MyDLPWMIDeviceListener::MyDLPWMIDeviceListener(void)
	{
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
        }
        catch (Exception ^ex)
        {
            if (w != nullptr)
                w->Stop();
        }

	}

	void MyDLPWMIDeviceListener::AddInsertUSBHandler()
	{
		WqlEventQuery ^q;
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
        }
        catch (Exception ^ex)
        {
            if (w != nullptr)
                w->Stop();
        }
	}

	void MyDLPWMIDeviceListener::AddInsertLogicalDeviceHandler()
	{
		WqlEventQuery ^q;
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
        }
        catch (Exception ^ex)
        {
            if (w != nullptr)
                w->Stop();
        }
	}

	void MyDLPWMIDeviceListener::USBAdded(Object ^sender, EventArrivedEventArgs ^e)
	{
		//Console::WriteLine("USB Added");
		if(MyDLPRemoteDeviceConf::GetInstance()->scanPluggedInRemovableDevices)
		{
			
		}
	}
	
	void MyDLPWMIDeviceListener::USBRemoved(Object ^sender, EventArrivedEventArgs ^e)
	{
		Console::WriteLine("USB Removed");
	}

	void MyDLPWMIDeviceListener::LogicalInserted(Object ^sender, EventArrivedEventArgs ^e)
	{
		PropertyData ^pd = e->NewEvent->Properties["TargetInstance"];
        if (pd != nullptr)
        {
            ManagementBaseObject ^mbo = dynamic_cast<ManagementBaseObject ^>(pd->Value);
            if (mbo->Properties["VolumeName"]->Value != nullptr)
            {
				Console::WriteLine("A CD/DVD inserted : " + mbo->Properties["VolumeName"]->Value);
            }
            else
            {
                Console::WriteLine("A CD/DVD ejected");
            }
        }
	}
}