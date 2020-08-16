// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanR_Bin_HexStrim.h"

#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_HexStrim

ChanR_Bin_HexStrim::ChanR_Bin_HexStrim(const ChanRU_UTF& iChanRU)
:	fChanRU(iChanRU)
,	fAllowUnderscore(false)
	{}

ChanR_Bin_HexStrim::ChanR_Bin_HexStrim(bool iAllowUnderscore, const ChanRU_UTF& iChanRU)
:	fChanRU(iChanRU)
,	fAllowUnderscore(iAllowUnderscore)
	{}

ChanR_Bin_HexStrim::~ChanR_Bin_HexStrim()
	{}

size_t ChanR_Bin_HexStrim::Read(byte* oDest, size_t iCount)
	{
	using namespace Util_Chan;

	byte* localDest = oDest;

	while (iCount)
		{
		sSkip_WSAndCPlusPlusComments(fChanRU);

		ZQ<int> firstDigitQ = sQRead_HexDigit(fChanRU);
		if (not firstDigitQ)
			{
			if (not fAllowUnderscore || not sTryRead_CP('_', fChanRU))
				break;
			firstDigitQ = 0;
			}

		sSkip_WSAndCPlusPlusComments(fChanRU);

		ZQ<int> secondDigitQ = sQRead_HexDigit(fChanRU);
		if (not secondDigitQ)
			{
			if (not fAllowUnderscore || not sTryRead_CP('_', fChanRU))
				sThrow_ParseException("Could not read second nibble of byte");
			secondDigitQ = 0;
			}

		*localDest++ = *firstDigitQ * 16 + *secondDigitQ;
		--iCount;
		}

	return localDest - oDest;
	}

} // namespace ZooLib
