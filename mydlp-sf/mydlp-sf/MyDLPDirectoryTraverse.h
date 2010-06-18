#pragma once
#pragma managed

#include "MyDLPSensitiveFileRecognition.h"
#include "MyDLPSensFilePool.h"

namespace mydlpsf
{
	public ref class MyDLPDirectoryTraverse
	{
	private:
		static void TraverseDirectory(String ^path, UInt32 lvl, MyDLPSensitiveFileRecognition ^fileSearch, String ^%detected);

	public:
		MyDLPDirectoryTraverse(void);
		static void TraverseAllDrives();
		static void TraverseDir(String ^path);
		
	};
}
