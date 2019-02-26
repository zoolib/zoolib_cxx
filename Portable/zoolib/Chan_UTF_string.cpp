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

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Unicode.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRU_UTF_string8

ChanRU_UTF_string8::ChanRU_UTF_string8(const string8& iString)
:	fString(iString)
,	fPosition(0)
	{}

ChanRU_UTF_string8::~ChanRU_UTF_string8()
	{}

size_t ChanRU_UTF_string8::Read(UTF32* oDest, size_t iCount)
	{
	const size_t theLength = fString.length();
	if (fPosition >= theLength)
		{
		return 0;
		}
	else
		{
		size_t countConsumed;
		size_t countProduced;
		Unicode::sUTF8ToUTF32(
			fString.data() + fPosition, theLength - fPosition,
			&countConsumed, nullptr,
			oDest, iCount,
			&countProduced);
		fPosition += countConsumed;
		return countProduced;
		}
	}

void ChanRU_UTF_string8::Unread(const UTF32* iSource, size_t iCount)
	{
	const string8::const_iterator stringStart = fString.begin();
	string8::const_iterator stringCurrent = stringStart + fPosition;
	const string8::const_iterator stringEnd = fString.end();

	size_t localCount = 0;

	while (localCount < iCount && Unicode::sDec(stringStart, stringCurrent, stringEnd))
		++localCount;
	fPosition = stringCurrent - stringStart;
//	return localCount;
	}

//size_t ChanRU_UTF_string8::UnreadableLimit()
//	{ return size_t(-1); }

const string8& ChanRU_UTF_string8::GetString8() const
	{ return fString; }

} // namespace ZooLib
