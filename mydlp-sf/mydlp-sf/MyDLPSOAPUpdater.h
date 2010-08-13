#pragma once
#pragma managed

#include "MyDLPRuleManager.h"
#include "MyDLPRuleVersion.h"

using namespace System;
using namespace System::Timers;

namespace mydlpsf
{
	public ref class MyDLPSOAPUpdater
	{
	private:
		MyDLPSOAPUpdater(void);
		static MyDLPSOAPUpdater ^obj;
		System::Timers::Timer ^timer;
		void Update( Object ^source, ElapsedEventArgs ^e );
		
		String ^RuleToString(soap::MyDLPRule ^rule);

	public:
		static MyDLPSOAPUpdater ^GetInstance();
		void StartUpdater();
		void StopUpdater();
		void UpdateRules();
	};
}