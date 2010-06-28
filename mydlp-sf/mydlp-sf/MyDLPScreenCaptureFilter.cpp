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
	/*	
		Process ^process = Process::GetCurrentProcess();
		ProcessModule ^module = process->MainModule;

		Console::WriteLine(GetModuleHandle(NULL)->unused.ToString());
		
		IntPtr ^ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(module->ModuleName);

		hhookSysMsg = SetWindowsHookEx(
			WH_KEYBOARD, &KeyboardProc, 
			(HINSTANCE)Marshal::GetHINSTANCE(System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]).ToPointer(), 0);

		if(hhookSysMsg == NULL) {
			Console::WriteLine(GetLastError().ToString());
			return false;
		}
	
		hookProc = gcnew HookProc(TestProc);
		hook = SetWindowsHookEx(
				2, 
				hookProc, 
				Marshal::GetHINSTANCE(System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]), 
				0);

		if(hook == nullptr) {
			Console::WriteLine("**********");
			return false;
		}
		*/

		installhook((HINSTANCE)Marshal::GetHINSTANCE(System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]).ToPointer());

		return true;
	}

	void MyDLPScreenCaptureFilter::StopHook()
	{
		//RemoveHooks();
		//UnhookWindowsHookEx(hhookSysMsg);
	}
/*
	HHOOK MyDLPScreenCaptureFilter::GetHook()
	{
		return hhookSysMsg;
	}	
*/
}
/*
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam,LPARAM lParam)
{
	
	printf("-------------\n");
	if(wParam == VK_SNAPSHOT)
	{
		mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen Hit");
	}
	mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen Hit " + wParam.ToString());
	
	return CallNextHookEx(mydlpsf::MyDLPScreenCaptureFilter::GetInstance()->GetHook(), nCode, wParam, lParam);
}
*/



