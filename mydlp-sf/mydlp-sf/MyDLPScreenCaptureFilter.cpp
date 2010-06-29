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
#include "MyDLPScreenCaptureFilter.h"

using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;
using namespace System::Reflection;
using namespace System::ComponentModel;

namespace mydlpsf
{
	MyDLPScreenCaptureFilter::MyDLPScreenCaptureFilter(void)
	{
	}

	MyDLPScreenCaptureFilter ^MyDLPScreenCaptureFilter::GetInstance()
	{
		if(screenCaptureFilter == nullptr)
		{
			screenCaptureFilter = gcnew MyDLPScreenCaptureFilter();
			screenCaptureFilter->hhookSysMsg = 0;
		}

		return screenCaptureFilter;
	}

	bool MyDLPScreenCaptureFilter::StartHook()
	{	
		if(hhookSysMsg == 0)
		{
			hhookSysMsg = SetWindowsHookEx(
				WH_KEYBOARD, &KeyboardProc, 
				(HINSTANCE)Marshal::GetHINSTANCE(System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]).ToPointer(), 
				0);

			if(hhookSysMsg == NULL) {
				MyDLPEventLogger::GetInstance()->LogError(GetLastError().ToString());
				return false;
			}
		}
		return true;
	}

	void MyDLPScreenCaptureFilter::StopHook()
	{
		if(hhookSysMsg != 0) {
			UnhookWindowsHookEx(hhookSysMsg);	
			hhookSysMsg = 0;
		}
	}

	HHOOK MyDLPScreenCaptureFilter::GetHook()
	{
		return hhookSysMsg;
	}	
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam,LPARAM lParam)
{	

	if(wParam == VK_SNAPSHOT)
	{
		if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->enableScreenCaptureFilter.Equals(FALSE))
			goto exit;

		array<Process ^> ^arr = Process::GetProcesses();

		for each(Process ^proc in arr)
		{
			if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidMSOffice &&
				(proc->ProcessName->ToLower()->Contains("word") || 
				proc->ProcessName->ToLower()->Contains("excel") ||
				proc->ProcessName->ToLower()->Contains("powerpoint") ||
				proc->ProcessName->ToLower()->Contains("outlook") ||
				proc->ProcessName->ToLower()->Contains("visio") ||
				proc->ProcessName->ToLower()->Contains("project")))
			{
				mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: MSOFFICE");
				return 1;
			}

			if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidOOOrg &&
				(proc->ProcessName->ToLower()->Contains("oowriter") || 
				proc->ProcessName->ToLower()->Contains("oocalc") ||
				proc->ProcessName->ToLower()->Contains("oopresent")))
			{
				mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: OPENOFFICE");
				return 1;
			}

			if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidAcrobatReader &&
				(proc->ProcessName->ToLower()->Contains("acroread") ||
				proc->ProcessName->ToLower()->Contains("acrobat")))
			{
				mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: ACROBAT READER");
				return 1;
			}

			if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidPhotoshop &&
				proc->ProcessName->ToLower()->Contains("photoshop"))
			{
				mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: PHOTOSHOP");
				return 1;
			}
			
			if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidAutoCAD &&
				proc->ProcessName->ToLower()->Contains("autocad"))
			{
				mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: AUTOCAD");
				return 1;
			}
		}
	}

exit:
	return CallNextHookEx(mydlpsf::MyDLPScreenCaptureFilter::GetInstance()->GetHook(), nCode, wParam, lParam);
}




