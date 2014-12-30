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

#include "zoolib/ChanPos.h"
#include "zoolib/ChanSize.h"
#include "zoolib/ChanSizeSet.h"

namespace ZooLib {

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

// =================================================================================================
// MARK: - ChanSize_Offset

class ChanSize_Offset
:	public ChanSize
	{
public:
	ChanSize_Offset(uint64 iOffset, const ChanSize& iChanSize)
	:	fChanSize(iChanSize)
	,	fOffset(iOffset)
		{}

// From ChanSize
	virtual uint64 Size()
		{ return sSize(fChanSize) - fOffset; }

protected:
	const ChanSize& fChanSize;
	const uint64 iOffset;
	};

// =================================================================================================
// MARK: - ChanSizeSet_Offset

class ChanSizeSet_Offset
:	public ChanSizeSet
	{
public:
	ChanSizeSet_Offset(uint64 iOffset, const ChanSizeSet& iChanSizeSet)
	:	fChanSizeSet(iChanSizeSet)
	,	fOffset(iOffset)
		{}

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{ sSizeSet(iSize + fOffset, fChanSizeSet); }

protected:
	const ChanSizeSet& fChanSizeSet;
	const uint64 iOffset;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Offset_h__
