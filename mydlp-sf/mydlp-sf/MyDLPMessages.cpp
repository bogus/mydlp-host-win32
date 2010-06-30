#include "StdAfx.h"
#include "MyDLPMessages.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;

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

	void MyDLPMessages::AddMessage(String ^message)
	{
		if(lastMessages->Count > 10)
			lastMessages->Dequeue();

		lastMessages->Enqueue(message);

		eventLog->WriteEntry(message);
		
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
