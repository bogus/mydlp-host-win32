#pragma once
#pragma managed 

#include <windows.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <winioctl.h>
#include <string.h>
#include <crtdbg.h>
#include <fltUser.h>
#include "MyDLPSensFilePool.h"
#include "MyDLPScannerInc.h"
#include <iostream>
#include <map>
#include <string>

#define SCANNER_DEFAULT_REQUEST_COUNT       5
#define SCANNER_DEFAULT_THREAD_COUNT        2
#define SCANNER_MAX_THREAD_COUNT            64

// Structures
typedef struct _SCANNER_THREAD_CONTEXT {
	HANDLE Port;
	HANDLE Completion;
} SCANNER_THREAD_CONTEXT, *PSCANNER_THREAD_CONTEXT;

typedef struct _SCANNER_MESSAGE {
	FILTER_MESSAGE_HEADER MessageHeader;
	SCANNER_NOTIFICATION Notification;
	OVERLAPPED Ovlp;
} SCANNER_MESSAGE, *PSCANNER_MESSAGE;

typedef struct _TEMPFILE_INFO {
	FILE *tmpfd;
	CHAR *filename;
} TEMPFILE_INFO;

typedef std::map<PWCHAR, TEMPFILE_INFO *> MWchTmpMap;
typedef std::pair<PWCHAR, TEMPFILE_INFO *> MWchTmpPair;

MWchTmpMap mMap;

// Function prototypes
DWORD ScannerWorker(__in PSCANNER_THREAD_CONTEXT Context);
BOOL ScanFile (__in_bcount(BufferSize) PUCHAR Buffer, __in ULONG BufferSize,
			   __in_bcount(FileNameLength) PWCHAR FileName, __in ULONG FileNameLength, 
			   __in USHORT Phase);
TEMPFILE_INFO* ScanMMap(__in_bcount(FileNameLength) PWCHAR FileName, __in ULONG FileNameLength,
			  __in BOOLEAN deletePair);

namespace mydlpsf
{

	public ref class MyDLPFSMFListener
	{
	public:
		MyDLPFSMFListener(void);
		int Init();
	};
}
