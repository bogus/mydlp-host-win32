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

#include "stdafx.h"
#include <windows.h>
#include <vcclr.h>
#include <stdio.h>
#include "MyDLPSensitiveFileRecognition.h"
#include "MyDLPEventLogger.h"
#include "MyDLPPSParse.h"
#include "MyDLPTempFileManager.h"
#include <pcre.h>
#using <mscorlib.dll>

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace IFilterParser;
using namespace System::Threading;
using namespace System::Runtime::CompilerServices;
using namespace System::Collections::Generic;

namespace mydlpsf {

	std::string MyDLPSensitiveFileRecognition::ManagedToSTL(String ^managed)
	{
		char *chars = (char *)(void *)Marshal::StringToHGlobalAnsi(managed);
		std::string stl = chars;
		Marshal::FreeHGlobal(IntPtr(chars));
		return stl;
	}

	String ^MyDLPSensitiveFileRecognition::STLToManaged(std::string stl)
	{
		return Marshal::PtrToStringAnsi(IntPtr((void *) stl.c_str()));
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	int MyDLPSensitiveFileRecognition::Init(int version)
	{
		int ret;
		if((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) {
			return 2;
		}
		if(!(this->engine = cl_engine_new())) {
			return 2;
		}
		this->version = version;
		return 0;
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	int MyDLPSensitiveFileRecognition::AddRegex(array<System::UInt32> ^ids, array<System::String ^> ^regex, int count)
	{
		int i = 0;
		const char *error;
		int erroffset = 0;
		array<pcre *> ^pcreArr = gcnew array<pcre *>(count);
		List<UInt32> ^idsArr = gcnew List<UInt32>();
		struct regex_st *regexes;

		UTF8Encoding ^encoder = gcnew UTF8Encoding();
		
		for(i = 0; i < count; i++)
		{		
			String ^utf8Regex = String::Empty;
			try
			{
				for(int j = 0; j < regex[i]->Length; j++)
				{	
					array<unsigned char> ^encodedBytes =  encoder->GetBytes(regex[i]->Substring(j,1));
					if(encodedBytes->Length > 1)
					{
						String ^splitChar = "-";
						encodedBytes =  encoder->GetBytes(regex[i]->Substring(j,1)->ToLower());
						utf8Regex += "(";
						array<String ^> ^arr = BitConverter::ToString(encodedBytes)->ToLower()->Split( splitChar->ToCharArray() );
						for each (String ^byteRep in arr)
							utf8Regex += "\\x" + byteRep;
						utf8Regex += "|";
						encodedBytes =  encoder->GetBytes(regex[i]->Substring(j,1)->ToUpper());
						arr = BitConverter::ToString(encodedBytes)->ToLower()->Split( splitChar->ToCharArray() );
						for each (String ^byteRep in arr)
							utf8Regex += "\\x" + byteRep;
						utf8Regex += ")";
					}
					else
					{
						utf8Regex += regex[i]->Substring(j,1);
					}					
				}
			} 
			catch (Exception ^ex)
			{
				MyDLPEventLogger::GetInstance()->LogError(ex->StackTrace);
			}
		
			pcre *temp_re = pcre_compile(ManagedToSTL(utf8Regex).c_str(), PCRE_CASELESS, &error, &erroffset, NULL);
			if(erroffset == 0) {
				pcreArr[i] = temp_re;
				idsArr->Add(ids[i]);
			}
		}
		
		if(count == 0) 
		{
			if(cl_dlp_regex_init(NULL, NULL, count) != 0) {
				return 2;
			}
			return 0;
		} 
		else
		{
			regexes = (struct regex_st *)malloc(sizeof(struct regex_st) * idsArr->Count);
		}
		
		for(i =  0; i < idsArr->Count; i++)
		{
			regexes[i].dlp_re = pcreArr[i];
			regexes[i].id = idsArr[i];
		}

		if(cl_dlp_regex_init2(regexes, idsArr->Count) != 0) {
			return 2;
		}
		
		return 0;
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	int MyDLPSensitiveFileRecognition::AddMD5s()
	{
		unsigned int sigs, ret;
		String^ md5 = mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->md5Val;

		if(md5->Length == 0) {
			return 0;
		}

		hdbFileName = MyDLPTempFileManager::GetInstance()->GetTempFileName(".hdb");
		StreamWriter ^writer = gcnew StreamWriter(gcnew FileStream(hdbFileName, 
			FileMode::CreateNew, FileAccess::Write));
		writer->Write(md5);
		writer->Flush();
		writer->Close();

		if((ret = cl_load(ManagedToSTL(hdbFileName).c_str(), this->engine, &sigs, CL_DB_STDOPT)) != CL_SUCCESS) {
			cl_engine_free(engine);
			cl_dlp_md5db_unlink();
			return -2;
		}

		MyDLPTempFileManager::GetInstance()->DeleteFile(hdbFileName);

		return sigs;
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	int MyDLPSensitiveFileRecognition::AddIBAN()
	{
		int ret = 0;

		if((ret = cl_dlp_iban_init()) != CL_SUCCESS) {
			cl_engine_free(engine);
			return 2;
		}

		return 0;
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	int MyDLPSensitiveFileRecognition::CompileClamEngine()
	{
		int ret = 0; 

		if((ret = cl_engine_compile(this->engine)) != 0) {
			cl_engine_free(engine);
			return 2;
		}

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableCC.Equals(TRUE))
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_CC_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxCCCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_CC_COUNT, 0);

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableSSN.Equals(TRUE))
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_SSN_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxSSNCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_SSN_COUNT, 0);

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableRegex.Equals(TRUE))
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_REGEX_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxRegexCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_REGEX_COUNT, 0);

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableIBAN.Equals(TRUE))
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_IBAN_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxIBANCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_IBAN_COUNT, 0);				

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableTRId.Equals(TRUE))
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_TRID_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxTRIdCount);		
		else 
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_TRID_COUNT, 0);

		scanOptions = 0;

		scanOptions = CL_SCAN_ALGORITHMIC | CL_SCAN_STRUCTURED;

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->blockEncrypted)
			scanOptions = scanOptions | CL_SCAN_BLOCKENCRYPTED;
/*
		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->blockBroken)
			scanOptions = scanOptions | CL_SCAN_BLOCKBROKEN;
*/

/*
		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->fileGroups->Count == 0) {
			scanOptions = scanOptions | CL_SCAN_ARCHIVE | CL_SCAN_MAIL | CL_SCAN_OLE2 | CL_SCAN_PDF | CL_SCAN_HTML | CL_SCAN_PE | CL_SCAN_PS;
		} else {
			for each (String ^option in mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->fileGroups) {
				if(clamFileGroupOptions->Contains(option))
					//scanOptions = scanOptions | UInt32::Parse(clamFileGroupOptions[option]->ToString());
					scanOptions = scanOptions;
			}
		}
*/
		scanOptions = scanOptions | CL_SCAN_ARCHIVE | CL_SCAN_MAIL | CL_SCAN_OLE2 | CL_SCAN_PDF | CL_SCAN_HTML | CL_SCAN_PS;
		
		return 0;
	}

	[MethodImpl(MethodImplOptions::Synchronized)]
	int MyDLPSensitiveFileRecognition::SearchSensitiveData(String ^filename)
	{
		const char *virname;
		unsigned long int size = 0; 
		int ret = 0;		

		this->result = "Clean";

		//filename = GetShortFileName(filename);

		try 
		{
			if((ret = cl_scanfile((const char *)ManagedToSTL(filename).c_str(), &virname, &size, this->engine, scanOptions)) == CL_VIRUS) {
				this->result = STLToManaged(virname);
				return 1;
			} 

			if (ret == CL_RESCAN_OLE2 || ret == CL_RESCAN_PDF || ret==CL_RESCAN_PS
				|| ret==CL_RESCAN_RTF) {

				String ^tempFilePath = MyDLPTempFileManager::GetInstance()->GetTempFileName();
				//String ^tempFilePath = GetShortFileName(tempFilePath);
				try
				{
					if(ret == CL_RESCAN_OLE2)
					{
						StreamWriter ^writer = gcnew StreamWriter(tempFilePath, false, System::Text::Encoding::UTF8);
						TextReader ^reader = gcnew FilterReader(filename);
						writer->Write(reader->ReadToEnd()); 
						reader->Close();
						writer->Close();
					} else if (ret == CL_RESCAN_PDF) {
						PDFParser ^parser = gcnew PDFParser(); 
						StreamWriter ^writer = gcnew StreamWriter(tempFilePath, false, System::Text::Encoding::UTF8);
						writer->WriteLine(parser->parseUsingPDFBox(filename)); 
						writer->Close();
					} else if (ret == CL_RESCAN_RTF) {
						(gcnew RTFParser())->ParseRTF(filename, tempFilePath);
					} else if (ret == CL_RESCAN_PS) {
						const char *inputFile = (const char *)(void *)Marshal::StringToHGlobalAnsi(GetShortFileName(filename));
						const char *outputFile = (const char *)(void *)Marshal::StringToHGlobalAnsi(GetShortFileName(tempFilePath));
						PSParse(inputFile, outputFile);
					} 
				}
				catch(Exception ^ex)
				{
					MyDLPEventLogger::GetInstance()->LogError("ParserException : " + filename + System::Environment::NewLine + ex->StackTrace);
					tempFilePath = String::Empty;
					ret = CL_CLEAN;
				}
 
				if(tempFilePath != String::Empty && File::Exists(tempFilePath)) 
				{
					if((ret = cl_scanfile((const char *)ManagedToSTL(tempFilePath).c_str(), &virname, &size, this->engine, scanOptions)) == CL_VIRUS) {
						this->result = STLToManaged(virname);
						ret = 1;
					}

					MyDLPTempFileManager::GetInstance()->DeleteFile(tempFilePath);

					return ret;
				}
			}

			if(ret == CL_CLEAN) {
				return 0;
			} else {
				cl_engine_free(engine);
				return 3;
			}
		} catch(IOException ^ex) {
			MyDLPEventLogger::GetInstance()->LogError("IOException : " + filename + System::Environment::NewLine + ex->StackTrace);
		} catch (Exception ^ex) {
			Close();
			MyDLPEventLogger::GetInstance()->LogError(filename + System::Environment::NewLine + ex->StackTrace);
			throw gcnew Exception("Sensitive File Search: " + filename, ex);
		}

		return ret;
	}

	String^ MyDLPSensitiveFileRecognition::GetLastResult()
	{
		return this->result;
	}

	int MyDLPSensitiveFileRecognition::Close()
	{

		/* free memory */
		cl_engine_free(this->engine);
		return 0;
	}

	String ^MyDLPSensitiveFileRecognition::GetShortFileName(String ^longName)
	{
		long     length = 0;
		TCHAR*   buffer = NULL;
		IntPtr ^ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(longName);
		
		length = GetShortPathName((LPCWSTR)ptr->ToPointer(), NULL, 0);
		if (length == 0) 
			return String::Empty;

		buffer = new TCHAR[length];

		length = GetShortPathName((LPCWSTR)ptr->ToPointer(), buffer, length);
		if (length == 0) 
			return String::Empty;
	    
		String ^str = gcnew String(buffer);
		
		return str;

	}
}