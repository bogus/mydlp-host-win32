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
using namespace System::Threading;
using namespace System::Runtime::CompilerServices;

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
			objectPool->objQueue = gcnew Queue();
			objectPool->isUpdating = false;				 
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
		CreateObject(poolSize);
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	void MyDLPSensFilePool::UpdatePool()
	{		
		try
		{
			isUpdating = true;
			version++;
			InitPool();
			Queue ^tmpObjQueue = gcnew Queue();
			for each (MyDLPSensitiveFileRecognition ^obj in objQueue)
			{
				if(obj->version < version)
					obj->Close();
				else
					tmpObjQueue->Enqueue(obj);
			}
			objQueue = tmpObjQueue;
		}
		catch(Exception ^ex)
		{
			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
		}
		finally
		{
			isUpdating = false;
		}
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	MyDLPSensitiveFileRecognition^ MyDLPSensFilePool::AcquireObject()
	{
		MyDLPSensitiveFileRecognition ^obj;
		try {
			if(objQueue->Count != 0) {
				do
				{
					obj = (MyDLPSensitiveFileRecognition ^)objQueue->Dequeue();
				}
				while(obj->version < version && objQueue->Count != 0);
				
				if(obj->version == version)
					return obj;
			}
		} 
		catch (InvalidOperationException ^ex) 
		{
			MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
		} 
		finally
		{
			CreateObject(3);	
		}
		return (MyDLPSensitiveFileRecognition^)objQueue->Dequeue();
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	void MyDLPSensFilePool::ReleaseObject(MyDLPSensitiveFileRecognition ^object)
	{
		
		if(object != nullptr && object->version == version)
			objQueue->Enqueue(object);
		
		if(object->version < version)
			object->Close();

	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	void MyDLPSensFilePool::DeleteObject(MyDLPSensitiveFileRecognition ^object)
	{
		object->Close();
		delete object;
		CreateObject(1);
	}
	
	void MyDLPSensFilePool::CreateObject(int count)
	{
		int i = 0;

		array<System::UInt32> ^ids = gcnew array<System::UInt32>(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->regexVal->Count);
		array<System::String ^> ^regex = gcnew array<System::String ^>(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->regexVal->Count);
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
			if(sensFileObject->Init(version) != 0)
				continue;
			if(sensFileObject->AddRegex(ids, regex, (int)regex->Length) != 0)
				continue;
			if(sensFileObject->AddMD5s(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->md5Val) != 0)
				continue;
			if(sensFileObject->AddIBAN() != 0)
				continue;
			if(sensFileObject->CompileClamEngine() != 0)
				continue;

			objQueue->Enqueue(sensFileObject);
		}
	}

	void MyDLPSensFilePool::SetMaxPoolSize(int size)
	{
		objectPool->poolSize = size;
	}
}