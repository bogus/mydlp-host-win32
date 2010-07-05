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
#include "MyDLPSensFilePool.h"
#include "MyDLPSensitiveFileRecognition.h"
#include "MyDLPEventLogger.h"

using namespace System;
using namespace Microsoft::Win32;

namespace mydlpsf
{
	MyDLPSensFilePool::MyDLPSensFilePool(void)
	{
		objectPool = nullptr;
	}
	
	MyDLPSensFilePool ^MyDLPSensFilePool::GetInstance()
	{
		if(objectPool == nullptr) {
			objectPool = gcnew MyDLPSensFilePool();
			objectPool->poolSize = MYDLP_SENS_FILE_OBJ_POOL_SIZE;
			objectPool->objQueue = gcnew Queue(objectPool->poolSize);

			MyDLPSensitiveFileRecognition::clamFileGroupOptions = gcnew Hashtable();
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("ARCHIVE", CL_SCAN_ARCHIVE);
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("MAIL", CL_SCAN_MAIL);
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("OLE2", CL_SCAN_OLE2);
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("HTML", CL_SCAN_HTML);
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("PE", CL_SCAN_PE);			
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("PDF", CL_SCAN_PDF);
			MyDLPSensitiveFileRecognition::clamFileGroupOptions->Add("PS", CL_SCAN_PS);
		}

		return objectPool;	
	}

	void MyDLPSensFilePool::InitPool()
	{
		array<MyDLPSensitiveFileRecognition ^> ^objArray = CreateObject(poolSize);
		for each(MyDLPSensitiveFileRecognition ^obj in objArray)
		{
			objQueue->Enqueue(obj);
		}
	}

	void MyDLPSensFilePool::UpdatePool()
	{		
		for each (MyDLPSensitiveFileRecognition ^obj in objQueue)
		{
			obj->Close();
		}
		objectPool->objQueue = gcnew Queue(objectPool->poolSize);
		InitPool();
	}

	MyDLPSensitiveFileRecognition^ MyDLPSensFilePool::AcquireObject()
	{
		try {
			if(objQueue->Count != 0)
				return (MyDLPSensitiveFileRecognition ^)objQueue->Dequeue();
		} 
		catch (InvalidOperationException ^ex) 
		{
			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
		} 
		
		return CreateObject(1)[0];
		
	}

	void MyDLPSensFilePool::ReleaseObject(MyDLPSensitiveFileRecognition ^object)
	{
		if(object != nullptr)
			objQueue->Enqueue(object);
	}

	void MyDLPSensFilePool::DeleteObject(MyDLPSensitiveFileRecognition ^object)
	{
		object->Close();
		delete object;
		objQueue->Enqueue(CreateObject(1)[0]);		
	}

	array<MyDLPSensitiveFileRecognition ^> ^MyDLPSensFilePool::CreateObject(int count)
	{
		int i = 0;
		RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
		
		MyDLPRemoteSensFileConf::Deserialize();

		array<System::UInt32> ^ids = gcnew array<System::UInt32>(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->regexVal->Count);
		array<System::String ^> ^regex = gcnew array<System::String ^>(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->regexVal->Count);
		array<MyDLPSensitiveFileRecognition ^> ^objArray = gcnew array<MyDLPSensitiveFileRecognition ^>(count);
		MyDLPSensitiveFileRecognition ^sensFileObject;
		
		for each(MyDLPClamRegex ^clamRegex in mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->regexVal)
		{
			ids[i] = clamRegex->id;
			regex[i] = clamRegex->regex;
			i++;
		}

		for(i = 0 ; i < count ; i++)
		{
			sensFileObject = gcnew MyDLPSensitiveFileRecognition();
			sensFileObject->Init();
			sensFileObject->AddRegex(ids, regex, (int)regex->Length);
			sensFileObject->AddMD5s(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->md5Val);
			sensFileObject->AddIBAN();
			sensFileObject->CompileClamEngine();
			objArray[i] = sensFileObject;
		}

		return objArray;
	}

	void MyDLPSensFilePool::SetMaxPoolSize(int size)
	{
		objectPool->poolSize = size;
	}
}