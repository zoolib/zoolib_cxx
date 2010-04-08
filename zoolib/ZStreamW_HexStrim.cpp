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
#include "zoolib/ZStrim.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_HexStrim

static const UTF8 spHexDigits[] = "0123456789ABCDEF";
static const UTF8 spHexDigitsWithUnderscore[] = "_123456789ABCDEF";

/**
\param iByteSeparator A UTF-8 string to be inserted between each byte that is output.
\param iChunkSeparator A UTF-8 string to be inserted between each chunk that is output.
\param iChunkSize The number of bytes to treat as a chunk.
\param iStrimSink The strim to which characters should be written.
*/
ZStreamW_HexStrim::ZStreamW_HexStrim(const std::string& iByteSeparator,
	const std::string& iChunkSeparator, size_t iChunkSize, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fByteSeparator(iByteSeparator),
	fChunkSeparator(iChunkSeparator),
	fChunkSize(iChunkSize),
	fCurrentChunkLength(0),
	fHexDigits(spHexDigits),
	fWrittenAny(false)
	{}

/**
\param iByteSeparator A UTF-8 string to be inserted between each byte that is output.
\param iChunkSeparator A UTF-8 string to be inserted between each chunk that is output.
\param iChunkSize The number of bytes to treat as a chunk.
\param iUseUnderscore If true then an underscore should be output in place of zero, if
false then a zero is output. This is useful when what's being output is the hex representation
of a 4 bit per pixel grayscale pixmap, in which case underscores are output for black pixels,
and visually drop out somewhat.
\param iStrimSink The strim to which characters should be written.
*/
ZStreamW_HexStrim::ZStreamW_HexStrim(const std::string& iByteSeparator,
	const std::string& iChunkSeparator, size_t iChunkSize,
	bool iUseUnderscore, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fByteSeparator(iByteSeparator),
	fChunkSeparator(iChunkSeparator),
	fChunkSize(iChunkSize),
	fCurrentChunkLength(0),
	fWrittenAny(false)
	{
	if (iUseUnderscore)
		fHexDigits = spHexDigitsWithUnderscore;
	else
		fHexDigits = spHexDigits;
	}

void ZStreamW_HexStrim::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);

	while (iCount)
		{
		if (fWrittenAny)
			{
			if (fCurrentChunkLength)
				fStrimSink.Write(fByteSeparator);
			else if (fChunkSize)
				fStrimSink.Write(fChunkSeparator);
			}
		fWrittenAny = true;

		UTF8 sHex[2];
		sHex[0] = fHexDigits[((*localSource) >> 4) & 0x0F];
		sHex[1] = fHexDigits[(*localSource) & 0x0F];
		fStrimSink.Write(sHex, 2);

		fCurrentChunkLength += 1;
		if (fCurrentChunkLength == fChunkSize)
			fCurrentChunkLength = 0;
		iCount -= 1;
		localSource += 1;
		if (oCountWritten)
			*oCountWritten += 1;
		}
	}

void ZStreamW_HexStrim::Imp_Flush()
	{
	fStrimSink.Flush();
	}

NAMESPACE_ZOOLIB_END
