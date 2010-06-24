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

#ifndef __MYDLP_REMOTESCREENCAPCONF_H_
#define __MYDLP_REMOTESCREENCAPCONF_H_

#pragma once
#pragma managed

using namespace System;
using namespace System::Xml;
using namespace System::Xml::Schema;
using namespace System::Xml::Serialization;
using namespace System::IO;

namespace mydlpsf
{
	public ref class MyDLPRemoteScreenCaptureConf
	{
	private:
		static MyDLPRemoteScreenCaptureConf ^screenCaptureConf;
		static void SetInstance(MyDLPRemoteScreenCaptureConf ^screenCaptureConf);

	public:
		MyDLPRemoteScreenCaptureConf(void);

		static MyDLPRemoteScreenCaptureConf ^GetInstance();
		
		static void Serialize();
		static void Deserialize();

		Boolean enableScreenCaptureFilter;
		Boolean forbidMSOffice;
		Boolean forbidOOOrg;
		Boolean forbidAcrobatReader;
		Boolean forbidPhotoshop;
		Boolean forbidAutoCAD;

		static const String ^confFileName = "screencapture.conf";
	};
}

#endif