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

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZAny& iVal)
	{
	if (const string8* theVal = iVal.PGet_T<string8>())
		return sMakeYadR(*theVal);

	if (const ZData_Any* theVal = iVal.PGet_T<ZData_Any>())
		return sMakeYadR(*theVal);

	if (const ZSeq_Any* theVal = iVal.PGet_T<ZSeq_Any>())
		return sMakeYadR(*theVal);

	if (const ZMap_Any* theVal = iVal.PGet_T<ZMap_Any>())
		return sMakeYadR(*theVal);

	return new ZYadAtomR_Any(iVal);
	}

ZRef<ZYadStreamR> sMakeYadR(const ZData_Any& iData)
	{ return new ZYadStreamRPos_Any(iData); }

ZRef<ZYadSeqRPos> sMakeYadR(const ZSeq_Any& iSeq)
	{ return new ZYadSeqRPos_Any(iSeq); }

ZRef<ZYadMapRPos> sMakeYadR(const ZMap_Any& iMap)
	{ return new ZYadMapRPos_Any(iMap); }

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

namespace { // anonymous

class Visitor_Do_GetVal
:	public virtual ZVisitor_Do_T<ZVal_Any>
,	public virtual ZVisitor_Yad
	{
public:
	Visitor_Do_GetVal(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault);

// From ZVisitor_Yad
	virtual void Visit_YadAtomR(ZRef<ZYadAtomR> iYadAtomR);
	virtual void Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual void Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual void Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR);
	virtual void Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);

private:
	bool fRepeatedPropsAsSeq;
	const ZVal_Any& fDefault;
	};

Visitor_Do_GetVal::Visitor_Do_GetVal(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault)
:	fRepeatedPropsAsSeq(iRepeatedPropsAsSeq)
,	fDefault(iDefault)
	{}

void Visitor_Do_GetVal::Visit_YadAtomR(ZRef<ZYadAtomR> iYadAtomR)
	{ this->pSetResult(iYadAtomR->AsAny()); }

void Visitor_Do_GetVal::Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR)
	{ this->pSetResult(sReadAll_T<ZData_Any>(iYadStreamR->GetStreamR())); }

void Visitor_Do_GetVal::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{ this->pSetResult(iYadStrimR->GetStrimR().ReadAll8()); }

void Visitor_Do_GetVal::Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR)
	{
	ZSeq_Any theSeq;

	while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
		theSeq.Append(this->Do(theChild));

	this->pSetResult(theSeq);
	}

void Visitor_Do_GetVal::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
	{
	ZMap_Any theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		{
		ZVal_Any theVal = this->Do(theChild);
		if (fRepeatedPropsAsSeq)
			{
			if (ZVal_Any* prior = theMap.PGet(theName))
				{
				if (ZSeq_Any* priorSeq = prior->PGet_T<ZSeq_Any>())
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

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

ZVal_Any sFromYadR(const ZVal_Any& iDefault, ZRef<ZYadR> iYadR)
	{ return sFromYadR(false, iDefault, iYadR); }

ZVal_Any sFromYadR(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadAtomR_Any> asAtom = iYadR.DynamicCast<ZYadAtomR_Any>())
		return asAtom->GetAny();

	if (ZRef<ZYadStrimU_String> asString = iYadR.DynamicCast<ZYadStrimU_String>())
		return asString->GetStrim().GetString8();

	if (ZRef<ZYadStreamRPos_Any> asYadStream = iYadR.DynamicCast<ZYadStreamRPos_Any>())
		return asYadStream->GetStream().GetData();

	if (ZRef<ZYadMapRPos_Any> asMap = iYadR.DynamicCast<ZYadMapRPos_Any>())
		return asMap->GetMap();

	if (ZRef<ZYadSeqRPos_Any> asSeq = iYadR.DynamicCast<ZYadSeqRPos_Any>())
		return asSeq->GetSeq();

	return Visitor_Do_GetVal(iRepeatedPropsAsSeq, iDefault).Do(iYadR);
	}

} // namespace ZooLib
