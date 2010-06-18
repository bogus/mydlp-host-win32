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
				if(MyDLPRemoteSensFileConf::excludedDirs->Count != 0)
				{
					for each(String ^dirName in MyDLPRemoteSensFileConf::excludedDirs)
					{
						if(dirName == key) {
							isExcluded = true;
							break;
						}
					}
				}
				if(!isExcluded)
					MyDLPDirectoryTraverse::TraverseDirectory(key, lvl + 1, fileSearch, detected);
            }
        }

	}
}