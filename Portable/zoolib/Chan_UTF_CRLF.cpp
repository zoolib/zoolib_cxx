/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/Unicode.h"

#include "zoolib/Chan_UTF_CRLF.h"
#include "zoolib/Compat_algorithm.h" // For min
#include "zoolib/ZDebug.h"

using std::min;

namespace ZooLib {

//##static const UTF32 spLF[] = { '\n' };
//##static const UTF32 spCR[] = { '\r' };

static const UTF32 LF = 0x0A; // Probably the same as '\n'
static const UTF32 CR = 0x0D; // Probably the same as '\r'

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_CRLFRemove

ChanR_UTF_CRLFRemove::ChanR_UTF_CRLFRemove(const ChanR_UTF& iChanR)
:	fChanR(iChanR),
	fReplacementCP('\n')
	{}

ChanR_UTF_CRLFRemove::ChanR_UTF_CRLFRemove(UTF32 iReplacementCP, const ChanR_UTF& iChanR)
:	fChanR(iChanR),
	fReplacementCP(iReplacementCP)
	{
	ZAssertStop(2, Unicode::sIsValid(fReplacementCP));
	}

size_t ChanR_UTF_CRLFRemove::QRead(UTF32* oDest, size_t iCount)
	{
	UTF32 buffer[kBufSize];

	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;
	while (localDestEnd > localDest)
		{
		const size_t countRead = sQRead(buffer, min(size_t(localDestEnd - localDest), kBufSize), fChanR);
		if (countRead == 0)
			break;

		const UTF32* readFrom = buffer;
		const UTF32* readFromEnd = buffer + countRead;
		for (;;)
			{
			UTF32 currCP;
			if (not Unicode::sReadInc(readFrom, readFromEnd, currCP))
				break;

			switch (currCP)
				{
				case CR:
					{
					*localDest++ = fReplacementCP;
					fLastWasCR = true;
					break;
					}
				case LF:
					{
					if (!fLastWasCR)
						*localDest++ = fReplacementCP;
					fLastWasCR = false;
					break;
					}
				default:
					{
					*localDest++ = currCP;
					fLastWasCR = false;
					break;
					}
				}
			}
		}

	return localDest - oDest;
	}

} // namespace ZooLib
