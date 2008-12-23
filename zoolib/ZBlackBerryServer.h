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

#ifndef __ZBlackBerry_Server__
#define __ZBlackBerry_Server__ 1
#include "zconfig.h"

#include "zoolib/ZBlackBerry.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerryServer

class ZBlackBerryServer
:	ZBlackBerry::Manager::Observer,
	ZBlackBerry::Device::Observer
	{
public:
	ZBlackBerryServer(ZRef<ZBlackBerry::Manager> iManager);
	~ZBlackBerryServer();

	void HandleRequest(ZRef<ZStreamerRWCon> iSRWCon);

private:
// From Manager::Observer
	virtual void ManagerChanged(ZRef<ZBlackBerry::Manager> iManager);

// From Device::Observer
	virtual void Finished(ZRef<ZBlackBerry::Device> iDevice);

private:
	ZMutex fMutex;
	ZCondition fCondition;
	ZRef<ZBlackBerry::Manager> fManager;

	ZRef<ZBlackBerry::Device> pGetDevice(uint64 iDeviceID);

	class Handler_ManagerChanged;
	friend class Handler_ManagerChanged;
	void pRemove_ManagerChanged(Handler_ManagerChanged*);
	std::vector<Handler_ManagerChanged*> fHandlers_ManagerChanged;

	class Handler_DeviceFinished;
	friend class Handler_DeviceFinished;
	void pRemove_DeviceFinished(Handler_DeviceFinished*);

	struct Entry_t
		{
		uint64 fID;
		bool fLive;
		ZRef<ZBlackBerry::Device> fDevice;
		std::vector<Handler_DeviceFinished*> fHandlers;
		};

	std::vector<Entry_t> fEntries;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZBlackBerry_Server__
