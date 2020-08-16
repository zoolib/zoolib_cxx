// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Chan_UTF_CFString.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Memory.h"
#include "zoolib/Unicode.h"

#include "zoolib/Apple/ZP_CF.h"

using std::min;
using std::max;

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark - ChanR_UTF_CFString

ChanR_UTF_CFString::ChanR_UTF_CFString(CFStringRef iStringRef)
:	fStringRef(iStringRef),
	fPosition(0)
	{}

ChanR_UTF_CFString::~ChanR_UTF_CFString()
	{}

void ChanR_UTF_CFString::ReadUTF16(UTF16* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	const size_t length = ::CFStringGetLength(fStringRef);
	if (0 == length)
		{
		if (oCountCP)
			*oCountCP = 0;
		if (oCountCU)
			*oCountCU = 0;
		}
	else
		{
		size_t countConsumed;
		const size_t countAvailable = length < fPosition ? 0 : length - fPosition;
		if (const UTF16* start = (const UTF16*)::CFStringGetCharactersPtr(fStringRef))
			{
			Unicode::sUTF16ToUTF16(
				start + fPosition, countAvailable,
				&countConsumed, nullptr,
				oDest, iCountCU,
				oCountCU,
				iCountCP, oCountCP);
			}
		else
			{
			UTF16 buffer[kBufSize];
			const size_t cuToCopy = min(iCountCU, min(countAvailable, kBufSize));
			::CFStringGetCharacters(fStringRef, CFRangeMake(fPosition, cuToCopy), (UniChar*)buffer);
			Unicode::sUTF16ToUTF16(
				buffer, cuToCopy,
				&countConsumed, nullptr,
				oDest, iCountCU,
				oCountCU,
				iCountCP, oCountCP);
			}
		fPosition += countConsumed;
		}
	}

// =================================================================================================
#pragma mark - ChanW_UTF_CFString

ChanW_UTF_CFString::ChanW_UTF_CFString(CFMutableStringRef iStringRef)
:	fStringRef(iStringRef)
	{}

ChanW_UTF_CFString::~ChanW_UTF_CFString()
	{}

size_t ChanW_UTF_CFString::WriteUTF16(const UTF16* iSource, size_t iCountCU)
	{
	if (iCountCU)
		::CFStringAppendCharacters(fStringRef, (const UniChar*)iSource, iCountCU);

	return iCountCU;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
