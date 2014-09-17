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

#include <vector>

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
	{
	if (not sQWrite(iString, iChanW))
		sThrow_Exhausted(iChanW);
	}

bool sQWrite(const std::string& iString, const ChanW_Bin& iChanW)
	{
	if (const size_t length = iString.size())
		{
		if (length != sWriteFully(iString.data(), length, iChanW))
			return false;
		}
	return true;
	}

void sWriteMust(const std::string& iString, const ChanW_Bin& iChanW)
	{
	if (not sQWrite(iString, iChanW))
		sThrow_Exhausted(iChanW);
	}

static
bool spQWritev(const ChanW_Bin& iChanW,
	const UTF8* iString, va_list iArgs)
	{
	std::vector<char> buffer(512, 0);
	for (;;)
		{
		#if ZCONFIG(Compiler, MSVC)
			va_list args = iArgs;
			int count = _vsnprintf(const_cast<char*>(&buffer[0]), buffer.size(), iString, args);
		#else
			va_list args;
			#ifdef __va_copy
				__va_copy(args, iArgs);
			#else
				va_copy(args, iArgs);
			#endif
			int count = vsnprintf(const_cast<char*>(&buffer[0]), buffer.size(), iString, args);
		#endif

		if (count < 0)
			{
			// Handle -ve result from glibc prior to version 2.1 by growing the string.
			buffer.resize(buffer.size() * 2);
			}
		else if (size_t(count) > buffer.size())
			{
			// Handle C99 standard, where count indicates how much space would have been needed.
			buffer.resize(count);
			}
		else if (count == 0)
			{
			// The string fitted and is empty.
			return true;
			}
		else
			{
			// The string fitted, we can now write it out.
			return size_t(count) == sWriteFully(&buffer[0], count, iChanW);
			}
		}
	}

bool sQWritef(const ChanW_Bin& iChanW, const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const bool result = spQWritev(iChanW, iString, args);
	va_end(args);
	return result;
	}

void sWriteMustf(const ChanW_Bin& iChanW, const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const bool result = spQWritev(iChanW, iString, args);
	va_end(args);
	if (not result)
		sThrow_Exhausted(iChanW);
	}

// =================================================================================================
// MARK: -

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const char* iString)
	{
	sQWrite(iString, iChanW) || sThrow_Exhausted(iChanW);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, char* iString)
	{
	sQWrite(iString, iChanW) || sThrow_Exhausted(iChanW);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const std::string& iString)
	{
	sQWrite(iString, iChanW) || sThrow_Exhausted(iChanW);
	return iChanW;
	}

// =================================================================================================
// MARK: - ChanW_Bin_string

ChanW_Bin_string::ChanW_Bin_string(std::string* ioString)
:	fStringPtr(ioString)
	{}

size_t ChanW_Bin_string::QWrite(const byte* iSource, size_t iCountCU)
	{
	fStringPtr->append((char*)iSource, iCountCU);
	return iCountCU;
	}

} // namespace ZooLib
