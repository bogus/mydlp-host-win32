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

#include "MyDLPSensitiveFileRecognition.h"
#include "MyDLPSensFilePool.h"

namespace mydlpsf
{
	public ref class MyDLPDirectoryTraverse
	{
	private:
		static void TraverseDirectory(String ^path, UInt32 lvl, MyDLPSensitiveFileRecognition ^fileSearch, String ^%detected);

	public:
		MyDLPDirectoryTraverse(void);
		static void TraverseAllDrives();
		static void TraverseDir(String ^path);
		
	};
}
#endif