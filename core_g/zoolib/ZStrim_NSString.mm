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

#include "zoolib/ZStrim_NSString.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZRef_NS.h"

using std::min;
using std::max;

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
// MARK: - ZStrimR_NSString

ZStrimR_NSString::ZStrimR_NSString(NSString* iString)
:	fString(iString)
,	fPosition(0)
	{}

ZStrimR_NSString::~ZStrimR_NSString()
	{}

void ZStrimR_NSString::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	const size_t length = [fString.Get() length];
	if (0 == length)
		{
		if (oCount)
			*oCount = 0;
		}
	else
		{
		size_t countConsumed;
		const size_t countAvailable = length < fPosition ? 0 : length - fPosition;
		UTF16 buffer[kBufSize];
		const size_t cuToCopy = min(iCount, min(countAvailable, kBufSize));
		const NSRange theRange = { fPosition, cuToCopy };
		[fString.Get() getCharacters:(unichar*)buffer range:theRange];
		ZUnicode::sUTF16ToUTF32(
			buffer, cuToCopy,
			&countConsumed, nullptr,
			oDest, iCount,
			oCount);
		fPosition += countConsumed;
		}
	}

bool ZStrimR_NSString::Imp_ReadCP(UTF32& oCP)
	{
	using namespace ZUnicode;
	const size_t length = [fString.Get() length];
	for (;;)
		{
		if (fPosition >= length)
			{
			// We've run off the end.
			return false;
			}

		const unichar theCU = [fString.Get() characterAtIndex:fPosition++];
		if (sIsSmallNormal(theCU))
			{
			oCP = theCU;
			return true;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (fPosition >= length)
				{
				// But we'd run off the end. Restore the value of ioCurrent to indicate this.
				--fPosition;
				return false;
				}

			const unichar theCU2 = [fString.Get() characterAtIndex:fPosition++];
			if (sIsLowSurrogate(theCU2))
				{
				oCP = sUTF32FromSurrogates(theCU, theCU2);
				return true;
				}
			--fPosition;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			oCP = theCU;
			return true;
			}
		else
			{
			// Must be an out of order low surrogate.
			}
		}
	return false;
	}

// =================================================================================================
// MARK: - ZStrimW_NSString

ZStrimW_NSString::ZStrimW_NSString(NSMutableString* ioString)
:	fString(ioString)
	{}

ZStrimW_NSString::~ZStrimW_NSString()
	{}

void ZStrimW_NSString::Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (iCountCU)
		{
		ZRef<NSString> asString = sAdopt&
			[[NSString alloc] initWithCharacters:(unichar*)iSource length:iCountCU];
		[fString appendString:asString];
		}

	if (oCountCU)
		*oCountCU = iCountCU;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
