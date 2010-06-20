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