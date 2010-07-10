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
#include "MyDLPTempFileManager.h"
#include "MyDLPEventLogger.h"

using namespace System::IO;

namespace mydlpsf
{
	MyDLPTempFileManager::MyDLPTempFileManager(void)
	{
	}

	MyDLPTempFileManager ^MyDLPTempFileManager::GetInstance()
	{
		if(tempFileManager == nullptr)
		{
			tempFileManager = gcnew MyDLPTempFileManager();
			tempFileManager->pathsToBeDeleted = gcnew List<String ^>();
			tempFileManager->deleteTimer = gcnew Timer();
			tempFileManager->deleteTimer->Interval = 10000;
			tempFileManager->deleteTimer->Elapsed += gcnew ElapsedEventHandler(tempFileManager, &MyDLPTempFileManager::DeleteTempFiles);
			tempFileManager->deleteTimer->Start();
		}

		return tempFileManager;
	}

	String ^MyDLPTempFileManager::GetTempFileName()
	{
		String ^tempStr = Path::GetTempFileName();
		if(File::Exists(tempStr))
			File::Delete(tempStr);
		tempStr = Path::GetDirectoryName(tempStr) + "\\mydlp-" + Path::GetFileNameWithoutExtension(tempStr) + ".myd";
		return tempStr;
	}

	String ^MyDLPTempFileManager::GetTempFileName(String ^extension)
	{
		String ^tempStr = Path::GetTempFileName();
		if(File::Exists(tempStr))
			File::Delete(tempStr);
		tempStr = Path::GetDirectoryName(tempStr) + "\\mydlp-" + Path::GetFileNameWithoutExtension(tempStr) + extension;
		return tempStr;
	}

	void MyDLPTempFileManager::DeleteFile(String ^path)
	{
		pathsToBeDeleted->Add(path);
	}

	void MyDLPTempFileManager::DeleteTempFiles( Object ^source, ElapsedEventArgs ^e )
	{
		List<String ^> ^deletedPaths = gcnew List<String ^>();

		for each (String ^path in pathsToBeDeleted)
		{
			if(File::Exists(path))
			{
				try
				{
					File::Delete(path);
					deletedPaths->Add(path);
				}
				catch(Exception ^ex)
				{
					MyDLPEventLogger::GetInstance()->LogError("File not deleted: " + path + Environment::NewLine + ex->StackTrace);
				}
			}
			else
			{
				deletedPaths->Add(path);
			}
		}

		for each (String ^path in deletedPaths)
		{
			pathsToBeDeleted->Remove(path);
		}

		array<String ^> ^tmpFiles = Directory::GetFiles(System::IO::Path::GetDirectoryName(Path::GetTempFileName()), "pdfbox*");
		for each (String ^path in tmpFiles) {
			if(File::Exists(path))
			{
				try
				{
					File::Delete(path);
				}
				catch(Exception ^ex)
				{
					MyDLPEventLogger::GetInstance()->LogError("File not deleted: " + path + ex->StackTrace);
				}
			}
		}
	}

}