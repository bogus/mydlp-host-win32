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

#include "StdAfx.h"
#include "MyDLPFSMFListener.h"

using System::String;

namespace mydlpsf
{
	MyDLPFSMFListener::MyDLPFSMFListener(void)
	{
	}

	int MyDLPFSMFListener::Init()
	{
		DWORD requestCount = SCANNER_DEFAULT_REQUEST_COUNT;
		DWORD threadCount = SCANNER_DEFAULT_THREAD_COUNT;
		HANDLE threads[SCANNER_MAX_THREAD_COUNT];
		SCANNER_THREAD_CONTEXT context;
		HANDLE port, completion;
		PSCANNER_MESSAGE msg;
		DWORD threadId;
		HRESULT hr;
		DWORD i, j;
	
		

		hr = FilterConnectCommunicationPort( ScannerPortName, 0, NULL, 0, NULL, &port );

		if (IS_ERROR( hr )) {
			return 2;
		}

		completion = CreateIoCompletionPort( port, NULL, 0, threadCount );

		if (completion == NULL) {
			CloseHandle( port );
			return 3;
		}

		context.Port = port;
		context.Completion = completion;

		for (i = 0; i < threadCount; i++) {
			threads[i] = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ScannerWorker, &context, 0, &threadId );
			if (threads[i] == NULL) {
				hr = GetLastError();
				goto init_cleanup;
			}
			for (j = 0; j < requestCount; j++) {
#pragma prefast(suppress:__WARNING_MEMORY_LEAK, "msg will not be leaked because it is freed in ScannerWorker")
				msg = (PSCANNER_MESSAGE)malloc( sizeof( SCANNER_MESSAGE ) );
				if (msg == NULL) {
					hr = ERROR_NOT_ENOUGH_MEMORY;
					goto init_cleanup;
				}
				memset( &msg->Ovlp, 0, sizeof( OVERLAPPED ) );
				hr = FilterGetMessage( port, &msg->MessageHeader,
									   FIELD_OFFSET( SCANNER_MESSAGE, Ovlp ), &msg->Ovlp );

				if (hr != HRESULT_FROM_WIN32( ERROR_IO_PENDING )) {
					free( msg );
					goto init_cleanup;
				}
			}
		}

		hr = S_OK;

		WaitForMultipleObjectsEx( i, threads, TRUE, INFINITE, FALSE );

	init_cleanup:

		printf( "Scanner:  All done. Result = 0x%08x\n", hr );

		CloseHandle( port );
		CloseHandle( completion );

		return hr;
	}
}

DWORD ScannerWorker(__in PSCANNER_THREAD_CONTEXT Context) 
{
	PSCANNER_NOTIFICATION notification;
	SCANNER_REPLY_MESSAGE replyMessage;
	PSCANNER_MESSAGE message;
	LPOVERLAPPED pOvlp;
	BOOL result;
	DWORD outSize;
	HRESULT hr;
	ULONG_PTR key;

#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant
	while (TRUE) {
#pragma warning(pop)
		result = GetQueuedCompletionStatus( Context->Completion, &outSize, &key, &pOvlp, INFINITE );
		message = CONTAINING_RECORD( pOvlp, SCANNER_MESSAGE, Ovlp );

		if (!result) {
			hr = HRESULT_FROM_WIN32( GetLastError() );
			break;
		}
		notification = &message->Notification;

		result = ScanFile( notification->Contents, notification->BytesToScan, 
			notification->FileName, notification->FileNameLength, notification->Phase );

		replyMessage.ReplyHeader.Status = 0;
		replyMessage.ReplyHeader.MessageId = message->MessageHeader.MessageId;
		replyMessage.Reply.SafeToOpen = !result;

		//if(replyMessage.Reply.SafeToOpen == 0)
		//	printf( "Replying message, SafeToOpen: %d\n", replyMessage.Reply.SafeToOpen );
		
		hr = FilterReplyMessage( Context->Port,
								 (PFILTER_REPLY_HEADER) &replyMessage,
								 sizeof( SCANNER_REPLY_MESSAGE ) );

		if (SUCCEEDED( hr )) {
			//printf( "Replied message\n" );
		} else {
			printf( "Scanner: Error replying message. Error = 0x%X\n", hr );
			break;
		}

		memset( &message->Ovlp, 0, sizeof( OVERLAPPED ) );

		hr = FilterGetMessage( Context->Port, &message->MessageHeader, 
								FIELD_OFFSET( SCANNER_MESSAGE, Ovlp ), &message->Ovlp );

		if (hr != HRESULT_FROM_WIN32( ERROR_IO_PENDING )) {
			break;
		}
	}

	if (!SUCCEEDED( hr )) {

		if (hr == HRESULT_FROM_WIN32( ERROR_INVALID_HANDLE )) {
			printf( "Scanner: Port is disconnected, probably due to scanner filter unloading.\n" );
		} else {
			printf( "Scanner: Unknown error occured. Error = 0x%X\n", hr );
		}
	}
	free( message );
	return hr;
}

BOOL ScanFile (__in_bcount(BufferSize) PUCHAR Buffer, __in ULONG BufferSize,
			   __in_bcount(FileNameLength) PWCHAR FileName, __in ULONG FileNameLength, 
			   __in USHORT Phase)
{
	FILE *fp;
	TEMPFILE_INFO *tempFileInfo;
	std::string tempFileName;

	tempFileInfo = ScanMMap(FileName, FileNameLength, false);

	if(Phase == 1) {
		
		if(tempFileInfo == NULL) {
			tempFileInfo = (TEMPFILE_INFO *)malloc(sizeof(TEMPFILE_INFO));
			tempFileName = _tempnam("c:\\tmp", "mydlptmp");
			tempFileName += (std::string)(const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(Guid::NewGuid().ToString() 
				+ System::IO::Path::GetExtension(gcnew System::String(FileName)))).ToPointer();
			tempFileInfo->filename = (char *) malloc(sizeof(char) * tempFileName.length());
			strcpy(tempFileInfo->filename, tempFileName.c_str());
			fopen_s(&fp,tempFileInfo->filename, "wb");
			tempFileInfo->tmpfd = fp;
			mMap.insert(MWchTmpPair(FileName, tempFileInfo));
		} else {
			fopen_s(&fp,tempFileInfo->filename, "ab");
			tempFileInfo->tmpfd = fp;
		}
		fwrite(Buffer, sizeof(UCHAR), BufferSize, tempFileInfo->tmpfd);
		fflush(tempFileInfo->tmpfd);
		fclose(tempFileInfo->tmpfd);
		
		printf("---- FileName: %S ---- \n", FileName);

		mydlpsf::MyDLPSensitiveFileRecognition ^recObj =  mydlpsf::MyDLPSensFilePool::GetInstance()->AcquireObject();
		int ret = recObj->SearchSensitiveData(gcnew System::String(tempFileInfo->filename));

		if(ret == 1) {
			mydlpsf::MyDLPEventLogger::GetInstance()->LogSensFile("File transfer blocked: " + 
				gcnew String(FileName) + " -- " + recObj->GetLastResult());
			mydlpsf::MyDLPSensFilePool::GetInstance()->ReleaseObject(recObj);
			return TRUE;
		}	

		mydlpsf::MyDLPSensFilePool::GetInstance()->ReleaseObject(recObj);

	} else if(Phase == 2) {
		
		if(tempFileInfo != NULL) {
			fclose(tempFileInfo->tmpfd);
			_unlink(tempFileInfo->filename);
			tempFileInfo = ScanMMap(FileName, FileNameLength, true);
		}
	}

	return FALSE;
}

TEMPFILE_INFO* ScanMMap(__in_bcount(FileNameLength) PWCHAR FileName, __in ULONG FileNameLength,
			  __in BOOLEAN deletePair) 
{
	MWchTmpMap::iterator p;
	BOOL found = false;

	for(p = mMap.begin(); p!= mMap.end(); ++p)
	{
		if(wcsncmp(p->first, FileName, FileNameLength) == 0) {
			if(deletePair) {
				mMap.erase(p);
				return NULL;
			}
			else
				return p->second;
		}
	}

	return NULL;
}
