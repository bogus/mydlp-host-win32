#pragma once

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
		void AddMessage(String ^message);
		String ^GetLastMessage();

		event EventHandler ^NewMessageArrived;
	};
}