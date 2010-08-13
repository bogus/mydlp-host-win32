#pragma once

#include "MyDLPLog.h"

using namespace System;
using namespace System::Diagnostics;

namespace mydlpsf
{
	public ref class MyDLPMessages
	{
	private:
		static MyDLPMessages ^messages;
		MyDLPMessages(void);
		System::Collections::Generic::Queue<String ^> ^lastMessages;
		EventLog ^eventLog;

	protected:
		virtual void OnNewMessageArrived(EventArgs ^e); 

	public:
		static MyDLPMessages ^GetInstance();
		void AddMessage(String ^message, soap::MyDLPLogIncedent ^log);
		String ^GetLastMessage();

		event EventHandler ^NewMessageArrived;
	};
}