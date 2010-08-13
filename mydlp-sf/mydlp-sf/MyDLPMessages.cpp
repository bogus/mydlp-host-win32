#include "StdAfx.h"
#include "MyDLPMessages.h"
#include "MyDLPRemoteConf.h"
#include "MyDLPTrustAllCertificatePolicy.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Management;
using namespace Microsoft::Win32;

namespace mydlpsf
{
	MyDLPMessages::MyDLPMessages(void)
	{
	}

	MyDLPMessages ^MyDLPMessages::GetInstance()
	{
		if(messages == nullptr)
		{
			messages = gcnew MyDLPMessages();
			messages->lastMessages = gcnew Queue<String ^>();

			messages->eventLog = gcnew EventLog();
			((System::ComponentModel::ISupportInitialize ^)(messages->eventLog))->BeginInit();
       
            messages->eventLog->Log = "MyDLPLog";
            messages->eventLog->Source = "MyDLPLogs-Service";
			
			messages->eventLog->ModifyOverflowPolicy(OverflowAction::OverwriteAsNeeded, 1);

			((System::ComponentModel::ISupportInitialize ^)(messages->eventLog))->EndInit();

			if(!EventLog::SourceExists("MyDLPLogs-Warnings"))
			{
				EventLog::CreateEventSource("MyDLPLogs-Warnings", "MyDLPLog");
			}

            messages->eventLog->Source = "MyDLPLogs-Warnings";
			messages->eventLog->Log ="MyDLPLog";
		}

		return messages;
	}

	void MyDLPMessages::AddMessage(String ^message, soap::MyDLPLogIncedent ^log)
	{
		if(lastMessages->Count > 10)
			lastMessages->Dequeue();

		lastMessages->Enqueue(message);

		eventLog->WriteEntry(message);

		if(log != nullptr)
		{
			System::Net::ServicePointManager::CertificatePolicy = gcnew MyDLPTrustAllCertificatePolicy();
			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );
			String ^username = dynamic_cast<String ^>(key->GetValue("remote_user"));
			String ^password = dynamic_cast<String ^>(key->GetValue("remote_pass"));
			String ^server = dynamic_cast<String ^>(key->GetValue("remote_server"));
			log->src_user = Environment::UserName;
			soap::MyDLPLog ^saveLog = gcnew soap::MyDLPLog();
			saveLog->Url = "https://"+server+"/mydlp-web-manager/service.php\?class=MyDLPLog";
			saveLog->logIncedent(log, username, password);
		}
		
		OnNewMessageArrived(EventArgs::Empty);
	}

	String ^MyDLPMessages::GetLastMessage()
	{
		return lastMessages->Peek();
	}

	void MyDLPMessages::OnNewMessageArrived(EventArgs ^e) 
    {
		this->NewMessageArrived(this, e);
	}
}
