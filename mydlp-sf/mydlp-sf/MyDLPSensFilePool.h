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
		void AddObjectToQueue(int count);

	public:
		static MyDLPSensFilePool ^GetInstance();
		void InitPool();
		void UpdatePool();
		MyDLPSensitiveFileRecognition ^AcquireObject();
		void ReleaseObject(MyDLPSensitiveFileRecognition ^object);
		void DeleteObject(MyDLPSensitiveFileRecognition ^object);
		void SetMaxPoolSize(int size);
	};

}

#endif