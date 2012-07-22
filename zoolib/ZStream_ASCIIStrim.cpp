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

#include "zoolib/ZStream_ASCIIStrim.h"
#include "zoolib/ZStrim.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_ASCIIStrim

ZStreamR_ASCIIStrim::ZStreamR_ASCIIStrim(const ZStrimR& iStrimR)
:	fStrimR(iStrimR)
	{}

void ZStreamR_ASCIIStrim::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
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
			fStrimR.Read(buffer, 6, &countCURead, iCount, &countCPRead);
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
			fStrimR.Read(localDest, iCount, &countRead);
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

	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<UTF8*>(oDest);
	}

// =================================================================================================
// MARK: - ZStreamW_ASCIIStrim

ZStreamW_ASCIIStrim::ZStreamW_ASCIIStrim(const ZStrimW& iStrimW)
:	fStrimW(iStrimW)
	{}

void ZStreamW_ASCIIStrim::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);
	while (iCount--)
		{
		UTF32 current = *localSource++;
		if (int32(current) >= 0 && current <= 127)
			fStrimW.WriteCP(current);
		}
	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const char*>(iSource);
	}

} // namespace ZooLib
