/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZYad_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_CFString.h"
#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZVal_CF.h"

#include <CoreFoundation/CFString.h>

namespace ZooLib {

using std::min;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - ZYadAtomR_CF

ZYadAtomR_CF::ZYadAtomR_CF(CFTypeRef iVal)
:	ZYadR_CF(iVal)
	{}

ZAny ZYadAtomR_CF::AsAny()
	{ return ZUtil_CF::sAsAny(this->GetVal()); }

// =================================================================================================
// MARK: - ZYadStreamRPos_CF

ZYadStreamRPos_CF::ZYadStreamRPos_CF(CFDataRef iDataRef)
:	ZYadR_CF(iDataRef)
,	ZStreamerRPos_CF(iDataRef)
	{}

ZYadStreamRPos_CF::~ZYadStreamRPos_CF()
	{}

bool ZYadStreamRPos_CF::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
// MARK: - ZYadStrimR_CF

ZYadStrimR_CF::ZYadStrimR_CF(CFStringRef iStringRef)
:	ZYadR_CF(iStringRef)
,	ZStrimmerR_CFString(iStringRef)
	{}

// =================================================================================================
// MARK: - ZYadSeqRPos_CF

ZYadSeqRPos_CF::ZYadSeqRPos_CF(CFArrayRef iArray)
:	ZYadR_CF(iArray)
,	YadSeqBase_t(iArray)
	{}

ZYadSeqRPos_CF::ZYadSeqRPos_CF(CFArrayRef iArray, uint64 iPosition)
:	ZYadR_CF(iArray)
,	YadSeqBase_t(iArray, iPosition)
	{}

// =================================================================================================
// MARK: - ZYadMapRPos_CF

namespace { // anonymous

struct GatherContents_t
	{
	vector<CFStringRef>* fNames;
	vector<CFTypeRef>* fValues;
	};

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	GatherContents_t* theParam = static_cast<GatherContents_t*>(iRefcon);
	theParam->fNames->push_back(static_cast<CFStringRef>(iKey));
	theParam->fValues->push_back(static_cast<CFTypeRef>(iValue));
	}

} // anonymous namespace

ZYadMapRPos_CF::ZYadMapRPos_CF(CFDictionaryRef iDictionary,
	uint64 iPosition,
	const std::vector<CFStringRef>& iNames,
	const std::vector<CFTypeRef>& iValues)
:	ZYadR_CF(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(iPosition)
,	fNames(iNames)
,	fValues(iValues)
	{}

ZYadMapRPos_CF::ZYadMapRPos_CF(CFDictionaryRef iDictionary)
:	ZYadR_CF(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(0)
	{
	GatherContents_t theParam;
	theParam.fNames = &fNames;
	theParam.fValues = &fValues;
	::CFDictionaryApplyFunction(fDictionary, spGatherContents, &theParam);
	}

ZRef<ZYadR> ZYadMapRPos_CF::ReadInc(string& oName)
	{
	if (fPosition < fNames.size())
		{
		oName = ZUtil_CF::sAsUTF8(fNames.at(fPosition));
		return sYadR(fValues[fPosition++]);
		}
	return null;
	}

ZRef<ZYadMapRClone> ZYadMapRPos_CF::Clone()
	{ return new ZYadMapRPos_CF(fDictionary, fPosition, fNames, fValues); }

void ZYadMapRPos_CF::SetPosition(const std::string& iName)
	{
	for (fPosition = 0; fPosition < fNames.size(); ++fPosition)
		{
		if (iName == ZUtil_CF::sAsUTF8(fNames.at(fPosition)))
			break;
		}
	}

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(CFTypeRef iVal)
	{
	const ZVal_CF theVal = iVal;

	if (ZQ<ZMap_CF> theQ = theVal.QGet<ZMap_CF>())
		return new ZYadMapRPos_CF(theQ.Get());

	if (ZQ<ZSeq_CF> theQ = theVal.QGet<ZSeq_CF>())
		return new ZYadSeqRPos_CF(theQ.Get());

	if (ZQ<ZData_CF> theQ = theVal.QGet<ZData_CF>())
		return new ZYadStreamRPos_CF(theQ.Get());

	if (ZQ<ZRef<CFStringRef> > theQ = theVal.QGet<ZRef<CFStringRef> >())
		return sYadR(theQ.Get());

	return new ZYadAtomR_CF(iVal);
	}

ZRef<ZYadR> sYadR(const ZRef<CFTypeRef>& iVal)
	{ return sYadR(iVal.Get()); }

ZRef<ZYadStrimR> sYadR(CFMutableStringRef iString)
	{ return new ZYadStrimR_CF(iString); }

ZRef<ZYadStrimR> sYadR(CFStringRef iString)
	{ return new ZYadStrimR_CF(iString); }

ZRef<ZYadStrimR> sYadR(const ZRef<CFStringRef>& iString)
	{ return sYadR(iString.Get()); }

ZRef<ZYadStreamR> sYadR(CFMutableDataRef iData)
	{ return new ZYadStreamRPos_CF(iData); }

ZRef<ZYadStreamR> sYadR(CFDataRef iData)
	{ return new ZYadStreamRPos_CF(iData); }

ZRef<ZYadStreamR> sYadR(const ZRef<CFDataRef>& iData)
	{ return sYadR(iData.Get()); }

ZRef<ZYadSeqRPos> sYadR(CFMutableArrayRef iArray)
	{ return new ZYadSeqRPos_CF(iArray); }

ZRef<ZYadSeqRPos> sYadR(CFArrayRef iArray)
	{ return new ZYadSeqRPos_CF(iArray); }

ZRef<ZYadSeqRPos> sYadR(const ZRef<CFArrayRef>& iArray)
	{ return sYadR(iArray.Get()); }

ZRef<ZYadMapRPos> sYadR(CFMutableDictionaryRef iDictionary)
	{ return new ZYadMapRPos_CF(iDictionary); }

ZRef<ZYadMapRPos> sYadR(CFDictionaryRef iDictionary)
	{ return new ZYadMapRPos_CF(iDictionary); }

ZRef<ZYadMapRPos> sYadR(const ZRef<CFDictionaryRef>& iDictionary)
	{ return sYadR(iDictionary.Get()); }

// =================================================================================================
// MARK: - sFromYadR

namespace { // anonymous

class Visitor_GetVal : public ZVisitor_Yad
	{
public:
	Visitor_GetVal(ZRef<CFTypeRef> iDefault);

// From ZVisitor_Yad
	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR);
	virtual void Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR);
	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR);
	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR);

// Our protocol
	ZRef<CFTypeRef> GetVal(ZRef<ZYadR> iYadR);

private:
	ZRef<CFTypeRef> fDefault;
	ZRef<CFTypeRef> fOutput;
	};

} // anonymous namespace

Visitor_GetVal::Visitor_GetVal(ZRef<CFTypeRef> iDefault)
:	fDefault(iDefault)
	{}

void Visitor_GetVal::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ fOutput = ZUtil_CF::sDAsCFType(fDefault, iYadAtomR->AsAny()); }

void Visitor_GetVal::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{ fOutput = sReadAll_T<ZData_CF>(iYadStreamR->GetStreamR()); }

void Visitor_GetVal::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{
	ZRef<CFMutableStringRef> result = ZUtil_CF::sStringMutable();
	ZStrimW_CFString(result).CopyAllFrom(iYadStrimR->GetStrimR());
	fOutput = result;
	}

void Visitor_GetVal::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{
	ZSeq_CF theSeq;

	while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
		theSeq.Append(this->GetVal(theChild));

	fOutput = theSeq;
	}

void Visitor_GetVal::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	ZMap_CF theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		theMap.Set(theName, this->GetVal(theChild));

	fOutput = theMap;
	}

ZRef<CFTypeRef> Visitor_GetVal::GetVal(ZRef<ZYadR> iYadR)
	{
	ZRef<CFTypeRef> result;
	if (iYadR)
		{
		iYadR->Accept(*this);
		std::swap(result, fOutput);
		}
	return result;
	}

ZRef<CFTypeRef> sFromYadR(CFTypeRef iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadR_CF> theYadR = iYadR.DynamicCast<ZYadR_CF>())
		return theYadR->GetVal();

	return Visitor_GetVal(iDefault).GetVal(iYadR);
	}

ZRef<CFTypeRef> sFromYadR(const ZRef<CFTypeRef>& iDefault, ZRef<ZYadR> iYadR)
	{ return sFromYadR(iDefault.Get(), iYadR); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
