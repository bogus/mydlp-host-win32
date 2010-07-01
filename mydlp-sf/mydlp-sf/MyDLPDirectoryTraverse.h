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
#ifndef __MYDLP_DIR_TRAVERSE__
#define __MYDLP_DIR_TRAVERSE__

#pragma once
#pragma managed

#include "MyDLPRemoteConf.h"
#include "MyDLPSensFilePool.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace mydlpsf
{
	public ref class MyDLPDirectoryTraverse
	{
	private:
		void TraverseDirectory(Object ^obj, Object ^origPath);
		void Traverse(Object ^obj);

		static List<MyDLPDirectoryTraverse ^> ^traverseList = gcnew List<MyDLPDirectoryTraverse ^>();

		Boolean ^cont;
		Boolean ^completed;
		Thread ^thread;
	
	protected:
		virtual void OnCompleted(EventArgs ^e); 
		virtual void OnDetectedChanged(EventArgs ^e); 

	public:
		MyDLPDirectoryTraverse(void);
		
		void StopScan();
		static List<MyDLPDirectoryTraverse ^> ^TraverseAllDrives();
		static List<MyDLPDirectoryTraverse ^> ^TraverseDir(String ^path);
		static void StopAllScans();

		String ^detected;

		event EventHandler ^Completed;
		event EventHandler ^DetectedChanged;
		
	};
}
#endif