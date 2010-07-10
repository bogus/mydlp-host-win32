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
#ifndef __MYDLP_SENS_FILE_REC__
#define __MYDLP_SENS_FILE_REC__

#pragma once
#pragma managed

#include "clamav.h" 
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "MyDLPRemoteSensFileConf.h"

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

namespace mydlpsf {

	public ref class MyDLPSensitiveFileRecognition
	{	
		private:
			struct cl_engine *engine;
			String^ result;
			String^ hdbFileName;
			std::string ManagedToSTL(String ^managed);
			String^ STLToManaged(std::string stl);
			UInt32 scanOptions;
			String ^GetShortFileName(String ^longName);

		public:
			int Init(int version);
			int AddRegex(array<System::UInt32> ^ids, array<System::String ^> ^regex, int count);
			int AddMD5s();
			int AddIBAN();
			int CompileClamEngine();
			int SearchSensitiveData(String ^filename);
			String^ GetLastResult();
			int Close();
			static System::Collections::Hashtable ^clamFileGroupOptions;
			int version;

	};
}

#endif