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
#include "MyDLPRemoteDeviceConf.h"

using namespace Microsoft::Win32;

namespace mydlpsf {
	MyDLPRemoteDeviceConf::MyDLPRemoteDeviceConf(void)
	{
		excludedDirs = gcnew System::Collections::Generic::List<String ^>();
		enableRemovableOnlineScanning = true;
		justLogRemovableOnlineScanning = false;
		
		scanInsertedLogical = true;
		scanPluggedInRemovableDevices = true;
		
		filterPSD = true;
		filterPSP = true;
		filterCDR = true;
		filterDWG = true;
	}

	MyDLPRemoteDeviceConf ^MyDLPRemoteDeviceConf::GetInstance()
	{
		if(deviceConf == nullptr)
		{
			deviceConf = gcnew MyDLPRemoteDeviceConf();
		}

		return deviceConf;
	}

	void MyDLPRemoteDeviceConf::SetInstance(MyDLPRemoteDeviceConf ^deviceConf)
	{
		mydlpsf::MyDLPRemoteDeviceConf::deviceConf = deviceConf;
	}

	void MyDLPRemoteDeviceConf::Serialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteDeviceConf::GetInstance()->GetType());
			TextWriter ^writer = gcnew StreamWriter(filename);
			serializer->Serialize(writer, MyDLPRemoteDeviceConf::GetInstance());
			writer->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteDeviceConf::Serialize", ex);
		}
	}

	void MyDLPRemoteDeviceConf::Deserialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteDeviceConf::GetInstance()->GetType());
			FileStream ^fs = gcnew FileStream(filename, FileMode::Open);
			XmlReader ^reader = gcnew XmlTextReader(fs);
			MyDLPRemoteDeviceConf::SetInstance(dynamic_cast<MyDLPRemoteDeviceConf ^> (serializer->Deserialize(reader)));
			fs->Close();
			reader->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteDeviceConf::Deserialize", ex);
		}
	}
}