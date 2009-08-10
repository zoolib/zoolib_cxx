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

#include "zoolib/ZDebug.h"
#include "zoolib/ZStrimU_Std.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_Std

ZStrimU_Std::ZStrimU_Std(ZTextDecoder* iDecoder, const ZStreamR& iStreamR)
:	fStrimR_StreamDecoder(iDecoder, iStreamR),
	fStrimR_CRLFRemove(fStrimR_StreamDecoder),
	fLineCount(0)
	{}

void ZStrimU_Std::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iCount;
	while (localDest < localDestEnd)
		{
		UTF32 theCP;
		if (fStack.empty())
			{
			if (!fStrimR_CRLFRemove.ReadCP(theCP))
				break;
			}
		else
			{
			theCP = fStack.back();
			fStack.pop_back();
			}

		if (theCP == '\n')
			++fLineCount;
		*localDest++ = theCP;
		}

	if (oCount)
		*oCount = localDest - iDest;
	}

void ZStrimU_Std::Imp_Unread(UTF32 iCP)
	{
	if (iCP == '\n')
		--fLineCount;
	fStack.push_back(iCP);
	}

size_t ZStrimU_Std::Imp_UnreadableLimit()
	{ return size_t(-1); }

void ZStrimU_Std::SetDecoder(ZTextDecoder* iDecoder)
	{ fStrimR_StreamDecoder.SetDecoder(iDecoder); }

ZTextDecoder* ZStrimU_Std::SetDecoderReturnOld(ZTextDecoder* iDecoder)
	{ return fStrimR_StreamDecoder.SetDecoderReturnOld(iDecoder); }

size_t ZStrimU_Std::GetLineCount()
	{ return fLineCount; }

NAMESPACE_ZOOLIB_END
