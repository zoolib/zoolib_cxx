/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/ZUtil_Strim_Data.h"

#include "zoolib/ZMemory.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZStream_Memory.h"

namespace ZooLib {

using std::min;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

void ZUtil_Strim_Data::sDumpData(const ZStreamRPos& iStreamRPos, const ZStrimW& s)
	{ sDumpData(iStreamRPos, s, ZUINT64_C(0xFFFFFFFFFFFFFFFF)); }

void ZUtil_Strim_Data::sDumpData(const ZStreamRPos& iStreamRPos, const ZStrimW& s, uint64 iMax)
	{ sDumpData(true, iStreamRPos, s, iMax); }

void ZUtil_Strim_Data::sDumpData(bool iShowSize, const ZStreamRPos& iStreamRPos, const ZStrimW& s, uint64 iMax)
	{
	const string byteSeparator = " ";
	const string chunkSeparator = "\n";
	const size_t chunkSize = 16;

	const uint64 theCount = iStreamRPos.GetSize() - iStreamRPos.GetPosition();
	uint64 countRemaining = min(theCount, iMax);

	if (iShowSize)
		{
		s.Writef("Size: %lld/%llX", theCount, theCount);
		if (theCount > countRemaining)
			s.Writef(", showing first %lld bytes", countRemaining);
		}

	while (countRemaining)
		{
		s.Write(chunkSeparator);

		uint64 lastPos = iStreamRPos.GetPosition();
		const uint64 countToCopy = min(countRemaining, uint64(chunkSize));
			
		uint64 countCopied;
		ZStreamW_HexStrim(byteSeparator, "", 0, s)
			.CopyFrom(iStreamRPos, countToCopy, &countCopied, nullptr);

		if (countCopied == 0)
			break;

		countRemaining -= countCopied;

		iStreamRPos.SetPosition(lastPos);
		if (size_t extraSpaces = chunkSize - countCopied)
			{
			// We didn't write a complete line of bytes, so pad it out.
			while (extraSpaces--)
				{
				// Two spaces for the two nibbles
				s.Write("  ");
				// And then the separator sequence
				s.Write(byteSeparator);
				}
			}

		s.Write(" // ");
		while (countCopied--)
			{
			char theChar = iStreamRPos.ReadInt8();
			if (theChar < 0x20 || theChar > 0x7E)
				s.WriteCP('.');
			else
				s.WriteCP(theChar);
			}
		}
	}

void ZUtil_Strim_Data::sDumpData(const ZStrimW& s, const void* iSource, uint64 iCount, uint64 iMax)
	{
	ZUtil_Strim_Data::sDumpData(s, true, iSource, iCount, iMax);
	}

void ZUtil_Strim_Data::sDumpData(const ZStrimW& s, const void* iSource, uint64 iCount)
	{
	ZUtil_Strim_Data::sDumpData(s, true, iSource, iCount);
	}

void ZUtil_Strim_Data::sDumpData(const ZStrimW& s, bool iShowSize, const void* iSource, uint64 iCount, uint64 iMax)
	{
	if (iCount)
		ZUtil_Strim_Data::sDumpData(iShowSize, ZStreamRPos_Memory(iSource, iCount), s, iMax);
	}

void ZUtil_Strim_Data::sDumpData(const ZStrimW& s, bool iShowSize, const void* iSource, uint64 iCount)
	{
	if (iCount)
		ZUtil_Strim_Data::sDumpData(iShowSize, ZStreamRPos_Memory(iSource, iCount), s, ZUINT64_C(0xFFFFFFFFFFFFFFFF));
	}

} // namespace ZooLib
