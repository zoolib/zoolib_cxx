/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZStrimU_Std.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZUtil_Any_JSON.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_JSON.h"

using std::string;

// =================================================================================================
// MARK: -

namespace ZooLib {
namespace ZUtil_Any_JSON {

ZQ<ZVal_Any> sQRead(const ZRef<ZStrimmerU>& iSU)
	{
	if (iSU)
		{
		ZYad_JSON::ReadOptions theRO;
		theRO.fAllowUnquotedPropertyNames = true;
		theRO.fAllowEquals = true;
		theRO.fAllowSemiColons = true;
		theRO.fAllowTerminators = true;
		theRO.fLooseSeparators = true;
		return sFromYadR(ZVal_Any(), sYadR(iSU, theRO)).QGet<ZMap_Any>();
		}
	return null;
	}

ZQ<ZVal_Any> sQRead(const ZRef<ZStrimmerR>& iSR)
	{
	if (iSR)
		return sQRead(ZRef<ZStrimmerU>(new ZStrimmerU_FT<ZStrimU_Unreader>(iSR)));
	return null;
	}

ZQ<ZVal_Any> sQRead(const ZRef<ZStreamerR>& iSR)
	{
	if (iSR)
		return sQRead(new ZStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(iSR));
	return null;
	}

void sWrite(const ZVal_Any& iVal, const ZStrimW& iStrimW)
	{ ZYad_JSON::sToStrim(sYadR(iVal), iStrimW); }

} // namespace ZUtil_Any_JSON
} // namespace ZooLib
