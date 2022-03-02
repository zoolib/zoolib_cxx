// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_Offset_h__
#define __ZooLib_Chan_Offset_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanPos_Offset

template <class Chan_p>
class ChanPos_Offset
:	public virtual ChanFilter<Chan_P>
	{
	typedef ChanFilter<Chan_P> inherited;
public:
	ChanPos_Offset(const Chan_p& iChan, uint64 iOffset)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	ChanPos_Offset(uint64 iOffset, const Chan_p& iChan)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	virtual uint64 Pos()
		{ return sPos(inherited::pGetChan()) - fOffset; }

	virtual void PosSet(uint64 iPos)
		{ sPosSet(inherited::pGetChan(), iPos + fOffset); }

protected:
	const uint64 fOffset;
	};

// =================================================================================================
#pragma mark - ChanSize_Offset

template <class Chan_p>
class ChanSize_Offset
:	public virtual ChanFilter<Chan_P>
	{
	typedef ChanFilter<Chan_P> inherited;
public:
	ChanSize_Offset(const Chan_p& iChan, uint64 iOffset)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	ChanSize_Offset(uint64 iOffset, const Chan_p& iChan)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

// From ChanSize
	virtual uint64 Size()
		{ return sSize(inherited::pGetChan()) - fOffset; }

protected:
	const uint64 fOffset;
	};

// =================================================================================================
#pragma mark - ChanSizeSet_Offset

template <class Chan_p>
class ChanSizeSet_Offset
:	public virtual ChanFilter<Chan_P>
	{
	typedef ChanFilter<Chan_P> inherited;
public:
	ChanSizeSet_Offset(const Chan_p& iChan, uint64 iOffset)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	ChanSizeSet_Offset(uint64 iOffset, const Chan_p& iChan)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{ sSizeSet(inherited::pGetChan(), iSize + fOffset); }

protected:
	const uint64 fOffset;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Offset_h__
