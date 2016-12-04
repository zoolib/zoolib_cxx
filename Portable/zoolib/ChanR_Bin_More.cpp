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

#include "zoolib/ChanR_Bin_More.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZQ<std::string> sQReadString(const ChanR_Bin& iChanR, size_t iCount)
	{
	std::string theString(iCount, 0);
	if (iCount and iCount != sQReadMemFully(iChanR, const_cast<char*>(theString.data()), iCount))
		return null;
	return theString;
	}

std::string sReadString(const ChanR_Bin& iChanR, size_t iCount)
	{
	const ZQ<std::string> theQ = sQReadString(iChanR, iCount);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

// -----

ZQ<uint64> sQReadCount(const ChanR_Bin& r)
	{
	ZQ<uint8> theQ = sQRead<uint8>(r);
	if (not theQ)
		return null;

	switch (*theQ)
		{
		case 255: return sQReadBE<uint64>(r);
		case 254: return sQReadBE<uint32>(r);
		case 253: return sQReadBE<uint16>(r);
		default: return *theQ;
		}
	}

uint64 sReadCount(const ChanR_Bin& r)
	{
	const ZQ<uint64> theQ = sQReadCount(r);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

} // namespace ZooLib
