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
#include "zoolib/ZUnicode.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
// MARK: - ZStrimU_Std

ZStrimU_Std::ZStrimU_Std(ZTextDecoder* iDecoder, const ZStreamR& iStreamR)
:	fStrimR_StreamDecoder(iDecoder, iStreamR)
,	fStrimR_CRLFRemove(fStrimR_StreamDecoder)
,	fPos(0)
,	fLine(0)
,	fColumn(0)
	{}

void ZStrimU_Std::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;
	while (localDest < localDestEnd)
		{
		UTF32 theCP;
		if (fStack.empty())
			{
			if (not fStrimR_CRLFRemove.ReadCP(theCP))
				break;
			++fPos;

			if (ZUnicode::sIsEOL(theCP))
				{
				++fLine;
				fColumn = 0;
				}
			else
				{
				++fColumn;
				}
			}
		else
			{
			theCP = fStack.back();
			fStack.pop_back();
			}

		*localDest++ = theCP;
		}

	if (oCount)
		*oCount = localDest - oDest;
	}

void ZStrimU_Std::Imp_Unread(UTF32 iCP)
	{ fStack.push_back(iCP); }

size_t ZStrimU_Std::Imp_UnreadableLimit()
	{ return size_t(-1); }

void ZStrimU_Std::SetDecoder(ZTextDecoder* iDecoder)
	{ fStrimR_StreamDecoder.SetDecoder(iDecoder); }

ZTextDecoder* ZStrimU_Std::GetSetDecoder(ZTextDecoder* iDecoder)
	{ return fStrimR_StreamDecoder.GetSetDecoder(iDecoder); }

int ZStrimU_Std::GetPos()
	{ return fPos - fStack.size(); }

int ZStrimU_Std::GetLine()
	{
	int line = fLine;
	for (vector<UTF32>::const_reverse_iterator ii = fStack.rbegin(), rend = fStack.rend();
		ii != rend; ++ii)
		{
		if (ZUnicode::sIsEOL(*ii))
			--line;
		}
	return line;
	}

int ZStrimU_Std::GetColumn()
	{
	int column = fColumn;
	for (vector<UTF32>::const_reverse_iterator ii = fStack.rbegin(), rend = fStack.rend();
		ii != rend; ++ii)
		{
		if (ZUnicode::sIsEOL(*ii))
			column = -1;
		else
			--column;
		}
	return column;
	}

} // namespace ZooLib
