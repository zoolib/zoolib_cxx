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

#include "zoolib/Chan_UTF_string.h" // For ChanW_UTF_string8
#include "zoolib/Visitor_Do_T.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_Std.h"
#include "zoolib/Yad_Val_T.h"

namespace ZooLib {

class YadStreamerRPos_Any
:	public ChanRPos_Bin_Data<Data_Any>
,	public virtual YadR_Any
,	public virtual YadStreamerR
	{
public:
	YadStreamerRPos_Any(const Data_Any& iData)
	:	YadR_Any(Any(iData))
	,	ChanRPos_Bin_Data<Data_Any>(iData)
		{}
	};

class YadSeqR_Any
:	public virtual YadSeqR_Val_T<Seq_Any>
,	public virtual YadR_Any
	{
public:
	YadSeqR_Any(const Seq_Any& iSeq)
	:	YadSeqR_Val_T<Seq_Any>(iSeq)
	,	YadR_Any(Any(iSeq))
		{}
	};

class YadMapR_Any
:	public virtual YadMapR_Val_T<Map_Any>
,	public virtual YadR_Any
	{
public:
	YadMapR_Any(const Map_Any& iMap)
	:	YadMapR_Val_T<Map_Any>(iMap)
	,	YadR_Any(Any(iMap))
		{}
	};

// =================================================================================================
#pragma mark -
#pragma mark sYadR

ZRef<YadR> sYadR(const Any& iVal)
	{
	if (const string8* theVal = iVal.PGet<string8>())
		return sYadR(*theVal);

	if (const Data_Any* theVal = iVal.PGet<Data_Any>())
		return sYadR(*theVal);

	if (const Seq_Any* theVal = iVal.PGet<Seq_Any>())
		return sYadR(*theVal);

	if (const Map_Any* theVal = iVal.PGet<Map_Any>())
		return sYadR(*theVal);

	return sMake_YadAtomR_Any(iVal);
	}

ZRef<YadStreamerR> sYadR(const Data_Any& iData)
	{ return new YadStreamerRPos_Any(iData); }

ZRef<YadSeqR> sYadR(const Seq_Any& iSeq)
	{ return new YadSeqR_Any(iSeq); }

ZRef<YadMapR> sYadR(const Map_Any& iMap)
	{ return new YadMapR_Any(iMap); }

// =================================================================================================
#pragma mark -
#pragma mark sFromYadR

namespace { // anonymous

class Visitor_Do_GetVal
:	public virtual Visitor_Do_T<Val_Any>
,	public virtual Visitor_Yad
	{
public:
// From Visitor_Yad
	Visitor_Do_GetVal(bool iRepeatedPropsAsSeq)
	:	fRepeatedPropsAsSeq(iRepeatedPropsAsSeq)
		{}

	virtual ZQ<Val_Any> QDo(const ZRef<Visitee>& iRep)
		{
		if (ZRef<YadR_Any> asAny = iRep.DynamicCast<YadR_Any>())
			return asAny->GetAny();
		return Visitor_Do_T<Val_Any>::QDo(iRep);
		}

	virtual void Visit_YadAtomR(const ZRef<YadAtomR>& iYadAtomR)
		{ this->pSetResult(iYadAtomR->AsAny()); }

	virtual void Visit_YadStreamerR(const ZRef<YadStreamerR>& iYadStreamerR)
		{ this->pSetResult(sReadAll_T<Data_Any>(sGetChan<ChanR_Bin>(iYadStreamerR))); }

	virtual void Visit_YadStrimmerR(const ZRef<YadStrimmerR>& iYadStrimmerR)
		{
		string8 theString;
		sCopyAll(sGetChan<ChanR_UTF>(iYadStrimmerR), ChanW_UTF_string8(&theString));
		this->pSetResult(theString);
		}

	virtual void Visit_YadSeqR(const ZRef<YadSeqR>& iYadSeqR)
		{
		Seq_Any theSeq;

		while (ZRef<YadR> theChild = iYadSeqR->ReadInc())
			theSeq.Append(this->Do(theChild));

		this->pSetResult(theSeq);
		}

	virtual void Visit_YadMapR(const ZRef<YadMapR>& iYadMapR)
		{
		Map_Any theMap;

		Name theName;
		while (ZRef<YadR> theChild = iYadMapR->ReadInc(theName))
			{
			Val_Any theVal = this->Do(theChild);
			if (fRepeatedPropsAsSeq)
				{
				if (Val_Any* prior = theMap.PMut(theName))
					{
					if (Seq_Any* priorSeq = prior->PMut<Seq_Any>())
						{
						priorSeq->Append(theVal);
						continue;
						}
					Seq_Any theSeq;
					theSeq.Append(*prior);
					theSeq.Append(theVal);
					theVal = theSeq;
					}
				}
			theMap.Set(theName, theVal);
			}

		this->pSetResult(theMap);
		}

private:
	bool fRepeatedPropsAsSeq;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Yad_Any

namespace Yad_Any {

ZQ<Val_Any> sQFromYadR(ZRef<YadR> iYadR)
	{ return sQFromYadR(false, iYadR); }

ZQ<Val_Any> sQFromYadR(bool iRepeatedPropsAsSeq, ZRef<YadR> iYadR)
	{ return Visitor_Do_GetVal(iRepeatedPropsAsSeq).QDo(iYadR); }

} // namespace Yad_Any

} // namespace ZooLib
