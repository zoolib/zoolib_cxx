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

template <class EE, class LL>
class Aspect_ReadAt
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t QReadAt(const LL& iLoc, EE* oDest, size_t iCount) = 0;
	};

template <class EE, class LL>
using ChanReadAt = DeriveFrom<Aspect_ReadAt<EE,LL>>;

template <class EE, class LL>
inline size_t sQReadAt(const ChanReadAt<LL,EE>& iChan, const LL& iLoc, EE* oDest, size_t iCount)
	{ return sNonConst(iChan).QReadAt(iLoc, oDest, iCount); }

typedef ChanReadAt<ZRef<YadR>,Name> YadMapAt;
typedef ChanReadAt<ZRef<YadR>,uint64> YadSeqAt;

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
#pragma mark ChanR_YadSeq_Val_T

template <class Seq_p>
class ChanR_YadSeq_Val_T
:	public ChanR_RefYad
	{
public:
	ChanR_YadSeq_Val_T(const Seq_p& iSeq)
	:	fSeq(iSeq)
	,	fPosition(0)
		{}

	ChanR_YadSeq_Val_T(const Seq_p& iSeq, uint64 iPosition)
	:	fSeq(iSeq)
	,	fPosition(iPosition)
		{}

// From ChanR_RefYad
	virtual size_t QRead(ZRef<YadR>* oDest, size_t iCount)
		{
		ZRef<YadR>* startDest = oDest;
		while (fPosition < fSeq.Count() && iCount--)
			*oDest++ = sYadR(fSeq.Get(fPosition++));
		return oDest - startDest;
		}

// Our protocol
	const Seq_p& GetSeq()
		{ return fSeq; }

protected:
	const Seq_p fSeq;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark YadMapR_Val_T

template <class Map_p, class Index_p = typename Map_p::Index_t>
class YadMapR_Val_T
:	public YadMapR
	{
public:
	typedef Index_p Index_t;

	YadMapR_Val_T(const Map_p& iMap)
	:	fMap(iMap)
	,	fIndex(fMap.Begin())
		{}

	YadMapR_Val_T(const Map_p& iMap, const Index_t& iIndex)
	:	fMap(iMap)
	,	fIndex(fMap.IndexOf(iMap, iIndex))
		{}

// From YadMapR
	ZRef<YadR> ReadInc(Name& oName)
		{
		if (fIndex != fMap.End())
			{
			oName = fMap.NameOf(fIndex);
			return sYadR(fMap.Get(fIndex++));
			}
		return null;
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
