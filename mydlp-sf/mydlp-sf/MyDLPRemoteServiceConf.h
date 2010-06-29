#pragma once
#pragma managed

using namespace System;
using namespace System::Xml;
using namespace System::Xml::Schema;
using namespace System::Xml::Serialization;
using namespace System::IO;

namespace mydlpsf
{
	public ref class MyDLPRemoteServiceConf
	{
	private:
		static MyDLPRemoteServiceConf ^serviceConf;
		static void SetInstance(MyDLPRemoteServiceConf ^serviceConf);
		MyDLPRemoteServiceConf(void);

	public:
		static MyDLPRemoteServiceConf ^GetInstance();
		static void Serialize();
		static void Deserialize();

		String ^language;
		String ^remoteServer;

		static const String ^confFileName = "service.conf";
	};
}
