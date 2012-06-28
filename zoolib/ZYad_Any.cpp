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

#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Std.h"
#include "zoolib/ZYad_Val_T.h"

namespace ZooLib {

class ZYadStreamerRPos_Any
:	public ZYadStreamerRPos_Val_T<ZData_Any>
,	public ZYadR_Any
	{
public:
	ZYadStreamerRPos_Any(const ZData_Any& iData)
	:	ZYadStreamerRPos_Val_T<ZData_Any>(iData)
	,	ZYadR_Any(ZAny(iData))
		{}
	};

class ZYadSeqAtRPos_Any
:	public ZYadSeqAtRPos_Val_T<ZSeq_Any>
,	public ZYadR_Any
	{
public:
	ZYadSeqAtRPos_Any(const ZSeq_Any& iSeq)
	:	ZYadSeqAtRPos_Val_T<ZSeq_Any>(iSeq)
	,	ZYadR_Any(ZAny(iSeq))
		{}
	};

class ZYadMapAtRPos_Any
:	public ZYadMapAtRPos_Val_T<ZMap_Any>
,	public ZYadR_Any
	{
public:
	ZYadMapAtRPos_Any(const ZMap_Any& iMap)
	:	ZYadMapAtRPos_Val_T<ZMap_Any>(iMap)
	,	ZYadR_Any(ZAny(iMap))
		{}
	};

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(const ZAny& iVal)
	{
	if (const string8* theVal = iVal.PGet<string8>())
		return sYadR(*theVal);

	if (const ZData_Any* theVal = iVal.PGet<ZData_Any>())
		return sYadR(*theVal);

	if (const ZSeq_Any* theVal = iVal.PGet<ZSeq_Any>())
		return sYadR(*theVal);

	if (const ZMap_Any* theVal = iVal.PGet<ZMap_Any>())
		return sYadR(*theVal);

	return sMake_YadAtomR_Any(iVal);
	}

ZRef<ZYadStreamerR> sYadR(const ZData_Any& iData)
	{ return new ZYadStreamerRPos_Any(iData); }

ZRef<ZYadSeqAtRPos> sYadR(const ZSeq_Any& iSeq)
	{ return new ZYadSeqAtRPos_Any(iSeq); }

ZRef<ZYadMapAtRPos> sYadR(const ZMap_Any& iMap)
	{ return new ZYadMapAtRPos_Any(iMap); }

// =================================================================================================
// MARK: - sFromYadR

namespace { // anonymous

class Visitor_Do_GetVal
:	public virtual ZVisitor_Do_T<ZVal_Any>
,	public virtual ZVisitor_Yad_PreferRPos
	{
public:
// From ZVisitor_Yad
	Visitor_Do_GetVal(bool iRepeatedPropsAsSeq)
	:	fRepeatedPropsAsSeq(iRepeatedPropsAsSeq)
		{}

	virtual ZQ<ZVal_Any> QDo(const ZRef<ZVisitee>& iRep)
		{
		if (ZRef<ZYadR_Any> asAny = iRep.DynamicCast<ZYadR_Any>())
			return asAny->GetAny();
		return ZVisitor_Do_T<ZVal_Any>::QDo(iRep);
		}

	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
		{ this->pSetResult(iYadAtomR->AsAny()); }

	virtual void Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR)
		{ this->pSetResult(sReadAll_T<ZData_Any>(iYadStreamerR->GetStreamR())); }

	virtual void Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR)
		{ this->pSetResult(iYadStrimmerR->GetStrimR().ReadAll8()); }

	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
		{
		ZSeq_Any theSeq;

		while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
			theSeq.Append(this->Do(theChild));

		this->pSetResult(theSeq);
		}

	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
		{
		ZMap_Any theMap;

		ZName theName;
		while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
			{
			ZVal_Any theVal = this->Do(theChild);
			if (fRepeatedPropsAsSeq)
				{
				if (ZVal_Any* prior = theMap.PMut(theName))
					{
					if (ZSeq_Any* priorSeq = prior->PMut<ZSeq_Any>())
						{
						priorSeq->Append(theVal);
						continue;
						}
					ZSeq_Any theSeq;
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
// MARK: - sFromYadR

ZVal_Any sFromYadR(const ZVal_Any& iDefault, ZRef<ZYadR> iYadR)
	{ return sFromYadR(false, iDefault, iYadR); }

ZVal_Any sFromYadR(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault, ZRef<ZYadR> iYadR)
	{ return Visitor_Do_GetVal(iRepeatedPropsAsSeq).DDo(iDefault, iYadR); }

} // namespace ZooLib
