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

#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZAny& iVal)
	{
	if (const string8* theVal = iVal.PGet_T<string8>())
		return sMakeYadR(*theVal);

	if (const ZData_Any* theVal = iVal.PGet_T<ZData_Any>())
		return new ZYadStreamRPos_Any(*theVal);

	if (const ZSeq_Any* theVal = iVal.PGet_T<ZSeq_Any>())
		return sMakeYadR(*theVal);

	if (const ZMap_Any* theVal = iVal.PGet_T<ZMap_Any>())
		return sMakeYadR(*theVal);

	return new ZYadPrimR_Any(iVal);
	}

ZRef<ZYadStreamR> sMakeYadR(const ZData_Any& iData)
	{ return new ZYadStreamRPos_Any(iData); }

ZRef<ZYadSeqR> sMakeYadR(const ZSeq_Any& iSeq)
	{ return new ZYadSeqRPos_Any(iSeq); }

ZRef<ZYadMapR> sMakeYadR(const ZMap_Any& iMap)
	{ return new ZYadMapRPos_Any(iMap); }

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

namespace ZANONYMOUS {

class Visitor_Yad_GetVal_Any : public ZVisitor_Yad
	{
public:
	Visitor_Yad_GetVal_Any(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault);

// From ZVisitor_Yad
	virtual bool Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR);
	virtual bool Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual bool Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual bool Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR);
	virtual bool Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);

	bool fRepeatedPropsAsSeq;
	const ZVal_Any& fDefault;
	ZVal_Any fOutput;
	};

} // anonymous namespace

ZVal_Any sFromYadR(const ZVal_Any& iDefault, ZRef<ZYadR> iYadR)
	{ return sFromYadR(false, iDefault, iYadR); }

ZVal_Any sFromYadR(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadPrimR_Any> asPrim = iYadR.DynamicCast<ZYadPrimR_Any>())
		return asPrim->GetAny();

	if (ZRef<ZYadStrimU_String> asString = iYadR.DynamicCast<ZYadStrimU_String>())
		return asString->GetStrim().GetString8();

	if (ZRef<ZYadStreamRPos_Any> asYadStream = iYadR.DynamicCast<ZYadStreamRPos_Any>())
		return asYadStream->GetStream().GetData();

	if (ZRef<ZYadMapRPos_Any> asMap = iYadR.DynamicCast<ZYadMapRPos_Any>())
		return asMap->GetMap();

	if (ZRef<ZYadSeqRPos_Any> asSeq = iYadR.DynamicCast<ZYadSeqRPos_Any>())
		return asSeq->GetSeq();

	Visitor_Yad_GetVal_Any theVisitor(iRepeatedPropsAsSeq, iDefault);
	iYadR->Accept(theVisitor);
	return theVisitor.fOutput;
	}

Visitor_Yad_GetVal_Any::Visitor_Yad_GetVal_Any(bool iRepeatedPropsAsSeq, const ZVal_Any& iDefault)
:	fRepeatedPropsAsSeq(iRepeatedPropsAsSeq)
,	fDefault(iDefault)
	{}

bool Visitor_Yad_GetVal_Any::Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR)
	{
	fOutput = iYadPrimR->AsAny();
	return true;
	}

bool Visitor_Yad_GetVal_Any::Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR)
	{
	fOutput = sReadAll_T<ZData_Any>(iYadStreamR->GetStreamR());
	return true;
	}

bool Visitor_Yad_GetVal_Any::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{
	fOutput = iYadStrimR->GetStrimR().ReadAll8();
	return true;
	}

bool Visitor_Yad_GetVal_Any::Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR)
	{
	ZSeq_Any theSeq;

	while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
		theSeq.Append(sFromYadR(fDefault, theChild));

	fOutput = theSeq;
	return true;
	}

bool Visitor_Yad_GetVal_Any::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
	{
	ZMap_Any theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		{
		ZVal_Any theVal = sFromYadR(fRepeatedPropsAsSeq, fDefault, theChild);
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

	fOutput = theMap;
	return true;
	}

NAMESPACE_ZOOLIB_END
