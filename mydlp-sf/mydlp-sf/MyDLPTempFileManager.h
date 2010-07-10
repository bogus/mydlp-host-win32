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
#pragma once

using namespace System;
using namespace System::Timers;
using namespace System::Collections::Generic;

namespace mydlpsf
{
	public ref class MyDLPTempFileManager
	{
	private:
		MyDLPTempFileManager(void);
		static MyDLPTempFileManager ^tempFileManager;
		List<String ^> ^pathsToBeDeleted;
		System::Timers::Timer ^deleteTimer;

	public:
		static MyDLPTempFileManager ^GetInstance();
		String ^GetTempFileName();
		String ^GetTempFileName(String ^extension);
		void DeleteFile(String ^path);
		void DeleteTempFiles( Object ^source, ElapsedEventArgs ^e );
	};
}