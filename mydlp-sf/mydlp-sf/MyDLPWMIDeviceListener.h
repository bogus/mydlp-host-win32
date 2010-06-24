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

#ifndef __MYDLP_WMIDEVICELISTENER_H_
#define __MYDLP_WMIDEVICELISTENER_H_

#pragma once
#pragma managed

#include "MyDLPRemoteConf.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Management;

namespace mydlpsf
{
	public ref class MyDLPWMIDeviceListener
	{
	private:
		static MyDLPWMIDeviceListener ^deviceListener;
		ManagementEventWatcher ^w;
		static void USBAdded(Object ^sender, EventArrivedEventArgs ^e);
		static void USBRemoved(Object ^sender, EventArrivedEventArgs ^e);
		static void LogicalInserted(Object ^sender, EventArrivedEventArgs ^e);

	protected:
		MyDLPWMIDeviceListener(void);

	public:
		static MyDLPWMIDeviceListener ^GetInstance();
		
		void AddRemoveUSBHandler();
		void AddInsertUSBHandler();
		void AddInsertLogicalDeviceHandler();
	};
}

#endif