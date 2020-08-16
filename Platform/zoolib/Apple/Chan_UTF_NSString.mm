// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Chan_UTF_NSString.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/Memory.h"
#include "zoolib/Unicode.h"

#include "zoolib/Apple/ZP_NS.h"

using std::min;
using std::max;

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark - ChanR_UTF_NSString

ChanR_UTF_NSString::ChanR_UTF_NSString(NSString* iString)
:	fString(iString)
,	fPosition(0)
	{}

ChanR_UTF_NSString::~ChanR_UTF_NSString()
	{}

size_t ChanR_UTF_NSString::Read(UTF32* oDest, size_t iCount)
	{
	if (const size_t length = [fString.Get() length])
		{
		size_t countConsumed;
		const size_t countAvailable = length < fPosition ? 0 : length - fPosition;
		UTF16 buffer[kBufSize];
		const size_t cuToCopy = min(iCount, min(countAvailable, kBufSize));
		const NSRange theRange = { fPosition, cuToCopy };
		[fString.Get() getCharacters:(unichar*)buffer range:theRange];
		size_t countProduced;
		Unicode::sUTF16ToUTF32(
			buffer, cuToCopy,
			&countConsumed, nullptr,
			oDest, iCount,
			&countProduced);
		fPosition += countConsumed;
		return countProduced;
		}
	return 0;
	}

// =================================================================================================
#pragma mark - ChanW_UTF_NSString

ChanW_UTF_NSString::ChanW_UTF_NSString(NSMutableString* ioString)
:	fString(ioString)
	{}

ChanW_UTF_NSString::~ChanW_UTF_NSString()
	{}

size_t ChanW_UTF_NSString::WriteUTF16(const UTF16* iSource, size_t iCountCU)
	{
	if (iCountCU)
		{
		ZP<NSString> asString = sAdopt&
			[[NSString alloc] initWithCharacters:(unichar*)iSource length:iCountCU];
		[fString appendString:asString];
		}
	return iCountCU;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
