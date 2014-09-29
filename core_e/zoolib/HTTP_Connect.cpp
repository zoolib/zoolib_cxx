/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/HTTP_Connect.h"

#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZStreamerRWCon_SSL.h"

namespace ZooLib {
namespace HTTP {

// =================================================================================================
// MARK: - HTTP::sQConnect

// This all constitutes a slightly skanky bridge between Chan and Stream.

class ChannerClose_RWCon
:	public ChannerClose
,	public ChanClose
	{
public:
	ChannerClose_RWCon(const ZRef<ZStreamerWCon>& iSWCon)
	:	fSWCon(iSWCon)
		{}

	virtual void GetChan(const ChanClose*& oChanPtr)
		{ oChanPtr = this; }

	virtual void Close()
		{ fSWCon->GetStreamWCon().SendDisconnect(); }

	const ZRef<ZStreamerWCon> fSWCon;
	};

ZQ<Connection_t> sAsConnectionQ(const ZRef<ZStreamerRWCon>& iSRWCon)
	{
	if (iSRWCon)
		return Connection_t(iSRWCon, iSRWCon, new ChannerClose_RWCon(iSRWCon));
	return null;
	}

ZQ<Connection_t> sQConnect(const std::string& iHost, uint16 iPort, bool iUseSSL)
	{
	if (ZRef<ZStreamerRWCon> theEP = ZNetName_Internet(iHost, iPort).Connect(10))
		{
		if (iUseSSL)
			theEP = sStreamerRWCon_SSL(theEP, theEP);

		return sAsConnectionQ(theEP);
		}
	return null;
	}

} // namespace HTTP
} // namespace ZooLib
