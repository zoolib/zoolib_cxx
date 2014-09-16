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

void sWrite(const char* iString, const ChanW_Bin& iChanW)
	{
	if (iString)
		{
		if (const size_t length = strlen(iString))
			{
			if (length != sWrite(iString, length, iChanW))
				ChanWBase::sThrow_Exhausted();
			}
		}
	}

void sWrite(const std::string& iString, const ChanW_Bin& iChanW)
	{
	if (const size_t length = iString.size())
		{
		if (length != sWriteFully(iString.data(), length, iChanW))
			ChanWBase::sThrow_Exhausted();
		}
	}

void sWritef(const ChanW_Bin& iChanW, const char* iString, ...)
	{
	std::vector<char> buffer(512, 0);
	for (;;)
		{
		va_list args;
		va_start(args, iString);

		const int count = vsnprintf(const_cast<char*>(&buffer[0]), buffer.size(), iString, args);

		va_end(args);

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
		else
			{
			// The string fitted, we can now write it out.
			if (count && size_t(count) != sWrite(buffer.data(), count, iChanW))
				ChanWBase::sThrow_Exhausted();
			return;
			}
		}
	}

// =================================================================================================
// MARK: -

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const char* iString)
	{
	sWrite(iString, iChanW);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, char* iString)
	{
	sWrite(iString, iChanW);
	return iChanW;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& iChanW, const std::string& iString)
	{
	sWrite(iString, iChanW);
	return iChanW;
	}

// =================================================================================================
// MARK: - ChanW_Bin_string

ChanW_Bin_string::ChanW_Bin_string(std::string* ioString)
:	fStringPtr(ioString)
	{}

size_t ChanW_Bin_string::Write(const byte* iSource, size_t iCountCU)
	{
	fStringPtr->append((char*)iSource, iCountCU);
	return iCountCU;
	}

} // namespace ZooLib
