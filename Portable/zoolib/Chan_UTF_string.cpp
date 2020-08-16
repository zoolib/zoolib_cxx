// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

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

size_t ChanRU_UTF_string8::Unread(const UTF32* iSource, size_t iCount)
	{
	const string8::const_iterator stringStart = fString.begin();
	string8::const_iterator stringCurrent = stringStart + fPosition;
	const string8::const_iterator stringEnd = fString.end();

	size_t localCount = 0;

	while (localCount < iCount && Unicode::sDec(stringStart, stringCurrent, stringEnd))
		++localCount;
	fPosition = stringCurrent - stringStart;
	return localCount;
	}

const string8& ChanRU_UTF_string8::GetString8() const
	{ return fString; }

} // namespace ZooLib
