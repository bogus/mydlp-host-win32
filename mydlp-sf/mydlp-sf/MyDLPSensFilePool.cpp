#include "StdAfx.h"
#include "MyDLPSensFilePool.h"
#include "MyDLPSensitiveFileRecognition.h"

using namespace System;

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
		int i = 0;
		array<System::UInt32> ^ids = gcnew array<System::UInt32>(MyDLPRemoteSensFileConf::regexVal->Count);
		array<System::String ^> ^regex = gcnew array<System::String ^>(MyDLPRemoteSensFileConf::regexVal->Count);
		MyDLPSensitiveFileRecognition ^sensFileObject;
		
		for each(MyDLPClamRegex ^clamRegex in MyDLPRemoteSensFileConf::regexVal)
		{
			ids[i] = clamRegex->id;
			regex[i] = clamRegex->regex;
			i++;
		}

		for(i = 0 ; i < objectPool->poolSize ; i++)
		{
			sensFileObject = gcnew MyDLPSensitiveFileRecognition();
			sensFileObject->Init();
			sensFileObject->AddRegex(ids, regex, (int)regex->Length);
			sensFileObject->AddMD5s(MyDLPRemoteSensFileConf::md5Val);
			sensFileObject->AddIBAN();
			sensFileObject->CompileClamEngine();
			objQueue->Enqueue(sensFileObject);
		}
	}

	void MyDLPSensFilePool::UpdatePool()
	{
		int i = 0;
		array<System::UInt32> ^ids = gcnew array<System::UInt32>(MyDLPRemoteSensFileConf::regexVal->Count);
		array<System::String ^> ^regex = gcnew array<System::String ^>(MyDLPRemoteSensFileConf::regexVal->Count);
		MyDLPSensitiveFileRecognition ^sensFileObject;
		
		objectPool->objQueue = gcnew Queue(objectPool->poolSize);

		for each(MyDLPClamRegex ^clamRegex in MyDLPRemoteSensFileConf::regexVal)
		{
			ids[i] = clamRegex->id;
			regex[i] = clamRegex->regex;
			i++;
		}

		for(i = 0 ; i < objectPool->poolSize ; i++)
		{
			sensFileObject = gcnew MyDLPSensitiveFileRecognition();
			sensFileObject->Init();
			sensFileObject->AddRegex(ids, regex, (int)regex->Length);
			sensFileObject->AddMD5s(MyDLPRemoteSensFileConf::md5Val);
			sensFileObject->AddIBAN();
			sensFileObject->CompileClamEngine();
			objQueue->Enqueue(sensFileObject);
		}
	}

	MyDLPSensitiveFileRecognition^ MyDLPSensFilePool::AcquireObject()
	{
		return (MyDLPSensitiveFileRecognition ^)objQueue->Dequeue();
	}

	void MyDLPSensFilePool::ReleaseObject(MyDLPSensitiveFileRecognition ^object)
	{
		objQueue->Enqueue(object);
	}

	void MyDLPSensFilePool::SetMaxPoolSize(int size)
	{
		objectPool->poolSize = size;
	}
}