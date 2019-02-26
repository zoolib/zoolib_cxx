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

#include "zoolib/Unicode.h"

#include "zoolib/ChanRU_UTF_Std.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
#pragma mark - ChanRU_UTF_Std

ChanRU_UTF_Std::ChanRU_UTF_Std(const ChanR_UTF& iChanR)
:	fChanR_CRLFRemove(iChanR)
,	fPos(0)
,	fLine(0)
,	fColumn(0)
	{}

size_t ChanRU_UTF_Std::Read(UTF32* oDest, size_t iCount)
	{
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;
	while (localDest < localDestEnd)
		{
		if (not fStack.empty())
			{
			*localDest++ = fStack.back();
			fStack.pop_back();
			}
		else
			{
			if (NotQ<UTF32> theQ = sQRead(fChanR_CRLFRemove))
				{ break; }
			else
				{
				++fPos;

				if (Unicode::sIsEOL(*theQ))
					{
					++fLine;
					fColumn = 0;
					}
				else
					{
					++fColumn;
					}
				*localDest++ = *theQ;
				}
			}
		}
	return localDest - oDest;
	}

void ChanRU_UTF_Std::Unread(const UTF32* iSource, size_t iCount)
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		fStack.push_back(*iSource++);
//	return iCount;
	}

//size_t ChanRU_UTF_Std::UnreadableLimit()
//	{ return size_t(-1); }

int ChanRU_UTF_Std::GetPos()
	{ return int(fPos - fStack.size()); }

int ChanRU_UTF_Std::GetLine()
	{
	int line = int(fLine);
	for (vector<UTF32>::const_reverse_iterator ii = fStack.rbegin(), rend = fStack.rend();
		ii != rend; ++ii)
		{
		if (Unicode::sIsEOL(*ii))
			--line;
		}
	return line;
	}

int ChanRU_UTF_Std::GetColumn()
	{
	int column = int(fColumn);
	for (vector<UTF32>::const_reverse_iterator ii = fStack.rbegin(), rend = fStack.rend();
		ii != rend; ++ii)
		{
		if (Unicode::sIsEOL(*ii))
			column = -1;
		else
			--column;
		}
	return column;
	}

} // namespace ZooLib
