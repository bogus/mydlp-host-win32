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

namespace mydlpsf {

	public ref class MyDLPSensitiveFileRecognition
	{	
		private:
			struct cl_engine *engine;
			String^ result;
			std::string ManagedToSTL(String ^managed);
			String^ STLToManaged(std::string stl);
			UInt32 scanOptions;

		public:
			int Init();
			int AddRegex(array<System::UInt32> ^ids, array<System::String ^> ^regex, int count);
			int AddMD5s(String ^md5);
			int AddIBAN();
			int CompileClamEngine();
			int SearchSensitiveData(String ^filename);
			String^ GetLastResult();
			int Close();
			static System::Collections::Hashtable ^clamFileGroupOptions;
	};
}

