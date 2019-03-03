/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ChanR_Bin_HexStrim.h"

#include "zoolib/Util_Chan_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_HexStrim

ChanR_Bin_HexStrim::ChanR_Bin_HexStrim(const ChanRU_UTF& iChanRU)
:	fChanRU(iChanRU)
,	fAllowUnderscore(false)
	{}

ChanR_Bin_HexStrim::ChanR_Bin_HexStrim(bool iAllowUnderscore, const ChanRU_UTF& iChanRU)
:	fChanRU(iChanRU)
,	fAllowUnderscore(iAllowUnderscore)
	{}

ChanR_Bin_HexStrim::~ChanR_Bin_HexStrim()
	{}

size_t ChanR_Bin_HexStrim::Read(byte* oDest, size_t iCount)
	{
	using namespace Util_Chan;

	byte* localDest = oDest;

	while (iCount)
		{
		sSkip_WSAndCPlusPlusComments(fChanRU);

		ZQ<int> firstDigitQ = sQRead_HexDigit(fChanRU);
		if (not firstDigitQ)
			{
			if (not fAllowUnderscore || not sTryRead_CP('_', fChanRU))
				break;
			firstDigitQ = 0;
			}

		sSkip_WSAndCPlusPlusComments(fChanRU);

		ZQ<int> secondDigitQ = sQRead_HexDigit(fChanRU);
		if (not secondDigitQ)
			{
			if (not fAllowUnderscore || not sTryRead_CP('_', fChanRU))
				sThrow_ParseException("Could not read second nibble of byte");
			secondDigitQ = 0;
			}

		*localDest++ = *firstDigitQ * 16 + *secondDigitQ;
		--iCount;
		}

	return localDest - oDest;
	}

} // namespace ZooLib
