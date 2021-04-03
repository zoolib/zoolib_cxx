// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanRU_UTF_Std.h"

#include "zoolib/ChanR.h"
#include "zoolib/Unicode.h"

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
			ZQ<UTF32> theQ = sQRead(fChanR_CRLFRemove);
			if (not theQ)
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

size_t ChanRU_UTF_Std::Unread(const UTF32* iSource, size_t iCount)
	{
	for (size_t theCount = iCount; theCount; --theCount)
		fStack.push_back(*iSource++);
	return iCount;
	}

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
