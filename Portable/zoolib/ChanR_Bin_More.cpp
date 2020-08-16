// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanR_Bin_More.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZQ<std::string> sQReadString(const ChanR_Bin& iChanR, size_t iCount)
	{
	std::string theString(iCount, 0);
	if (iCount and iCount != sReadMemFully(iChanR, const_cast<char*>(theString.data()), iCount))
		return null;
	return theString;
	}

std::string sReadString(const ChanR_Bin& iChanR, size_t iCount)
	{
	if (const ZQ<std::string> theQ = sQReadString(iChanR, iCount))
		return *theQ;
	sThrow_ExhaustedR();
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
	if (const ZQ<uint64> theQ = sQReadCount(r))
		return *theQ;
	sThrow_ExhaustedR();
	}

std::string sReadCountPrefixedString(const ChanR_Bin& iChanR)
	{ return sReadString(iChanR, size_t(sReadCount(iChanR))); }

} // namespace ZooLib
