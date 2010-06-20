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

#ifndef __MYDLPSCANNERINC_H__
#define __MYDLPSCANNERINC_H__
//
//  Name of port used to communicate
//

const PWSTR ScannerPortName = L"\\MyDLPScannerPort";

#define SCANNER_READ_BUFFER_SIZE  32768
#define SCANNER_FILENAME_BUFFER_SIZE   1024

typedef struct _SCANNER_NOTIFICATION {
    ULONG BytesToScan;
    ULONG Reserved;             // for quad-word alignement of the Contents structure
    UCHAR Contents[SCANNER_READ_BUFFER_SIZE];
	USHORT Phase;
	WCHAR FileName[SCANNER_FILENAME_BUFFER_SIZE];
	ULONG FileNameLength;
} SCANNER_NOTIFICATION, *PSCANNER_NOTIFICATION;

typedef struct _SCANNER_REPLY {
    BOOLEAN SafeToOpen;    
} SCANNER_REPLY, *PSCANNER_REPLY;

typedef struct _SCANNER_REPLY_MESSAGE {
	FILTER_REPLY_HEADER ReplyHeader;
	SCANNER_REPLY Reply;
} SCANNER_REPLY_MESSAGE, *PSCANNER_REPLY_MESSAGE;


#endif __MYDLPSCANNERINC_H__