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

#include "zoolib/ChanW_UTF_string.h" // For ChanW_UTF_string8
#include "zoolib/Unicode.h" // For Unicode::sIsEOL
#include "zoolib/Util_Chan_UTF.h"

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
// MARK: -

void sCopy_Line(const ChanR_UTF& iSource, const ChanW_UTF& oDest)
	{
	for (;;)
		{
		if (ZQ<UTF32,false> theCPQ = sQRead(iSource))
			break;
		else if (Unicode::sIsEOL(*theCPQ))
			break;
		else
			sWriteMust(*theCPQ, oDest);
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

// -----------------

string8 sRead_Until(const ChanR_UTF& iSource, UTF32 iTerminator)
	{
	string8 result;
	sCopy_Until<UTF32>(iSource, ChanW_UTF_string8(&result), iTerminator);
	return result;
	}

// -----------------

void sWriteExact(float iVal, const ChanW_UTF& iChanW)
	{
	// 9 decimal digits are necessary and sufficient for single precision IEEE 754.
	// "What Every Computer Scientist Should Know About Floating Point", Goldberg, 1991.
	// <http://docs.sun.com/source/806-3568/ncg_goldberg.html>
	sWriteMustf(iChanW, "%.9g", iVal);
	}

void sWriteExact(double iVal, const ChanW_UTF& iChanW)
	{
	// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
	sWriteMustf(iChanW, "%.17g", iVal);
	}

void sWriteExact(long double iVal, const ChanW_UTF& iChanW)
	{
	// This is a guess for now.
	sWriteMustf(iChanW, "%.34Lg", iVal);
	}

} // namespace Util_Chan
} // namespace ZooLib
