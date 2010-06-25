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
#include "MyDLPEventLogger.h"

using namespace Microsoft::Win32;

namespace mydlpsf
{
	MyDLPEventLogger::MyDLPEventLogger(void)
	{
	}

	MyDLPEventLogger ^MyDLPEventLogger::GetInstance()
	{
		if(eventLogger == nullptr)
		{
			eventLogger = gcnew MyDLPEventLogger();

			RegistryKey ^key = Registry::LocalMachine->OpenSubKey( "Software\\MyDLP" );

			Layout::ILayout ^patternLayout = gcnew Layout::PatternLayout("%-5p%d{yyyy-MM-dd hh:mm:ss} – %m\r\n");
			
			eventLogger->sensFileLogPath = key->GetValue("Log_Dir") + "\\" + "sens.log";
			Config::BasicConfigurator::Configure(LogManager::CreateDomain("sensfile"), 
				gcnew Appender::FileAppender(patternLayout, eventLogger->sensFileLogPath, true));
			eventLogger->logSensFile = LogManager::GetLogger("sensfile", "Sensifitive File Logging");
			
			eventLogger->deviceLogPath = key->GetValue("Log_Dir") + "\\" + "device.log";
			Config::BasicConfigurator::Configure(LogManager::CreateDomain("device"), 
				gcnew Appender::FileAppender(patternLayout, eventLogger->deviceLogPath, true));
			eventLogger->logDevice = LogManager::GetLogger("device", "Device Logging");
			
			eventLogger->removableLogPath = key->GetValue("Log_Dir") + "\\" + "removable.log";
			Config::BasicConfigurator::Configure(LogManager::CreateDomain("removable"), 
				gcnew Appender::FileAppender(patternLayout, eventLogger->removableLogPath, true));
			eventLogger->logRemovable = LogManager::GetLogger("removable", "Removable Media Logging");

			eventLogger->errorLogPath = key->GetValue("Log_Dir") + "\\" + "error.log";
			Config::BasicConfigurator::Configure(LogManager::CreateDomain("error"), 
				gcnew Appender::FileAppender(patternLayout, eventLogger->errorLogPath, true));
			eventLogger->logError = LogManager::GetLogger("error", "Error Logging");
			
		}
		return eventLogger;
	}
	
	void MyDLPEventLogger::LogSensFile(String ^log)
	{
		try
		{
			eventLogger->logSensFile->Info(log);
		} catch (System::Exception ^ex) {

		}
	}

	void MyDLPEventLogger::LogDevice(String ^log)
	{		
		try
		{
			eventLogger->logDevice->Info(log);
		} catch (System::Exception ^ex) {
		}
	}

	void MyDLPEventLogger::LogRemovable(String ^log)
	{		
		try
		{
			eventLogger->logRemovable->Warn(log);
		} catch (System::Exception ^ex) {
		}
	}

	void MyDLPEventLogger::LogError(String ^log)
	{		
		try
		{
			eventLogger->logError->Warn(log);
		} catch (System::Exception ^ex) {
		}
	}

	void MyDLPEventLogger::LogLib(String ^log)
	{

	}
}