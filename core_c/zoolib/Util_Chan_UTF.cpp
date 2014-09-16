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

#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/ChanW_XX_More.h" // For ChanW_XX_Discard
#include "zoolib/ChanW_UTF_string.h" // For ChanW_UTF_string8
#include "zoolib/ZUnicode.h" // For ZUnicode::sIsEOL

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
// MARK: -

void sCopy_Line(const ChanR_UTF& iSource, const ChanW_UTF32& oDest)
	{
	for (;;)
		{
		if (ZQ<UTF32,false> theCPQ = sQRead<UTF32>(iSource))
			break;
		else if (ZUnicode::sIsEOL(*theCPQ))
			break;
		else
			sQWriteCP(*theCPQ, oDest);
		}
	}

void sSkip_Line(const ChanR_UTF& iSource)
	{ sCopy_Line(iSource, ChanW_XX_Discard<UTF32>()); }

string8 sRead_Line(const ChanR_UTF& iSource)
	{
	string8 result;
	sCopy_Line(iSource, ChanW_UTF_string8(&result));
	return result;
	}

} // namespace Util_Chan
} // namespace ZooLib
