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

#ifndef __ZBlackBerry_Client_h__
#define __ZBlackBerry_Client_h__ 1
#include "zconfig.h"

#include "zoolib/blackberry/ZBlackBerry.h"

namespace ZooLib {
namespace ZBlackBerry {

// =================================================================================================
// MARK: - Manager_Client

class Manager_Client
:	public Manager
	{
private:
	Manager_Client();

public:
	Manager_Client(ZRef<ZStreamerRWConFactory> iFactory, bool iAutoReconnect);
	virtual ~Manager_Client();

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// From Manager
	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs);

	virtual ZRef<Device> Open(uint64 iDeviceID);

private:
	class Commer_Changed;
	friend class Commer_Changed;

	void pDetached(ZRef<Commer_Changed> iCommer);
	void pStartCommer(ZRef<ZStreamerRWCon> iSRWCon);

	ZRef<Commer_Changed> fCommer_Changed;

	ZRef<ZStreamerRWConFactory> fFactory;
	bool fAutoReconnect;
	};

} // namespace ZBlackBerry
} // namespace ZooLib

#endif // __ZBlackBerry_Client_h__
