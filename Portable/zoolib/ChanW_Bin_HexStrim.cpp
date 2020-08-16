// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanW_Bin_HexStrim.h"

namespace ZooLib {

static const UTF8 spHexDigits[] = "0123456789ABCDEF";
static const UTF8 spHexDigitsLowercase[] = "0123456789abcdef";

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim_Real

ChanW_Bin_HexStrim_Real::ChanW_Bin_HexStrim_Real(bool iLowercaseHex, const ChanW_UTF& iChanW_UTF)
:	fChanW_UTF(iChanW_UTF)
	{
	if (iLowercaseHex)
		fHexDigits = spHexDigitsLowercase;
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
	bool iLowercaseHex,
	const ChanW_UTF& iChanW_UTF)
:	fChanW_UTF(spSpacings(iByteSeparator, iChunkSeparator, iChunkSize), iChanW_UTF)
,	fChanW_Bin(iLowercaseHex, fChanW_UTF)
	{}

size_t ChanW_Bin_HexStrim::Write(const byte* iSource, size_t iCount)
	{ return sWrite(fChanW_Bin, iSource, iCount); }

void ChanW_Bin_HexStrim::Flush()
	{ fChanW_Bin.Flush(); }

} // namespace ZooLib
