#pragma once
#pragma managed

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

using namespace System;

namespace mydlpsf
{
	public ref class MyDLPClamRegex
	{
	public:
		MyDLPClamRegex(void);

		UInt32 id;
		String ^regex;
	};
}