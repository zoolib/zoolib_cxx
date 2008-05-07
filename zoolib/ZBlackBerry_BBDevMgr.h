/* ------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZBlackBerry_BBDevMgr__
#define __ZBlackBerry_BBDevMgr__ 1
#include "zconfig.h"

#include "ZBlackBerry.h"

#if ZCONFIG(OS, Win32)

#include "ZBlackBerryCOM.h"

namespace ZBlackBerry {

class Device_BBDevMgr;

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager_BBDevMgr

class Manager_BBDevMgr
:	public Manager,
	ZBlackBerryCOM::IDeviceManagerEvents
	{
	typedef ZBlackBerryCOM::IDevice IDevice;
	typedef ZBlackBerryCOM::IDeviceManager IDeviceManager;
	typedef ZBlackBerryCOM::IDeviceManagerEvents IDeviceManagerEvents;

public:
	Manager_BBDevMgr();

	virtual ~Manager_BBDevMgr();

// From Manager
	virtual void GetDeviceIDs(vector<uint64>& oDeviceIDs);
	virtual ZRef<Device> Open(uint64 iDeviceID);

// From IUnknown via IDeviceManagerEvents
	virtual STDMETHODIMP QueryInterface(const IID& inInterfaceID, void** outObjectRef);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

// From IDeviceManagerEvents
	virtual STDMETHODIMP DeviceConnect(IDevice* iDevice);
	virtual STDMETHODIMP DeviceDisconnect(IDevice* iDevice);

private:
	ZMutex fMutex;
	ZCondition fCondition;

	IDeviceManager* fDeviceManager;
	uint32 fCookie;

	struct Entry_t
		{
		ZRef<Device_BBDevMgr> fDevice;
		uint64 fID;
		};

	vector<Entry_t> fEntries;
	uint64 fNextID;
	};

} // namespace ZBlackBerry

#endif // ZCONFIG(OS, Win32)

#endif // __ZBlackBerry_BBDevMgr__
