/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZBlackBerry_BBDevMgr_h__
#define __ZBlackBerry_BBDevMgr_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__BlackBerry_BBDevMgr
	#define ZCONFIG_API_Avail__BlackBerry_BBDevMgr ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Desired__BlackBerry_BBDevMgr
	#define ZCONFIG_API_Desired__BlackBerry_BBDevMgr 1
#endif

#include "zoolib/blackberry/ZBlackBerry.h"

#if ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)

#include "zoolib/blackberry/ZBlackBerryCOM.h"

namespace ZooLib {
namespace ZBlackBerry {

class Device_BBDevMgr;

// =================================================================================================
// MARK: - ZBlackBerry::Manager_BBDevMgr

class Manager_BBDevMgr
:	private ZBlackBerryCOM::IDeviceManagerEvents,
	public Manager
	{
	typedef ZBlackBerryCOM::IDevice IDevice;
	typedef ZBlackBerryCOM::IDeviceManager IDeviceManager;
	typedef ZBlackBerryCOM::IDeviceManagerEvents IDeviceManagerEvents;

public:
	Manager_BBDevMgr();

	virtual ~Manager_BBDevMgr();

// From ZCounted via Manager
	virtual void Initialize();
	virtual void Finalize();

// From IUnknown via IDeviceManagerEvents
	virtual STDMETHODIMP QueryInterface(const IID& inInterfaceID, void** outObjectRef);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

// From IDeviceManagerEvents
	virtual STDMETHODIMP DeviceConnect(IDevice* iDevice);
	virtual STDMETHODIMP DeviceDisconnect(IDevice* iDevice);

// From Manager
	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs);
	virtual ZRef<Device> Open(uint64 iDeviceID);

private:
	ZMtxR fMutex;
	ZCnd fCondition;

	ZRef<IDeviceManager> fDeviceManager;
	uint32 fCookie;

	struct Entry_t
		{
		ZRef<Device_BBDevMgr> fDevice;
		uint64 fID;
		};

	std::vector<Entry_t> fEntries;
	uint64 fNextID;
	};

} // namespace ZBlackBerry
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)

#endif // __ZBlackBerry_BBDevMgr_h__
