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

#ifndef __ZBlackBerry_Streamer_h__
#define __ZBlackBerry_Streamer_h__ 1
#include "zconfig.h"

//#include "zoolib/ZCommer.h"
#include "zoolib/blackberry/ZBlackBerry.h"

namespace ZooLib {
namespace ZBlackBerry {

class Commer_Streamer;

// =================================================================================================
// MARK: - ZBlackBerry::Device_Streamer

class Device_Streamer
:	public Device
	{
public:
	Device_Streamer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~Device_Streamer();

// From ZBlackBerry::Device
	virtual void Initialize();
	virtual void Finalize();

	virtual ZRef<Channel> Open(bool iPreserveBoundaries,
		const std::string& iName, const PasswordHash* iPasswordHash, Error* oError);

	virtual ZQ<Data> GetAttribute(uint16 iObject, uint16 iAttribute);

	virtual uint32 GetPIN();

private:
	void pCommerFinished(ZRef<ZCommer> iCommer);

	ZRef<Commer_Streamer> fCommer;
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	};

} // namespace ZBlackBerry
} // namespace ZooLib

#endif // __ZBlackBerry_Streamer_h__
