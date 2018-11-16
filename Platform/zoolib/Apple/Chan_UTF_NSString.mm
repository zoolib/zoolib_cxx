/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/Apple/Chan_UTF_NSString.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/Memory.h"
#include "zoolib/Unicode.h"

#include "zoolib/Apple/ZRef_NS.h"

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
		ZRef<NSString> asString = sAdopt&
			[[NSString alloc] initWithCharacters:(unichar*)iSource length:iCountCU];
		[fString appendString:asString];
		}
	return iCountCU;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
