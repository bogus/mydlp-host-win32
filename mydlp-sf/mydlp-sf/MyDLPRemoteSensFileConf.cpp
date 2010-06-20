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
#include "MyDLPRemoteSensFileConf.h"

namespace mydlpsf {

	MyDLPRemoteSensFileConf::MyDLPRemoteSensFileConf(void)
	{
		regexVal = gcnew System::Collections::Generic::List<MyDLPClamRegex ^>();
		md5Val = "";
		excludedDirs = gcnew System::Collections::Generic::List<String ^>();
		blockEncrypted = true;
		blockBroken = true;
		fileGroups = gcnew System::Collections::Generic::List<String ^>();
		enableCC = true;
		maxCCCount = 1;
		enableRegex = true;
		maxRegexCount = 1;
		enableTRId = true;
		maxTRIdCount = 1;
		enableIBAN = true;
		maxIBANCount = 1;
		enableSSN = true;
		maxSSNCount = 1;		
	}

	MyDLPRemoteSensFileConf ^MyDLPRemoteSensFileConf::GetInstance()
	{
		if(sensFileConf == nullptr)
		{
			sensFileConf = gcnew MyDLPRemoteSensFileConf();
		}

		return sensFileConf;
	}

	void MyDLPRemoteSensFileConf::SetInstance(MyDLPRemoteSensFileConf ^sensFileConf)
	{
		mydlpsf::MyDLPRemoteSensFileConf::sensFileConf = sensFileConf;
	}
}
