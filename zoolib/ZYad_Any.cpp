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

	return new ZYadAtomR_Any(iVal);
	}

ZRef<ZYadStreamR> sYadR(const ZData_Any& iData)
	{ return new ZYadStreamRPos_Any(iData); }

ZRef<ZYadSatRPos> sYadR(const ZSeq_Any& iSeq)
	{ return new ZYadSatRPos_Any(iSeq); }

ZRef<ZYadMatRPos> sYadR(const ZMap_Any& iMap)
	{ return new ZYadMatRPos_Any(iMap); }

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

	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
		{ this->pSetResult(iYadAtomR->AsAny()); }

	virtual void Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
		{ this->pSetResult(sReadAll_T<ZData_Any>(iYadStreamR->GetStreamR())); }

	virtual void Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
		{ this->pSetResult(iYadStrimR->GetStrimR().ReadAll8()); }

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

		string theName;
		while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
			{
			ZVal_Any theVal = this->Do(theChild);
			if (fRepeatedPropsAsSeq)
				{
				if (ZVal_Any* prior = theMap.PGetMutable(theName))
					{
					if (ZSeq_Any* priorSeq = prior->PGetMutable<ZSeq_Any>())
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
	{
	if (ZRef<ZYadAtomR_Any> asAtom = iYadR.DynamicCast<ZYadAtomR_Any>())
		return asAtom->GetAny();

	if (ZRef<ZYadStrimU_String> asString = iYadR.DynamicCast<ZYadStrimU_String>())
		return asString->GetStrim().GetString8();

	if (ZRef<ZYadStreamRPos_Any> asYadStream = iYadR.DynamicCast<ZYadStreamRPos_Any>())
		return asYadStream->GetStream().GetData();

	if (ZRef<ZYadMatRPos_Any> asMap = iYadR.DynamicCast<ZYadMatRPos_Any>())
		return asMap->GetMap();

	if (ZRef<ZYadSatRPos_Any> asSeq = iYadR.DynamicCast<ZYadSatRPos_Any>())
		return asSeq->GetSeq();

	return Visitor_Do_GetVal(iRepeatedPropsAsSeq).DDo(iDefault, iYadR);
	}

} // namespace ZooLib
