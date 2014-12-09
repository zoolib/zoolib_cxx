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

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Yad.h"

namespace ZooLib {

// =================================================================================================
// MARK: - YadR_Val_T

template <class Val_p>
class YadR_Val_T
:	public virtual YadR
	{
public:
	typedef YadR_Val_T YadBase_t;

	YadR_Val_T()
		{}

	template <class P0>
	YadR_Val_T(const P0& i0)
	:	fVal(i0)
		{}

	template <class P0, class P1>
	YadR_Val_T(const P0& i0, const P1& i1)
	:	fVal(i0, i1)
		{}

// Our protocol
	const Val_p& GetVal()
		{ return fVal; }

protected:
	const Val_p fVal;
	};

// =================================================================================================
// MARK: - YadStreamerRPos_Val_T

template <class Data_p>
class YadStreamerRPos_Val_T
:	public virtual YadStreamerR
,	public virtual Channer_T<ChanRPos_Bin_Data<Data_p>, ChanR_Bin>
	{
public:
	YadStreamerRPos_Val_T(const Data_p& iData)
	:	Channer_T<ChanRPos_Bin_Data<Data_p>, ChanR_Bin>(iData)
		{}

// From YadR
	bool IsSimple(const YadOptions& iOptions)
		{ return false; }
	};

// =================================================================================================
// MARK: - YadSeqR_Val_T

template <class Seq_p>
class YadSeqR_Val_T
:	public YadSeqR
	{
public:
	YadSeqR_Val_T(const Seq_p& iSeq)
	:	fSeq(iSeq)
	,	fPosition(0)
		{}

	YadSeqR_Val_T(const Seq_p& iSeq, uint64 iPosition)
	:	fSeq(iSeq)
	,	fPosition(iPosition)
		{}

// From YadSeqR via YadSeqR
	virtual ZRef<YadR> ReadInc()
		{
		if (fPosition < fSeq.Count())
			return sYadR(fSeq.Get(fPosition++));
		return null;
		}

// Our protocol
	const Seq_p& GetSeq()
		{ return fSeq; }

protected:
	const Seq_p fSeq;
	uint64 fPosition;
	};

// =================================================================================================
// MARK: - YadMapR_Val_T

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
