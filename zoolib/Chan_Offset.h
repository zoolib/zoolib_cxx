/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Chan_Offset_h__
#define __ZooLib_Chan_Offset_h__ 1
#include "zconfig.h"

#include "zoolib/ChanCount.h"
#include "zoolib/ChanCountSet.h"
#include "zoolib/ChanPos.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanCount_Offset

class ChanCount_Offset
:	public ChanCount
	{
public:
	ChanCount_Offset(uint64 iOffset, const ChanCount& iChanCount)
	:	fChanCount(iChanCount)
	,	fOffset(iOffset)
		{}

// From ChanCount
	virtual uint64 Count()
		{ return sCount(fChanCount) - fOffset; }

protected:
	const ChanCount& fChanCount;
	const uint64 iOffset;
	};

// =================================================================================================
// MARK: - ChanCountSet_Offset

class ChanCountSet_Offset
:	public ChanCountSet
	{
public:
	ChanCountSet_Offset(uint64 iOffset, const ChanCountSet& iChanCountSet)
	:	fChanCountSet(iChanCountSet)
	,	fOffset(iOffset)
		{}

// From ChanCountSet
	virtual void CountSet(uint64 iCount)
		{ sCountSet(iCount + fOffset, fChanCountSet); }

protected:
	const ChanCountSet& fChanCountSet;
	const uint64 iOffset;
	};

// =================================================================================================
// MARK: - ChanPos_Offset

class ChanPos_Offset
:	public ChanPos
	{
public:
	ChanPos_Offset(uint64 iOffset, const ChanPos& iChanPos)
	:	fChanPos(iChanPos)
	,	fOffset(iOffset)
		{}

	virtual uint64 Pos()
		{ return sPos(fChanPos) - fOffset; }

	virtual void SetPos(uint64 iPos)
		{ sSetPos(iPos + fOffset, fChanPos); }

protected:
	const ChanPos& fChanPos;
	const uint64 iOffset;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Offset_h__
