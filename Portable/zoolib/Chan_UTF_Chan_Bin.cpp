/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Compat_algorithm.h"
#include "zoolib/Unicode.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Static helpers

static size_t spReadUTF32_UTF8(UTF32* oDest, const size_t iCount,
	const ChanR_Bin& iChanR_Bin)
	{
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;

	byte curByte = 0;
	bool gotByte = false;
	while (localDest < localDestEnd)
		{
		if (not gotByte)
			{
			if (NotQ<byte> theQ = sQRead(iChanR_Bin))
				break;
			else
				curByte = *theQ;
			}

		gotByte = false;

		size_t sequenceLength = Unicode::sUTF8SequenceLength[curByte];
		if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else if (sequenceLength == 1)
			{
			*localDest++ = curByte;
			}
		else
			{
			UTF32 theCP = curByte & Unicode::sUTF8StartByteMask[sequenceLength];
			gotByte = false;
			while (--sequenceLength)
				{
				if (NotQ<byte> theQ = sQRead(iChanR_Bin))
					break;
				else
					curByte = *theQ;
				if ((curByte & 0xC0) != 0x80)
					{
					// It's not a legal continuation byte.
					gotByte = true;
					break;
					}
				theCP <<= 6;
				theCP += curByte & 0x3F;
				}

			if (not gotByte)
				*localDest++ = theCP;
			}
		}
	return localDest - oDest;
	}

// =================================================================================================
#pragma mark - ZStrimR_StreamUTF8

ChanR_UTF_Chan_Bin_UTF8::ChanR_UTF_Chan_Bin_UTF8(const ChanR_Bin& iChanR_Bin)
:	fChanR_Bin(iChanR_Bin)
	{}

size_t ChanR_UTF_Chan_Bin_UTF8::Read(UTF32* oDest, size_t iCount)
	{ return spReadUTF32_UTF8(oDest, iCount, fChanR_Bin); }

// =================================================================================================
#pragma mark - ChanW_UTF_Chan_Bin_UTF8

ChanW_UTF_Chan_Bin_UTF8::ChanW_UTF_Chan_Bin_UTF8(const ChanW_Bin& iChanW_Bin)
:	fChanW_Bin(iChanW_Bin)
	{}

size_t ChanW_UTF_Chan_Bin_UTF8::WriteUTF8(const UTF8* iSource, size_t iCountCU)
	{ return sWriteMem(fChanW_Bin, iSource, iCountCU); }

void ChanW_UTF_Chan_Bin_UTF8::Flush()
	{ sFlush(fChanW_Bin); }

} // namespace ZooLib
