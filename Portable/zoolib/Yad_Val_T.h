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

#ifndef __ZooLib_Yad_Val_T_h__
#define __ZooLib_Yad_Val_T_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/Yad.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark YadR_Val_T

//template <class Val_p>
//class YadR_Val_T
//:	public /*virtual*/ YadR
//	{
//public:
//	typedef YadR_Val_T YadBase_t;
//
//	YadR_Val_T()
//		{}
//
//	template <class P0>
//	YadR_Val_T(const P0& i0)
//	:	fVal(i0)
//		{}
//
//	template <class P0, class P1>
//	YadR_Val_T(const P0& i0, const P1& i1)
//	:	fVal(i0, i1)
//		{}
//
//// Our protocol
//	const Val_p& GetVal()
//		{ return fVal; }
//
//protected:
//	const Val_p fVal;
//	};

// =================================================================================================
#pragma mark -
#pragma mark Chan_YadSeqAtRPos_T

template <class Seq_p>
class Chan_YadSeqAtRPos_T
:	public DeriveFrom
		<
		Aspect_Pos,
		Aspect_Read<RefYad>,
		Aspect_ReadAt<uint64, RefYad>,
		Aspect_Size
		>
	{
public:
	Chan_YadSeqAtRPos_T(const Seq_p& iSeq)
	:	fSeq(iSeq)
	,	fPosition(0)
		{}

	Chan_YadSeqAtRPos_T(const Seq_p& iSeq, uint64 iPosition)
	:	fSeq(iSeq)
	,	fPosition(iPosition)
		{}

// From Aspect_Pos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void SetPos(uint64 iPosition)
		{ fPosition = iPosition; }

// From Aspect_Read
	virtual size_t QRead(RefYad* oDest, size_t iCount)
		{
		RefYad* startDest = oDest;
		while (fPosition < fSeq.Size() && iCount--)
			*oDest++ = sYadR(fSeq.Get(fPosition++));
		return oDest - startDest;
		}

// From Aspect_ReadAt
	virtual size_t QReadAt(const uint64& iLoc, RefYad* oDest, size_t iCount)
		{
		RefYad* startDest = oDest;
		uint64 loc = iLoc;
		while (loc < fSeq.Size() && iCount--)
			*oDest++ = sYadR(fSeq.Get(loc++));
		return oDest - startDest;
		}

// From Aspect_Size
	virtual uint64 Size()
		{ return fSeq.Size(); }

// Our protocol
	const Seq_p& GetSeq()
		{ return fSeq; }

protected:
	const Seq_p fSeq;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark Chan_YadMapAtRPos_T

template <class Map_p, class Index_p = typename Map_p::Index_t>
class Chan_YadMapAtRPos_T
:	public DeriveFrom
		<
		Aspect_Read<NameRefYad>,
		Aspect_ReadAt<Name,RefYad>
		>
	{
public:
	typedef Index_p Index_t;

	Chan_YadMapAtRPos_T(const Map_p& iMap)
	:	fMap(iMap)
	,	fIndex(fMap.Begin())
		{}

	Chan_YadMapAtRPos_T(const Map_p& iMap, const Index_t& iIndex)
	:	fMap(iMap)
	,	fIndex(fMap.IndexOf(iMap, iIndex))
		{}

// From Aspect_Read
	virtual size_t QRead(NameRefYad* oDest, size_t iCount)
		{
		NameRefYad* startDest = oDest;
		if (fIndex != fMap.End() && iCount--)
			{
			oDest->first = fMap.NameOf(fIndex);
			oDest->second = sYadR(fMap.Get(fIndex));
			++fIndex;
			++oDest;
			}
		return oDest - startDest;
		}

// From Aspect_ReadAt
	virtual size_t QReadAt(const Name& iLoc, RefYad* oDest, size_t iCount)
		{
		if (iCount)
			{
			if (ZQ<typename Map_p::Val_t> theValQ = sQGet(fMap, iLoc))
				{
				*oDest = sYadR(*theValQ);
				return 1;
				}
			}
		return 0;
		}

// Our protocol
	const Map_p& GetMap()
		{ return fMap; }

protected:
	const Map_p fMap;
	Index_t fIndex;
	};

} // namespace ZooLib

#endif // __ZooLib_Yad_Val_T_h__
