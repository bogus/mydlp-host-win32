#ifndef __MYDLP_SENSFILEPOOL_H__
#define __MYDLP_SENSFILEPOOL_H__

#pragma once
#pragma managed

#include "MyDLPClamRegex.h"
#include "MyDLPSensitiveFileRecognition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "clamav.h"


using namespace System;
using namespace System::Collections;

#define MYDLP_SENS_FILE_OBJ_POOL_SIZE 10

namespace mydlpsf
{
	public ref class MyDLPSensFilePool
	{
	private:
		MyDLPSensFilePool(void);
		static MyDLPSensFilePool ^objectPool;
		Int32 poolSize;
		Queue ^objQueue;

	public:
		static MyDLPSensFilePool ^GetInstance();
		void InitPool();
		void UpdatePool();
		MyDLPSensitiveFileRecognition ^AcquireObject();
		void ReleaseObject(MyDLPSensitiveFileRecognition ^object);
		void SetMaxPoolSize(int size);
	};

}

#endif