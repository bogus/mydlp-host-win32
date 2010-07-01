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
#include "MyDLPDirectoryTraverse.h"
#include "MyDLPEventLogger.h"
#include "MyDLPMessages.h"
#include <vcclr.h>
#using <mscorlib.dll>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;


namespace mydlpsf {

	MyDLPDirectoryTraverse::MyDLPDirectoryTraverse(void)
	{
		cont = true;
		completed = false;
	}

	List<MyDLPDirectoryTraverse ^> ^MyDLPDirectoryTraverse::TraverseAllDrives() {
		
		List<MyDLPDirectoryTraverse ^> ^dirTraverseList = gcnew List<MyDLPDirectoryTraverse ^>();

		array<String ^> ^allDrives = Environment::GetLogicalDrives();
        List<String ^> ^fixedDrives = gcnew List<String ^>();

		for (int i = 0; i < allDrives->Length; i++)
        {
			IntPtr ptr = Marshal::StringToHGlobalUni(allDrives[i]);
			if(DRIVE_FIXED == GetDriveType((LPCWSTR)ptr.ToPointer()))
			{
				fixedDrives->Add(allDrives[i]);
			}
			Marshal::FreeHGlobal(ptr);
        }

		
		for each (String ^driveLetter in fixedDrives)
		{
			MyDLPDirectoryTraverse ^dirTraverse = gcnew MyDLPDirectoryTraverse();
			dirTraverse->detected = gcnew String("Traverse Drive : " + driveLetter + System::Environment::NewLine);
			Thread ^thread = gcnew Thread( gcnew ParameterizedThreadStart(dirTraverse, &MyDLPDirectoryTraverse::Traverse) );
			thread->Start(driveLetter);
			
			dirTraverseList->Add(dirTraverse);
		}
	
		return dirTraverseList;
	}

	void MyDLPDirectoryTraverse::StopScan()
	{
		cont = false;
	}

	List<MyDLPDirectoryTraverse ^> ^MyDLPDirectoryTraverse::TraverseDir(String ^path) 
	{
		MyDLPDirectoryTraverse ^dirTraverse = gcnew MyDLPDirectoryTraverse();
		List<MyDLPDirectoryTraverse ^> ^dirTraverseList = gcnew List<MyDLPDirectoryTraverse ^>();

		dirTraverse->detected = gcnew String("TraverseDir: " + path + System::Environment::NewLine);
		Thread ^thread = gcnew Thread( gcnew ParameterizedThreadStart(dirTraverse, &MyDLPDirectoryTraverse::Traverse) );
		thread->Start(path);

		dirTraverseList->Add(dirTraverse);
		return dirTraverseList;
	}

	void MyDLPDirectoryTraverse::Traverse(Object ^obj)
	{
		
		String ^origPath = gcnew String(((String ^)obj)->ToCharArray());
		TraverseDirectory(obj, origPath);
	}

	void MyDLPDirectoryTraverse::TraverseDirectory(Object ^obj, Object ^origPath)
	{		
		if(cont->Equals(FALSE)) {
			return;
		}
		
		String ^path = obj->ToString();
		String ^originalPath = (String ^)origPath;

		try
		{
			array<String ^> ^dirs = Directory::GetDirectories(path);
			array<String ^> ^files = Directory::GetFiles(path);
			System::Collections::SortedList ^all = gcnew System::Collections::SortedList();

			// Add all the directories to the list
			for (int i = 0; i < dirs->Length; i++)
			{
				all[dirs[i]] = "d";
			}

			// Add all the files to the list
			for (int i = 0; i < files->Length; i++)
			{
				all[files[i]] = "f";
			}

			for each(String ^key in all->Keys)
			{
				if(cont->Equals(FALSE)) {
					return;
				}

				if (((String ^)all[key]) == "f")
				{
					try
					{
						int ret = 0;                    
						FileStream ^fs = gcnew FileStream(key, FileMode::Open, FileAccess::Read);
						fs->Close();
						
						MyDLPSensitiveFileRecognition ^fileSearch = MyDLPSensFilePool::GetInstance()->AcquireObject();
						ret = fileSearch->SearchSensitiveData(key);
	                    
						if (ret == 1)
						{
							detected += key + ":" + fileSearch->GetLastResult() + System::Environment::NewLine;
							OnDetectedChanged(EventArgs::Empty);
						}
						else if (ret == 2)
						{
							MyDLPEventLogger::GetInstance()->LogError(key + " -- EARG ");
						}
						else if (ret == 3)
						{
							MyDLPEventLogger::GetInstance()->LogError(key + " -- Error ");
						}
						

						MyDLPSensFilePool::GetInstance()->ReleaseObject(fileSearch);
					}
					catch (Exception ^ex)
					{
						MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
					}
				}
				else if (((String ^)all[key]) == "d")
				{
					bool isExcluded = false;
					if(MyDLPRemoteDeviceConf::GetInstance()->excludedDirs->Count != 0)
					{
						for each(String ^dirName in MyDLPRemoteDeviceConf::GetInstance()->excludedDirs)
						{
							if(dirName == key) {
								isExcluded = true;
								break;
							}
						}
					}
					if(!isExcluded && cont)
						MyDLPDirectoryTraverse::TraverseDirectory(key, origPath);
				}
			}
		} catch (Exception ^ex) {
			cont = false;
			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
		}

		if(path == originalPath) {
			OnCompleted(EventArgs::Empty);
			MyDLPMessages::GetInstance()->AddMessage("Scan completed - " + path);
		}
	}

	void MyDLPDirectoryTraverse::OnCompleted(EventArgs ^e) 
    {
		if(completed->Equals(FALSE))
		{
			this->Completed(this, e);
			completed = true;
			MyDLPEventLogger::GetInstance()->LogSensFile(detected);
		}
    }

	void MyDLPDirectoryTraverse::OnDetectedChanged(EventArgs ^e) 
    {
		this->DetectedChanged(this, e);
	}

}