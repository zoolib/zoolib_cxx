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

#ifndef __ZBlackBerry_Client__
#define __ZBlackBerry_Client__ 1
#include "zconfig.h"

#include "zoolib/ZBlackBerry.h"
#include "zoolib/ZCommer.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * Manager_Client

class Manager_Client
:	public Manager,
	private ZCommer
	{
private:
	Manager_Client();

public:
	Manager_Client(ZRef<ZStreamerRWConFactory> iFactory);
	virtual ~Manager_Client();

// From Manager
	virtual void Start();
	virtual void Stop();

	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs);

	virtual ZRef<Device> Open(uint64 iDeviceID);

// From ZStreamReader via ZCommer
	virtual void RunnerDetached(ZStreamReaderRunner* iRunner);

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);
	virtual void Detached();

private:
	class Handler_ManagerChanged;
	friend class Handler_ManagerChanged;

	ZRef<ZStreamerRWConFactory> fFactory;
	bool fSendNotificationRequest;
	bool fSendClose;
	};

} // namespace ZBlackBerry

NAMESPACE_ZOOLIB_END

#endif // __ZBlackBerry_Client__
