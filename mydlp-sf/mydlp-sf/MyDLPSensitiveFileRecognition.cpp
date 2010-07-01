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
#include "MyDLPSensitiveFileRecognition.h"
#include <stdio.h>
#include <vcclr.h>
#include "MyDLPEventLogger.h"
#using <mscorlib.dll>

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace IFilterParser;

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

	int MyDLPSensitiveFileRecognition::Init()
	{
		int ret;
		if((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) {
			return 2;
		}
		if(!(this->engine = cl_engine_new())) {
			return 2;
		}
		return 0;
	}

	int MyDLPSensitiveFileRecognition::AddRegex(array<System::UInt32> ^ids, array<System::String ^> ^regex, int count)
	{
		int i = 0;
		unsigned char **regex_list = (unsigned char **)malloc(sizeof(unsigned char *) * count);
		unsigned int *id_list = (unsigned int *)malloc(sizeof(int) * count);
		char **str = (char **)malloc(sizeof(int) * count);
		UTF8Encoding ^encoder = gcnew UTF8Encoding();
		

		if(count == 0) 
		{
			if(cl_dlp_regex_init(NULL, NULL, count) != 0) {
				return 2;
			}
			return 0;
		}
		
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
						array<String ^> ^arr = BitConverter::ToString(encodedBytes)->ToLower()->Split( splitChar->ToCharArray() );
						for each (String ^byteRep in arr)
							utf8Regex += "\\x" + byteRep;
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
			
			char *tmpStr = (char *)malloc(sizeof(char) * (utf8Regex->Length + 1));
			strcpy_s(tmpStr, utf8Regex->Length + 1,ManagedToSTL(utf8Regex).c_str());
			
			str[i] = tmpStr;
			regex_list[i] = (unsigned char *)str[i];
			id_list[i] = ids[i];
		}

		if(cl_dlp_regex_init(id_list, (const unsigned char **)regex_list, count) != 0) {
			return 2;
		}
		return 0;
	}

	int MyDLPSensitiveFileRecognition::AddMD5s(String^ md5)
	{
		unsigned int sigs, ret;
		char *md5db_file;

		if(md5->Length == 0) {
			return 0;
		}

		if((md5db_file = cl_dlp_md5db_init((const unsigned char *)ManagedToSTL(md5).c_str())) == NULL) {
			cl_engine_free(engine);
			return -2;
		}

		if((ret = cl_load(md5db_file, this->engine, &sigs, CL_DB_STDOPT)) != CL_SUCCESS) {
			cl_engine_free(engine);
			cl_dlp_md5db_unlink();
			return -2;
		}

		cl_dlp_md5db_unlink();

		return sigs;
	}

	int MyDLPSensitiveFileRecognition::AddIBAN()
	{
		int ret = 0;

		if((ret = cl_dlp_iban_init()) != CL_SUCCESS) {
			cl_engine_free(engine);
			return 2;
		}

		return 0;
	}

	int MyDLPSensitiveFileRecognition::CompileClamEngine()
	{
		int ret = 0; 

		if((ret = cl_engine_compile(this->engine)) != 0) {
			cl_engine_free(engine);
			return 2;
		}

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableCC)
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_CC_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxCCCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_CC_COUNT, 0);

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableSSN)
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_SSN_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxSSNCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_SSN_COUNT, 0);

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableRegex)
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_REGEX_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxRegexCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_REGEX_COUNT, 0);

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableIBAN)
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_IBAN_COUNT, mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->maxIBANCount);
		else
			cl_engine_set_num(this->engine, CL_ENGINE_MIN_IBAN_COUNT, 0);				

		if(mydlpsf::MyDLPRemoteSensFileConf::GetInstance()->enableTRId)
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

	int MyDLPSensitiveFileRecognition::SearchSensitiveData(String ^filename)
	{
		const char *virname;
		unsigned long int size = 0; 
		int ret = 0;
		long     length = 0;
		TCHAR*   buffer = NULL;

		this->result = "Clean";
		
		try 
		{
			IntPtr ptr = Marshal::StringToHGlobalUni(filename);
			length = GetShortPathName((LPCWSTR)ptr.ToPointer(), NULL, 0);
			if (length == 0)
				return 2;

			buffer = new TCHAR[length];

			length = GetShortPathName((LPCWSTR)ptr.ToPointer(), buffer, length);
			if (length == 0) 
				return 2;

			Marshal::FreeHGlobal(ptr);

			if((ret = cl_scanfile((const char *)ManagedToSTL(filename).c_str(), &virname, &size, this->engine, scanOptions)) == CL_VIRUS) {
				this->result = STLToManaged(virname);
				return 1;
			} 
			
			if (ret == CL_RESCAN_OLE2 || ret == CL_RESCAN_PDF || ret==CL_RESCAN_PS) {

				String ^tempFilePath = System::IO::Path::GetTempFileName();
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
				} else {
					tempFilePath = String::Empty;
					ret = CL_CLEAN;
				}
 
				if(tempFilePath != String::Empty) 
				{
					if((ret = cl_scanfile((const char *)ManagedToSTL(tempFilePath).c_str(), &virname, &size, this->engine, scanOptions)) == CL_VIRUS) {
						this->result = STLToManaged(virname);
						ret = 1;
					}
					if(File::Exists(tempFilePath))
					{
						File::Delete(tempFilePath);
						array<String ^> ^pdfBoxFiles = Directory::GetFiles(System::IO::Path::GetDirectoryName(Path::GetTempFileName()), "pdfbox*");
						for each (String ^file in pdfBoxFiles) {
							File::Delete(file);
						}
					}

					return ret;
				}
			}

			if(ret == CL_CLEAN) {
				return 0;
			} else if (ret == CL_EARG) {
				cl_engine_free(engine);
				return 2;
			} else {
				cl_engine_free(engine);
				return 3;
			}
		} catch (Exception ^ex) {
			MyDLPEventLogger::GetInstance()->LogError(length.ToString() + "--" + gcnew String(buffer) + System::Environment::NewLine + ex->StackTrace);
		}

		return ret;
	}

	String^ MyDLPSensitiveFileRecognition::GetLastResult()
	{
		return this->result;
	}

	int MyDLPSensitiveFileRecognition::Close()
	{
		/* unlink temp hdb file */
		cl_dlp_md5db_unlink();

		/* free memory */
		cl_engine_free(this->engine);
		return 0;
	}
}