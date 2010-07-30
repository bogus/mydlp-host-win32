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
#include "MyDLPMD5File.h"

#using <System.Xml.dll>

using namespace System;
using namespace System::Xml;
using namespace System::Xml::Schema;
using namespace System::Xml::Serialization;
using namespace System::IO;


namespace mydlpsf {

	public ref class MyDLPRemoteSensFileConf
	{
	private:
		static MyDLPRemoteSensFileConf ^sensFileConf;
		static void SetInstance(MyDLPRemoteSensFileConf ^sensFileConf);
		MyDLPRemoteSensFileConf(void);

	public:
		static MyDLPRemoteSensFileConf ^GetInstance();
		
		static void Serialize();
		static void Deserialize();

		System::Collections::Generic::List<MyDLPClamRegex ^> ^regexVal;
		System::Collections::Generic::List<MyDLPMD5File ^> ^md5Val;
        
		Boolean blockEncrypted;
		String ^blockEncrypted_ruleid;

		Boolean blockBroken;
		String ^blockBroken_ruleid;

		System::Collections::Generic::List<String^> ^fileGroups;
		String ^fileGroups_ruleid;

		Boolean enableCC;
		UInt32 maxCCCount;
		String ^enableCC_ruleid;
		
		Boolean enableRegex;
		UInt32 maxRegexCount;
		
		Boolean enableTRId;
		UInt32 maxTRIdCount;
		String ^enableTRId_ruleid;
		
		Boolean enableIBAN;
		UInt32 maxIBANCount;
		String ^enableIBAN_ruleid;
		
		Boolean enableSSN;
		UInt32 maxSSNCount;
		String ^enableSSN_ruleid;

		static const String ^confFileName = "sensfile.conf";
	};

}

#endif