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
#include <vcclr.h>
#using <mscorlib.dll>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::IO;
using namespace System::Runtime::InteropServices;


namespace mydlpsf {

	MyDLPDirectoryTraverse::MyDLPDirectoryTraverse(void)
	{
		cont = true;
	}

	void MyDLPDirectoryTraverse::TraverseAllDrives() {
		array<String ^> ^allDrives = Environment::GetLogicalDrives();
        List<String ^> ^fixedDrives = gcnew List<String ^>();
		String ^detected = gcnew String("TraverseAllDrives\n");

		for (int i = 0; i < allDrives->Length; i++)
        {
			IntPtr ptr = Marshal::StringToHGlobalUni(allDrives[i]);
			if(DRIVE_FIXED == GetDriveType((LPCWSTR)ptr.ToPointer()))
			{
				fixedDrives->Add(allDrives[i]);
			}
			Marshal::FreeHGlobal(ptr);
        }

		MyDLPSensitiveFileRecognition ^sensFileObj = MyDLPSensFilePool::GetInstance()->AcquireObject();

		for each (String ^driveLetter in fixedDrives)
		{
			TraverseDirectory(driveLetter, 4, sensFileObj, detected);
		}
		
		MyDLPSensFilePool::GetInstance()->ReleaseObject(sensFileObj);

		MyDLPEventLogger::GetInstance()->LogSensFile(detected);

	}

	void MyDLPDirectoryTraverse::StopScan()
	{
		cont = false;
	}

	void MyDLPDirectoryTraverse::TraverseDir(String ^path) 
	{
		String ^detected = gcnew String("TraverseDir: " + path + "\n");
		MyDLPSensitiveFileRecognition ^sensFileObj = MyDLPSensFilePool::GetInstance()->AcquireObject();
		TraverseDirectory(path, 4, sensFileObj, detected);
		MyDLPSensFilePool::GetInstance()->ReleaseObject(sensFileObj);
		MyDLPEventLogger::GetInstance()->LogSensFile(detected);
	}

	void MyDLPDirectoryTraverse::TraverseDirectory(String ^path, UInt32 lvl, 
		MyDLPSensitiveFileRecognition ^fileSearch, String ^%detected)
	{		

		if(cont->Equals(FALSE)) {
			return;
		}

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
                    ret = fileSearch->SearchSensitiveData(key);
                    
                    if (ret == 1)
                    {
                        detected += key + ":" + fileSearch->GetLastResult() + "\n";
                    }
                    else if (ret == 2)
                    {
                        //Error in clamav
                    }
                }
                catch (...)
                {
                    // error in file not exist or cannot be opened
                }
            }
            else if (((String ^)all[key]) == "d")
            {
				bool isExcluded = false;
				if(MyDLPRemoteSensFileConf::GetInstance()->excludedDirs->Count != 0)
				{
					for each(String ^dirName in MyDLPRemoteSensFileConf::GetInstance()->excludedDirs)
					{
						if(dirName == key) {
							isExcluded = true;
							break;
						}
					}
				}
				if(!isExcluded && cont)
					MyDLPDirectoryTraverse::TraverseDirectory(key, lvl + 1, fileSearch, detected);
            }
        }
	}
}