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

#include "zoolib/ZStreamW_HexStrim.h"

namespace ZooLib {

static const UTF8 spHexDigits[] = "0123456789ABCDEF";
static const UTF8 spHexDigitsWithUnderscore[] = "_123456789ABCDEF";

// =================================================================================================
// MARK: - ZStreamW_HexStrim_Real

ZStreamW_HexStrim_Real::ZStreamW_HexStrim_Real(bool iUseUnderscore, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink)
	{
	if (iUseUnderscore)
		fHexDigits = spHexDigitsWithUnderscore;
	else
		fHexDigits = spHexDigits;
	}

void ZStreamW_HexStrim_Real::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);

	while (iCount)
		{
		UTF8 theHex[2];
		theHex[0] = fHexDigits[((*localSource) >> 4) & 0x0F];
		theHex[1] = fHexDigits[(*localSource) & 0x0F];
		fStrimSink.Write(theHex, 2);
		--iCount;
		++localSource;
		}

	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ZStreamW_HexStrim_Real::Imp_Flush()
	{ fStrimSink.Flush(); }

// =================================================================================================
// MARK: - ZStreamW_HexStrim

static
ZStrimW_InsertSeparator::Spacings
spSpacings(const std::string& iByteSeparator, const std::string& iChunkSeparator, size_t iChunkSize)
	{
	ZStrimW_InsertSeparator::Spacings result;

	if (iByteSeparator.size())
		result[2] = iByteSeparator;

	if (iChunkSize && iChunkSeparator.size())
		result[iChunkSize * 2] = iChunkSeparator;
	
	return result;
	}

ZStreamW_HexStrim::ZStreamW_HexStrim(
	const std::string& iByteSeparator,
	const std::string& iChunkSeparator, size_t iChunkSize,
	const ZStrimW& iStrimSink)
:	fStrim(spSpacings(iByteSeparator, iChunkSeparator, iChunkSize), iStrimSink)
,	fStream(false, fStrim)
	{}

void ZStreamW_HexStrim::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ fStream.Write(iSource, iCount, oCountWritten); }

void ZStreamW_HexStrim::Imp_Flush()
	{ fStream.Flush(); }

} // namespace ZooLib
