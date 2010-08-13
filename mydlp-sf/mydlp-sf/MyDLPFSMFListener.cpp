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
#include "MyDLPMessages.h"
#include "MyDLPRemoteConf.h"
#include "MyDLPTempFileManager.h"
#include "MyDLPLog.h"

using namespace System::IO;
using namespace System::Threading;
using namespace System::Runtime::CompilerServices;

namespace mydlpsf
{
	MyDLPFSMFListener::MyDLPFSMFListener(void)
	{
	}

	int MyDLPFSMFListener::Init()
	{
		DWORD requestCount = SCANNER_DEFAULT_REQUEST_COUNT;
		DWORD threadCount = SCANNER_DEFAULT_THREAD_COUNT;
		SCANNER_THREAD_CONTEXT context;
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

		mydlpsf::MyDLPEventLogger::GetInstance()->LogError( "Scanner:  All done. Result = " + hr.ToString() );

		CloseHandle( port );
		CloseHandle( completion );

		return hr;
	}

	void MyDLPFSMFListener::StartCommunicationPort()
	{
		try
		{
			
			listener = gcnew MyDLPFSMFListener();
			listener->Init();
		
		} 
		catch(Exception ^ex)
		{
			mydlpsf::MyDLPEventLogger::GetInstance()->LogError( "StartCommunicationPort " + ex->StackTrace );
		}
	}

	void MyDLPFSMFListener::RunFilter()
	{
		if(!isRunning && MyDLPRemoteDeviceConf::GetInstance()->enableRemovableOnlineScanning.Equals(TRUE))
		{
			Thread ^thread = gcnew Thread(gcnew ThreadStart(MyDLPFSMFListener::StartCommunicationPort));
			thread->Start();
			isRunning = true;
		}
	}

	void MyDLPFSMFListener::StopFilter()
	{
		if(isRunning)
		{
			isRunning = false;

			CloseHandle( port );
			CloseHandle( completion );

			for(int i = 0; i < SCANNER_MAX_THREAD_COUNT; i++ )
				CloseHandle(threads[i]);

			delete listener;
		}
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
	
	try
	{
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
				notification->FileName, notification->FileNameLength, notification->Phase);

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
				mydlpsf::MyDLPEventLogger::GetInstance()->LogError( "Scanner: Error replying message. Error = " + hr.ToString() );
				break;
			}

			memset( &message->Ovlp, 0, sizeof( OVERLAPPED ) );

			hr = FilterGetMessage( Context->Port, &message->MessageHeader, 
									FIELD_OFFSET( SCANNER_MESSAGE, Ovlp ), &message->Ovlp );

			if (hr != HRESULT_FROM_WIN32( ERROR_IO_PENDING )) {
				break;
			}
		}
	}
	catch(Exception ^ex) {
		mydlpsf::MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
	}

	if (!SUCCEEDED( hr )) {
		System::String ^logStr;
		if (hr == HRESULT_FROM_WIN32( ERROR_INVALID_HANDLE )) {
			logStr = "Scanner: Port is disconnected, probably due to scanner filter unloading." + System::Environment::NewLine;
		} else {
			logStr = "Scanner: Unknown error occured. Error = " + hr.ToString() + System::Environment::NewLine;
		}
		mydlpsf::MyDLPEventLogger::GetInstance()->LogError(logStr);
	}

	free( message );
	return hr;
}

BOOL ScanFile (__in_bcount(BufferSize) PUCHAR Buffer, __in ULONG BufferSize,
			   __in_bcount(FileNameLength) PWCHAR FileName, __in ULONG FileNameLength, 
			   __in USHORT Phase)
{
	mydlpsf::MyDLPSensitiveFileRecognition ^recObj = nullptr;
	String ^filename = gcnew String(FileName);
	String ^tempFileName = String::Empty;
	FileStream ^fs;

	System::Threading::Monitor::Enter(mydlpsf::MyDLPFSMFListener::listener);
	try
	{
		if(Phase == 1) {
			
			if(!mydlpsf::MyDLPFSMFListener::tempFileMap->ContainsKey(filename)) {
				tempFileName = mydlpsf::MyDLPTempFileManager::GetInstance()->GetTempFileName(System::IO::Path::GetExtension(filename));				
				fs = gcnew FileStream(tempFileName, FileMode::CreateNew, FileAccess::Write);
				mydlpsf::MyDLPFSMFListener::tempFileMap->Add(filename, tempFileName);
			} else {
				tempFileName = mydlpsf::MyDLPFSMFListener::tempFileMap[filename];
				fs = gcnew FileStream(tempFileName, FileMode::Append, FileAccess::Write);
			}
			array<unsigned char> ^buffer = gcnew array<unsigned char>(BufferSize);
			for(unsigned int i = 0; i < BufferSize; i++)
				buffer[i] = Buffer[i];

			fs->Write(buffer, 0, BufferSize);
			fs->Flush();
			fs->Close();
			
			if(Buffer[BufferSize - 1] == 0 ||
				(Buffer[BufferSize - 1] == 10 && Buffer[BufferSize - 2] == 70 && Buffer[BufferSize - 3] == 79) ||
				cl_istext(Buffer, BufferSize) != 5) {
				
				while(mydlpsf::MyDLPSensFilePool::GetInstance()->isUpdating);
				recObj =  mydlpsf::MyDLPSensFilePool::GetInstance()->AcquireObject();
				int ret = recObj->SearchSensitiveData(tempFileName);
				
				if(ret == 1) {
					bool retVal = TRUE;

					if(mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterPSD.Equals(FALSE) &&
						recObj->GetLastResult()->ToLower()->Contains("photoshop"))
						retVal = FALSE;
					if(mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterDWG.Equals(FALSE) &&
						recObj->GetLastResult()->ToLower()->Contains("autocad"))
						retVal = FALSE;
					if(mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterPSP.Equals(FALSE) &&
						recObj->GetLastResult()->ToLower()->Contains("paintshop"))
						retVal = FALSE;
					if(mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterCDR.Equals(FALSE) &&
						recObj->GetLastResult()->ToLower()->Contains("corel"))
						retVal = FALSE;
				
					if(retVal) {
						mydlpsf::MyDLPEventLogger::GetInstance()->LogRemovable(filename + " -- " + recObj->GetLastResult());
						mydlpsf::soap::MyDLPLogIncedent ^log = gcnew mydlpsf::soap::MyDLPLogIncedent();
						log->matcher = "removable";
						if(recObj->GetLastResult()->ToLower()->Contains("creditcard")) {
							log->rule_id = mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableCC_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("ssn")) {
							log->rule_id = mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableSSN_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("trid")) {
							log->rule_id = mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableTRId_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("iban")) {
							log->rule_id = mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableIBAN_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("photoshop")) {
							log->rule_id = mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterPSD_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("autocad")) {
							log->rule_id = mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterDWG_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("paintshop")) {
							log->rule_id = mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterPSP_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("corel")) {
							log->rule_id = mydlpsf::MyDLPRemoteDeviceConf::GetInstance()->filterCDR_ruleid;
						} else if(recObj->GetLastResult()->ToLower()->Contains("regex")) {
							String ^split = "-";
							array<String ^> ^arr = recObj->GetLastResult()->Split(split->ToCharArray());
							UInt32 regexId = UInt32::Parse(arr[1]);
							for each(mydlpsf::MyDLPClamRegex ^regex in 
								mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->regexVal)
							{
								if(regex->id == regexId)
								{
									log->rule_id = regex->rule_id;
									break;
								}
							}
						} else if(recObj->GetLastResult()->ToLower()->Contains("__")) {
							String ^split = "__";
							array<String ^> ^arr = recObj->GetLastResult()->Split(split->ToCharArray());
							UInt32 fileId = UInt32::Parse(arr[1]);
							for each(mydlpsf::MyDLPMD5File ^file in 
								mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->md5Val)
							{
								if(file->id == fileId)
								{
									log->rule_id = file->rule_id;
									break;
								}
							}
						}
						log->destination = "USB / 1394";
						log->protocol = "LOCAL";
						log->filename = filename;
						log->misc = "File transfer to USB fitlered";
						mydlpsf::MyDLPMessages::GetInstance()->AddMessage(filename + " -- " + recObj->GetLastResult(), log);
						for each(mydlpsf::MyDLPRule ^rule in mydlpsf::MyDLPRemoteRules::GetInstance()->rules)
						{
							if(Convert::ToString(rule->id) == log->rule_id) {
								if(rule->action == "log")
									retVal = FALSE;
								break;
							}
						}
					}
					return retVal;
				}	
			} 
		} else if(Phase == 2) {
			if(mydlpsf::MyDLPFSMFListener::tempFileMap->ContainsKey(filename))
			{
				tempFileName = mydlpsf::MyDLPFSMFListener::tempFileMap[filename];
				if(tempFileName->Length > 0)
				{
					mydlpsf::MyDLPTempFileManager::GetInstance()->DeleteFile(tempFileName);
					mydlpsf::MyDLPFSMFListener::tempFileMap->Remove(filename);
				}
			}
		}
	}
	catch(IOException ^ex)
	{
		mydlpsf::MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
	}
	catch(Exception ^ex)
	{
		mydlpsf::MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
		if(recObj != nullptr)
			mydlpsf::MyDLPSensFilePool::GetInstance()->DeleteObject(recObj);
		recObj = nullptr;
	}
	finally
	{
		System::Threading::Monitor::Exit(mydlpsf::MyDLPFSMFListener::listener);
		if(recObj != nullptr)
			mydlpsf::MyDLPSensFilePool::GetInstance()->ReleaseObject(recObj);
	}
	return FALSE;
}
