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

#include "zoolib/ChanW_Bin_HexStrim.h"

namespace ZooLib {

static const UTF8 spHexDigits[] = "0123456789ABCDEF";
static const UTF8 spHexDigitsWithUnderscore[] = "_123456789ABCDEF";

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim_Real

ChanW_Bin_HexStrim_Real::ChanW_Bin_HexStrim_Real(bool iUseUnderscore, const ChanW_UTF& iChanW_UTF)
:	fChanW_UTF(iChanW_UTF)
	{
	if (iUseUnderscore)
		fHexDigits = spHexDigitsWithUnderscore;
	else
		fHexDigits = spHexDigits;
	}

size_t ChanW_Bin_HexStrim_Real::Write(const byte* iSource, size_t iCount)
	{
	const byte* localSource = iSource;

	while (iCount)
		{
		UTF32 theHex[2];
		theHex[0] = fHexDigits[((*localSource) >> 4) & 0x0F];
		theHex[1] = fHexDigits[(*localSource) & 0x0F];
		sEWrite(fChanW_UTF, theHex, 2);
		--iCount;
		++localSource;
		}

	return localSource - iSource;
	}

void ChanW_Bin_HexStrim_Real::Flush()
	{ sFlush(fChanW_UTF); }

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim

static
ChanW_UTF_InsertSeparator::Spacings
spSpacings(const std::string& iByteSeparator, const std::string& iChunkSeparator, size_t iChunkSize)
	{
	ChanW_UTF_InsertSeparator::Spacings result;

	if (iByteSeparator.size())
		result[2] = iByteSeparator;

	if (iChunkSize && iChunkSeparator.size())
		result[iChunkSize * 2] = iChunkSeparator;
	
	return result;
	}

ChanW_Bin_HexStrim::ChanW_Bin_HexStrim(
	const std::string& iByteSeparator,
	const std::string& iChunkSeparator, size_t iChunkSize,
	const ChanW_UTF& iChanW_UTF)
:	fChanW_UTF(spSpacings(iByteSeparator, iChunkSeparator, iChunkSize), iChanW_UTF)
,	fChanW_Bin(false, fChanW_UTF)
	{}

size_t ChanW_Bin_HexStrim::Write(const byte* iSource, size_t iCount)
	{ return sWrite(fChanW_Bin, iSource, iCount); }

void ChanW_Bin_HexStrim::Flush()
	{ fChanW_Bin.Flush(); }

} // namespace ZooLib
