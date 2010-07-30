#pragma once
#pragma managed

#include "MyDLPRuleManager.h"

using namespace System;

namespace mydlpsf
{
	public ref class MyDLPSOAPUpdater
	{
	public:
		MyDLPSOAPUpdater(void);

		void UpdateRules();
		String ^RuleToString(soap::MyDLPRule ^rule);
	};
}