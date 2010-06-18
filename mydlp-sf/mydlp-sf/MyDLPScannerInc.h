#ifndef __MYDLPSCANNERINC_H__
#define __MYDLPSCANNERINC_H__
//
//  Name of port used to communicate
//

const PWSTR ScannerPortName = L"\\MyDLPScannerPort";

#define SCANNER_READ_BUFFER_SIZE  2048
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