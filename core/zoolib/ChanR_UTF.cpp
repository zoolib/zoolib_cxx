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

#include "zoolib/ChanR_UTF.h"
#include "zoolib/Unicode.h"

namespace ZooLib {

// =================================================================================================
// MARK: -

ZQ<string32> sQReadUTF32(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string32 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP);
		UTF32* dest = sNonConst(result.data()) + destGenerated;
		const size_t cuRead = sQRead(dest, iCountCP, iChanR);
		const size_t cpRead = Unicode::sCUToCP(dest, cuRead);

		if (cpRead == 0)
			return null;
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

ZQ<string16> sQReadUTF16(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string16 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP + 1);
		size_t cuRead, cpRead;
		sRead(sNonConst(result.data()) + destGenerated,
			iCountCP + 1, &cuRead, iCountCP, &cpRead, iChanR);

		if (cpRead == 0)
			return null;
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

ZQ<string8> sQReadUTF8(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string8 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP + 5);
		size_t cuRead, cpRead;
		sRead(sNonConst(result.data()) + destGenerated,
			iCountCP + 5, &cuRead, iCountCP, &cpRead, iChanR);

		if (cpRead == 0)
			return null;
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

// -----

string32 sReadMustUTF32(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	const ZQ<string32> theQ = sQReadUTF32(iCountCP, iChanR);
	if (not theQ)
		sThrow_Exhausted(iChanR);
	return *theQ;
	}

string16 sReadMustUTF16(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	const ZQ<string16> theQ = sQReadUTF16(iCountCP, iChanR);
	if (not theQ)
		sThrow_Exhausted(iChanR);
	return *theQ;
	}

string8 sReadMustUTF8(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	const ZQ<string8> theQ = sQReadUTF8(iCountCP, iChanR);
	if (not theQ)
		sThrow_Exhausted(iChanR);
	return *theQ;
	}

} // namespace ZooLib
