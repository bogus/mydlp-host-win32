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

namespace mydlpsf
{
	public ref class MyDLPConfigurationUpdateListener
	{
	private:
		static MyDLPConfigurationUpdateListener ^listener;
		System::IO::FileSystemWatcher ^watcher;
		MyDLPConfigurationUpdateListener(void);
		System::Timers::Timer ^timer;
		String ^lastFileName;
		void ReleaseFileName( Object ^source, ElapsedEventArgs ^e );

	public:
		static MyDLPConfigurationUpdateListener ^GetInstance();
		void StartListener();
		void StopListener();
		void OnChanged(Object^ o, System::IO::FileSystemEventArgs^ e);
	};
}
