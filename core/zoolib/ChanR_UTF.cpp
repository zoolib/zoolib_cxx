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
//#include "zoolib/ChanR_UTF_More.h"
#include "zoolib/ZUnicode.h"

#include <stdexcept> // for range_error

namespace ZooLib {

// =================================================================================================
// MARK: -

void ChanR_UTF::sThrow_Exhausted()
	{
	throw std::range_error("ChanR_UTF::sThrow_Exhausted");
	}

// =================================================================================================
// MARK: -

ZQ<UTF32> sQReadCP(const ChanR_UTF& iChanR)
	{
	UTF32 result;
	if (1 == sRead(&result, 1, iChanR))
		return result;
	return null;
	}

string32 sReadUTF32(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string32 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP);
		UTF32* dest = sNonConst(result.data()) + destGenerated;
		const size_t cuRead = sRead(dest, iCountCP, iChanR);
		const size_t cpRead = ZUnicode::sCUToCP(dest, cuRead);
		if (cpRead == 0)
			ChanR_UTF::sThrow_Exhausted();
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

string16 sReadUTF16(size_t iCountCP, const ChanR_UTF& iChanR)
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
			ChanR_UTF::sThrow_Exhausted();
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

string8 sReadUTF8(size_t iCountCP, const ChanR_UTF& iChanR)
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
			ChanR_UTF::sThrow_Exhausted();
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

} // namespace ZooLib
