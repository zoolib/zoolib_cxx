/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Util_Chan_UTF.h"


namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_ASCIIStrim

ChanR_Bin_ASCIIStrim::ChanR_Bin_ASCIIStrim(const ChanR_UTF& iChanR)
:	fChanR_UTF(iChanR)
	{}

size_t ChanR_Bin_ASCIIStrim::QRead(byte* oDest, size_t iCount)
	{
	UTF8* localDest = reinterpret_cast<UTF8*>(oDest);

	if (iCount < 6)
		{
		// When reading UTF8 we must have up to six bytes available.
		UTF8 buffer[6];
		while (iCount)
			{
			size_t countCURead;
			size_t countCPRead;
			sRead(buffer, 6, &countCURead, iCount, &countCPRead, fChanR_UTF);
			if (countCURead == 0)
				break;

			// Transcribe only ASCII code units/points.
			for (UTF8* readFrom = &buffer[0], *destEnd = &buffer[countCURead];
				readFrom < destEnd; ++readFrom)
				{
				if (*readFrom & 0x80)
					continue;
				*localDest++ = *readFrom;
				--iCount;
				}
			}
		}
	else
		{
		while (iCount)
			{
			// Top up our buffer with UTF8 code points.
			size_t countRead;
			sRead(localDest, iCount, &countRead, iCount, nullptr, fChanR_UTF);
			if (countRead == 0)
				break;

			// Scan till we find a non-ASCII code point (if any).
			for (UTF8* readFrom = localDest, *destEnd = localDest + countRead;
				readFrom < destEnd; ++readFrom)
				{
				if (*readFrom & 0x80)
					{
					// We found a problem, so start transcribing only those bytes that are okay.
					for (UTF8* writeTo = readFrom; readFrom < destEnd; ++readFrom)
						{
						if (not (*readFrom & 0x80))
							*writeTo++ = *readFrom;
						}
					break;
					}
				}
			localDest += countRead;
			iCount -= countRead;
			}
		}

	return localDest - reinterpret_cast<UTF8*>(oDest);
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_ASCIIStrim

ChanW_Bin_ASCIIStrim::ChanW_Bin_ASCIIStrim(const ChanW_UTF& iChanW)
:	fChanW_UTF(iChanW)
	{}

size_t ChanW_Bin_ASCIIStrim::QWrite(const byte* iSource, size_t iCount)
	{
	const byte* localSource = iSource;
	while (iCount--)
		{
		UTF32 current = *localSource++;
		if (int32(current) >= 0 && current <= 127)
			{
			if (not sQWrite(current, fChanW_UTF))
				break;
			}
		}
	return localSource - iSource;
	}

} // namespace ZooLib
