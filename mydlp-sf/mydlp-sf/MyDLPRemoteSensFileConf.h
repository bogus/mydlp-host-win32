#ifndef __MYDLP_REMSENSCONF_H_
#define __MYDLP_REMSENSCONF_H_

#pragma once
#pragma managed 

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "MyDLPClamRegex.h"

using namespace System;

namespace mydlpsf {

	public ref class MyDLPRemoteSensFileConf
	{
	public:
		MyDLPRemoteSensFileConf(void);

		static System::Collections::Generic::List<MyDLPClamRegex ^> ^regexVal = 
			gcnew System::Collections::Generic::List<MyDLPClamRegex ^>();
        static String^ md5Val = "";
		static System::Collections::Generic::List<String^> ^excludedDirs = 
			gcnew System::Collections::Generic::List<String ^>();
        
		static Boolean blockEncrypted = true;
		static Boolean blockBroken = true;

		static System::Collections::Generic::List<String^> ^fileGroups = 
			gcnew System::Collections::Generic::List<String ^>();

		static Boolean enableCC = true;
		static UInt32 maxCCCount = 1;
		
		static Boolean enableRegex = true;
		static UInt32 maxRegexCount = 1;
		
		static Boolean enableTRId = true;
		static UInt32 maxTRIdCount = 1;
		
		static Boolean enableIBAN = true;
		static UInt32 maxIBANCount = 1;
		
		static Boolean enableSSN = true;
		static UInt32 maxSSNCount = 1;		
	};

}

#endif