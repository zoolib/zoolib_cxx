/* -------------------------------------------------------------------------------------------------
Copyright (c) 2015 Andrew Green
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

#include "zoolib/Util_Any_JSONB.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSONB.h"

#include "zoolib/Log.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_Any_JSON.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_Any_JSONB {

ZQ<Val_Any> sQRead(const ZRef<ChannerR_Bin>& iChannerR)
	{
	const ZTime start = ZTime::sSystem();
	ZQ<Val_Any> theQ = Yad_Any::sQFromYadR(Yad_JSONB::sYadR(iChannerR));
	if (not theQ)
		return null;

	if (ZLOGF(w, eDebug+1))
		{
		w << "Received in " << sStringf("%.3gms: ", (ZTime::sSystem() - start) * 1e3);
		Util_Any_JSON::sWrite(*theQ, w);
		}
	return theQ;
	}

void sWrite(const Val_Any& iVal, const ChanW_Bin& iChanW)
	{
	const ZTime start = ZTime::sSystem();
	Yad_JSONB::sToChan(sYadR(iVal), iChanW);
	if (ZLOGF(w, eDebug+1))
		{
		w << "Sent in " << sStringf("%.3gms: ", (ZTime::sSystem() - start) * 1e3);
		Util_Any_JSON::sWrite(iVal, w);
		}
	}

} // namespace Util_Any_JSONB
} // namespace ZooLib

