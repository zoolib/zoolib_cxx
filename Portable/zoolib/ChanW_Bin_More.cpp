// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanW_Bin_More.h"

#include "zoolib/Stringf.h"

#include <string.h> // For strlen

namespace ZooLib {

// =================================================================================================
#pragma mark -

bool sQWrite(const ChanW_Bin& iChanW, const char* iString)
	{
	if (iString)
		{
		if (const size_t length = strlen(iString))
			{
			if (length != sWriteMem(iChanW, iString, length))
				return false;
			}
		}
	return true;
	}

void sEWrite(const ChanW_Bin& iChanW, const char* iString)
	{ sQWrite(iChanW, iString) || sThrow_ExhaustedW(); }

bool sQWrite(const ChanW_Bin& iChanW, const std::string& iString)
	{
	if (const size_t length = iString.size())
		{
		if (length != sWriteMemFully(iChanW, iString.data(), length))
			return false;
		}
	return true;
	}

void sEWrite(const ChanW_Bin& iChanW, const std::string& iString)
	{ sQWrite(iChanW, iString) || sThrow_ExhaustedW(); }

static
bool spQWritev(const ChanW_Bin& iChanW, const UTF8* iString, va_list iArgs)
	{
	const std::string theString = sStringv(iString, iArgs);
	if (theString.size())
		return theString.size() == sWriteMemFully(iChanW, theString.data(), theString.size());
	return true;
	}

bool sQWritef(const ChanW_Bin& iChanW, const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const bool result = spQWritev(iChanW, iString, args);
	va_end(args);
	return result;
	}

void sEWritef(const ChanW_Bin& iChanW, const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const bool result = spQWritev(iChanW, iString, args);
	va_end(args);
	if (not result)
		sThrow_ExhaustedW();
	}

// =================================================================================================
#pragma mark -

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const char* iString)
	{
	sEWrite(iChanW, iString);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, char* iString)
	{
	sEWrite(iChanW, iString);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const std::string& iString)
	{
	sEWrite(iChanW, iString);
	return iChanW;
	}

// =================================================================================================
#pragma mark -

bool sQWriteCount(const ChanW_Bin& w, uint64 iValue)
	{
	if (iValue < 253)
		return sQWriteBE<uint8>(w, iValue);

	if (iValue <= 0xFFFFU)
		return sQWriteBE<uint8>(w, 253) && sQWriteBE<uint16>(w, uint16(iValue));

	if (iValue <= 0xFFFFFFFFU)
		return sQWriteBE<uint8>(w, 254) && sQWriteBE<uint32>(w, uint32(iValue));

	return sQWriteBE<uint8>(w, 255) && sQWriteBE<uint64>(w, iValue);
	}

void sEWriteCount(const ChanW_Bin& w, uint64 iValue)
	{ sQWriteCount(w, iValue) || sThrow_ExhaustedW(); }

void sEWriteCountPrefixedString(const ChanW_Bin& w, const std::string& iString)
	{
	const size_t theLength = iString.length();
	sEWriteCount(w, theLength);
	if (theLength)
		sEWriteMem(w, iString.data(), theLength);
	}

} // namespace ZooLib
