#include "StdAfx.h"
#include "MyDLPRemoteRules.h"

using namespace Microsoft::Win32;
using namespace System::Xml;
using namespace System::Xml::Schema;
using namespace System::Xml::Serialization;
using namespace System::IO;
using namespace System::Collections;

namespace mydlpsf {
	MyDLPRemoteRules::MyDLPRemoteRules(void)
	{
		rules = gcnew Generic::List<MyDLPRule ^>();
	}

	MyDLPRemoteRules ^MyDLPRemoteRules::GetInstance()
	{
		if(rulesConf == nullptr)
		{
			rulesConf = gcnew MyDLPRemoteRules();
		}

		return rulesConf;
	}

	void MyDLPRemoteRules::SetInstance(MyDLPRemoteRules ^rulesConf)
	{
		mydlpsf::MyDLPRemoteRules::rulesConf = rulesConf;
	}

	void MyDLPRemoteRules::Serialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteRules::GetInstance()->GetType());
			TextWriter ^writer = gcnew StreamWriter(filename);
			serializer->Serialize(writer, MyDLPRemoteRules::GetInstance());
			writer->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteRules::Serialize", ex);
		}
	}

	void MyDLPRemoteRules::Deserialize()
	{
		try
		{
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^filename = key->GetValue("Config_Dir") + "\\" + confFileName;
			XmlSerializer ^serializer = gcnew XmlSerializer(MyDLPRemoteRules::GetInstance()->GetType());
			FileStream ^fs = gcnew FileStream(filename, FileMode::Open, FileAccess::ReadWrite);
			XmlReader ^reader = gcnew XmlTextReader(fs);
			MyDLPRemoteRules::SetInstance(dynamic_cast<MyDLPRemoteRules ^> (serializer->Deserialize(reader)));
			fs->Close();
			reader->Close();
		} 
		catch(Exception ^ex)
		{
			throw gcnew Exception("MyDLPRemoteRules::Deserialize", ex);
		}
	}
}