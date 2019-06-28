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

#include "zoolib/Chan_XX_Buffered.h"
#include "zoolib/Net_Internet.h"

//###include "zoolib/ZStreamerRWCon_SSL.h"

namespace ZooLib {
namespace HTTP {

// =================================================================================================
#pragma mark - HTTP::sQConnect

ZP<ChannerRWClose_Bin> sConnect(const std::string& iHost, uint16 iPort, bool iUseSSL)
	{
	if (ZP<ChannerRWClose_Bin> theSR = sCall(sZP(new NetName_Internet(iHost, iPort))))
		{
//##		theComboQ->SetW(new ChanW_XX_Buffered<byte>(theComboQ->GetW()))

		if (not iUseSSL)
			return theSR;

//##		if (ZP<ZStreamerRWCon> wrapped = sStreamerRWCon_SSL(theEP, theSW))
//##			return Connection_t(wrapped, wrapped, new ChannerClose_RWCon(wrapped));
		}
	return null;
	}

} // namespace HTTP
} // namespace ZooLib
