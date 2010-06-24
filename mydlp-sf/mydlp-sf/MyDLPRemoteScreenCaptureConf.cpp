#include "StdAfx.h"
#include "MyDLPRemoteScreenCaptureConf.h"

using namespace Microsoft::Win32;

namespace mydlpsf
{
	MyDLPRemoteScreenCaptureConf::MyDLPRemoteScreenCaptureConf(void)
	{
		enableScreenCaptureFilter = true;
		forbidMSOffice = true;
		forbidOOOrg = true;
		forbidAcrobatReader = true;
		forbidPhotoshop = true;
		forbidAutoCAD = true;
	}

	MyDLPRemoteScreenCaptureConf ^MyDLPRemoteScreenCaptureConf::GetInstance()
	{
		if(screenCaptureConf == nullptr)
		{
			screenCaptureConf = gcnew MyDLPRemoteScreenCaptureConf();
		}

		return screenCaptureConf;
	}

	void MyDLPRemoteScreenCaptureConf::SetInstance(MyDLPRemoteScreenCaptureConf ^deviceConf)
	{
		mydlpsf::MyDLPRemoteScreenCaptureConf::screenCaptureConf = screenCaptureConf;
	}

	void MyDLPRemoteScreenCaptureConf::Serialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteScreenCaptureConf::GetInstance()->GetType());
			TextWriter ^writer = gcnew StreamWriter(filename);
			serializer->Serialize(writer, MyDLPRemoteScreenCaptureConf::GetInstance());
			writer->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteScreenCaptureConf::Serialize", ex);
		}
	}

	void MyDLPRemoteScreenCaptureConf::Deserialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteScreenCaptureConf::GetInstance()->GetType());
			FileStream ^fs = gcnew FileStream(filename, FileMode::Open);
			XmlReader ^reader = gcnew XmlTextReader(fs);
			MyDLPRemoteScreenCaptureConf::SetInstance(dynamic_cast<MyDLPRemoteScreenCaptureConf ^> (serializer->Deserialize(reader)));
			fs->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteScreenCaptureConf::Deserialize", ex);
		}
	}
}
