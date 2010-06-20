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
#pragma once
#pragma managed 

#ifndef __MYDLP_REM_DEV_CONF__
#define __MYDLP_REM_DEV_CONF__

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

#endif