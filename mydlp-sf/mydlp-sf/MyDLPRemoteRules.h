/* 
 *  Copyright (C) 2010 Medra Teknoloji
 *
 *  Authors: Burak OGUZ <burak@medra.com.tr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */

#pragma once
#pragma managed

#ifndef __MYDLP_REM_RULES_CONF__
#define __MYDLP_REM_RULES_CONF__

#include "MyDLPRule.h"

using namespace System;

namespace mydlpsf {

	public ref class MyDLPRemoteRules
	{
	private:
		static MyDLPRemoteRules ^rulesConf;
		static void SetInstance(MyDLPRemoteRules ^rulesConf);
		MyDLPRemoteRules(void);

	public:
		static MyDLPRemoteRules ^GetInstance();
		static void Serialize();
		static void Deserialize();

		Collections::Generic::List<MyDLPRule ^> ^rules;

		static const String ^confFileName = "rules.conf";
	};

}

#endif