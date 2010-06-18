#pragma once
#pragma managed 

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

using namespace System;

namespace mydlpsf {
	public ref class MyDLPRemoteDeviceConf
	{
	public:
		MyDLPRemoteDeviceConf(void);
		static Boolean checkUSB;
        static Boolean disableUSB;
        static Boolean logUSBDiscs;
        static Boolean logUSBFileMovements;
        static Boolean encryptUSBFileMovements;

        static Boolean checkLogical;
        static Boolean disableLogical;
        static Boolean disableCDDVDRW;
        static Boolean logLogicalDiscs;
        
        static Boolean checkNIC;
        static Boolean disableWirelessNIC;
        static Boolean disableBluetoothNIC;
        static Boolean disableWiredNIC;  
	};
}
