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
#include "MyDLPMessages.h"

using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;
using namespace System::Reflection;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;

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
		}

		return screenCaptureFilter;
	}

	bool MyDLPScreenCaptureFilter::StartHook()
	{	
		if(khook == nullptr)
		{
			khook = gcnew KeyboardHook();
			khook->InstallHook();
			khook->KeyDown += gcnew EventHandler<KeyboardEventArgs ^>(this,&MyDLPScreenCaptureFilter::CheckKeyboard);
		}
		return true;
	}

	void MyDLPScreenCaptureFilter::StopHook()
	{
		if(khook != nullptr)
		{
			khook->RemoveHook();
			khook = nullptr;	
		}
	}

	void MyDLPScreenCaptureFilter::CheckKeyboard(Object ^sender, WindowsHook::KeyboardEventArgs ^e)
	{
		if(e->KeyCode == Keys::PrintScreen)
		{
			if(!mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->enableScreenCaptureFilter)
			{
				e->Handled = false;
				return;
			}

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
					mydlpsf::MyDLPMessages::GetInstance()->AddMessage("Print Screen filtered: MSOFFICE");
					e->Handled = true;
					break;
				}

				if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidOOOrg &&
					(proc->ProcessName->ToLower()->Contains("oowriter") || 
					proc->ProcessName->ToLower()->Contains("oocalc") ||
					proc->ProcessName->ToLower()->Contains("oopresent")))
				{
					mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: OPENOFFICE");
					mydlpsf::MyDLPMessages::GetInstance()->AddMessage("Print Screen filtered: OPENOFFICE");
					e->Handled = true;
					break;
				}

				if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidAcrobatReader &&
					(proc->ProcessName->ToLower()->Contains("acroread") ||
					proc->ProcessName->ToLower()->Contains("acrobat")))
				{
					mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: ACROBAT READER");
					mydlpsf::MyDLPMessages::GetInstance()->AddMessage("Print Screen filtered: ACROBAT READER");
					e->Handled = true;
					break;
				}

				if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidPhotoshop &&
					(proc->ProcessName->ToLower()->Contains("photoshop") || 
					proc->ProcessName->ToLower()->Contains("imageready")))
				{
					mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: PHOTOSHOP");
					mydlpsf::MyDLPMessages::GetInstance()->AddMessage("Print Screen filtered: PHOTOSHOP");
					e->Handled = true;
					break;
				}
				
				if(mydlpsf::MyDLPRemoteScreenCaptureConf::GetInstance()->forbidAutoCAD &&
					proc->ProcessName->ToLower()->Contains("autocad"))
				{
					mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen filtered: AUTOCAD");
					mydlpsf::MyDLPMessages::GetInstance()->AddMessage("Print Screen filtered: AUTOCAD");
					e->Handled = true;
					break;
				}
			}
		}
	}
}



