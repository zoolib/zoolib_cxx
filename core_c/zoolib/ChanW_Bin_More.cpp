/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Stringf.h"

#include <string.h> // For strlen

namespace ZooLib {

// =================================================================================================
// MARK: -

bool sQWrite(const char* iString, const ChanW_Bin& iChanW)
	{
	if (iString)
		{
		if (const size_t length = strlen(iString))
			{
			if (length != sQWrite(iString, length, iChanW))
				return false;
			}
		}
	return true;
	}

void sWriteMust(const char* iString, const ChanW_Bin& iChanW)
	{ sQWrite(iString, iChanW) || sThrow_ExhaustedW(); }

bool sQWrite(const std::string& iString, const ChanW_Bin& iChanW)
	{
	if (const size_t length = iString.size())
		{
		if (length != sQWriteFully(iString.data(), length, iChanW))
			return false;
		}
	return true;
	}

void sWriteMust(const std::string& iString, const ChanW_Bin& iChanW)
	{ sQWrite(iString, iChanW) || sThrow_ExhaustedW(); }

static
bool spQWritev(const ChanW_Bin& iChanW, const UTF8* iString, va_list iArgs)
	{
	const std::string theString = sStringv(iString, iArgs);
	if (theString.size())
		return theString.size() == sQWriteFully(theString.data(), theString.size(), iChanW);
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

void sWritefMust(const ChanW_Bin& iChanW, const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const bool result = spQWritev(iChanW, iString, args);
	va_end(args);
	if (not result)
		sThrow_ExhaustedW();
	}

// =================================================================================================
// MARK: -

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const char* iString)
	{
	sWriteMust(iString, iChanW);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, char* iString)
	{
	sWriteMust(iString, iChanW);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const std::string& iString)
	{
	sWriteMust(iString, iChanW);
	return iChanW;
	}

// =================================================================================================
// MARK: -

bool sQWriteCount(uint64 iValue, const ChanW_Bin& w)
	{
	if (iValue < 253)
		return sQWrite<uint8>(iValue, w);

	if (iValue <= 0xFFFFU)
		return sQWrite<uint8>(253, w) && sQWriteBE<uint16>(iValue, w);

	if (iValue <= 0xFFFFFFFFU)
		return sQWrite<uint8>(254, w) && sQWriteBE<uint32>(iValue, w);

	return sQWrite<uint8>(255, w) && sQWriteBE<uint64>(iValue, w);
	}

void sWriteCountMust(uint64 iValue, const ChanW_Bin& w)
	{ sQWriteCount(iValue, w) || sThrow_ExhaustedW(); }

} // namespace ZooLib
