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
#ifndef __MYDLP_FSMF_LISTENER__
#define __MYDLP_FSMF_LISTENER__

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
#include "MyDLPEventLogger.h"
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
#endif