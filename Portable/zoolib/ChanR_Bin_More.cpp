// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/Chan_Bin_string.h"
#include "zoolib/Util_Chan.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZQ<std::string> sQReadString(const ChanR_Bin& iChanR, size_t iCount)
	{
	std::string theString(iCount, 0);
	if (iCount && iCount != sReadMemFully(iChanR, const_cast<char*>(theString.data()), iCount))
		return null;
	return theString;
	}

std::string sReadString(const ChanR_Bin& iChanR, size_t iCount)
	{
	if (const ZQ<std::string> theQ = sQReadString(iChanR, iCount))
		return *theQ;
	sThrow_ExhaustedR();
	}

std::string sReadAllString(const ChanR_Bin& iChanR)
	{
	std::string result;
	sECopyAll(iChanR, ChanW_Bin_string(&result));
	return result;
	}

bool sRead_String(const ChanR_Bin& iChanR, const std::string& iPattern)
	{
	if (ZQ<std::string> theQ = sQReadString(iChanR, iPattern.length()))
		return *theQ == iPattern;
	return false;
	}

bool sTryRead_String(const ChanRU_Bin& iChanRU, const std::string& iPattern)
	{
	const size_t requiredLength = iPattern.length();
	std::string readString(requiredLength, 0);
	const size_t countRead =
		sReadMemFully(iChanRU, const_cast<char*>(readString.data()), requiredLength);
	if (countRead == requiredLength && iPattern == readString)
		return true;
	sUnreadMem(iChanRU, readString.data(), countRead);
	return false;
	}

// -----

ZQ<uint64> sQReadCount(const ChanR_Bin& r)
	{
	ZQ<uint8> theQ = sQReadBE<uint8>(r);
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
