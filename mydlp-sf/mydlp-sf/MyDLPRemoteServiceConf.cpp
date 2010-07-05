#include "StdAfx.h"
#include "MyDLPRemoteServiceConf.h"

using namespace Microsoft::Win32;

namespace mydlpsf
{
	MyDLPRemoteServiceConf::MyDLPRemoteServiceConf(void)
	{
		language = "en-US";
		remoteServer = "10.0.0.1";
		showPopupNotification = true;
	}

	MyDLPRemoteServiceConf ^MyDLPRemoteServiceConf::GetInstance()
	{
		if(serviceConf == nullptr)
		{
			serviceConf = gcnew MyDLPRemoteServiceConf();
		}

		return serviceConf;
	}

	void MyDLPRemoteServiceConf::SetInstance(MyDLPRemoteServiceConf ^serviceConf)
	{
		mydlpsf::MyDLPRemoteServiceConf::serviceConf = serviceConf;
	}

	void MyDLPRemoteServiceConf::Serialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteServiceConf::GetInstance()->GetType());
			TextWriter ^writer = gcnew StreamWriter(filename);
			serializer->Serialize(writer, MyDLPRemoteServiceConf::GetInstance());
			writer->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteServiceConf::Serialize", ex);
		}
	}

	void MyDLPRemoteServiceConf::Deserialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteServiceConf::GetInstance()->GetType());
			FileStream ^fs = gcnew FileStream(filename, FileMode::Open);
			XmlReader ^reader = gcnew XmlTextReader(fs);
			MyDLPRemoteServiceConf::SetInstance(dynamic_cast<MyDLPRemoteServiceConf ^> (serializer->Deserialize(reader)));
			fs->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteServiceConf::Deserialize", ex);
		}
	}
}
