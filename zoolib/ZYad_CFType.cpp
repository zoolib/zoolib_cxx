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

#include "zoolib/ZYad_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_CFString.h"
#include "zoolib/ZUtil_CFType.h"
#include "zoolib/ZVal_CFType.h"

#include <CoreFoundation/CFString.h>

namespace ZooLib {

using std::min;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadAtomR_CFType

ZYadAtomR_CFType::ZYadAtomR_CFType(CFTypeRef iVal)
:	ZYadR_CFType(iVal)
	{}

ZAny ZYadAtomR_CFType::AsAny()
	{ return ZUtil_CFType::sAsAny(this->GetVal()); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_CFType

ZYadStreamRPos_CFType::ZYadStreamRPos_CFType(CFDataRef iDataRef)
:	ZYadR_CFType(iDataRef)
,	ZStreamerRPos_CFType(ZRef<CFDataRef>(iDataRef))
	{}

ZYadStreamRPos_CFType::~ZYadStreamRPos_CFType()
	{}

bool ZYadStreamRPos_CFType::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_CFType

ZYadStrimR_CFType::ZYadStrimR_CFType(CFStringRef iStringRef)
:	ZYadR_CFType(iStringRef)
,	ZStrimmerR_CFString(iStringRef)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_CFType

ZYadSeqRPos_CFType::ZYadSeqRPos_CFType(CFArrayRef iArray)
:	ZYadR_CFType(iArray)
,	YadSeqBase_t(iArray)
	{}

ZYadSeqRPos_CFType::ZYadSeqRPos_CFType(CFArrayRef iArray, uint64 iPosition)
:	ZYadR_CFType(iArray)
,	YadSeqBase_t(iArray, iPosition)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CFType

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

ZYadMapRPos_CFType::ZYadMapRPos_CFType(CFDictionaryRef iDictionary,
	uint64 iPosition,
	const std::vector<CFStringRef>& iNames,
	const std::vector<CFTypeRef>& iValues)
:	ZYadR_CFType(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(iPosition)
,	fNames(iNames)
,	fValues(iValues)
	{}

ZYadMapRPos_CFType::ZYadMapRPos_CFType(CFDictionaryRef iDictionary)
:	ZYadR_CFType(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(0)
	{
	GatherContents_t theParam;
	theParam.fNames = &fNames;
	theParam.fValues = &fValues;
	::CFDictionaryApplyFunction(fDictionary, spGatherContents, &theParam);
	}

ZRef<ZYadR> ZYadMapRPos_CFType::ReadInc(string& oName)
	{
	if (fPosition < fNames.size())
		{
		oName = ZUtil_CFType::sAsUTF8(fNames.at(fPosition));
		return sMakeYadR(fValues[fPosition++]);
		}
	return null;
	}

ZRef<ZYadMapRPos> ZYadMapRPos_CFType::Clone()
	{ return new ZYadMapRPos_CFType(fDictionary, fPosition, fNames, fValues); }

void ZYadMapRPos_CFType::SetPosition(const std::string& iName)
	{
	for (fPosition = 0; fPosition < fNames.size(); ++fPosition)
		{
		if (iName == ZUtil_CFType::sAsUTF8(fNames.at(fPosition)))
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(CFTypeRef iVal)
	{
	const ZVal_CFType theVal = iVal;

	if (ZQ<ZMap_CFType> theQ = theVal.QGet<ZMap_CFType>())
		return new ZYadMapRPos_CFType(theQ.Get());

	if (ZQ<ZSeq_CFType> theQ = theVal.QGet<ZSeq_CFType>())
		return new ZYadSeqRPos_CFType(theQ.Get());

	if (ZQ<ZData_CFType> theQ = theVal.QGet<ZData_CFType>())
		return new ZYadStreamRPos_CFType(theQ.Get());

	if (ZQ<ZRef<CFStringRef> > theQ = theVal.QGet<ZRef<CFStringRef> >())
		return sMakeYadR(theQ.Get().Get());

	return new ZYadAtomR_CFType(iVal);
	}

ZRef<ZYadStrimR> sMakeYadR(CFMutableStringRef iString)
	{ return new ZYadStrimR_CFType(iString); }

ZRef<ZYadStrimR> sMakeYadR(CFStringRef iString)
	{ return new ZYadStrimR_CFType(iString); }

ZRef<ZYadStreamR> sMakeYadR(CFMutableDataRef iData)
	{ return new ZYadStreamRPos_CFType(iData); }

ZRef<ZYadStreamR> sMakeYadR(CFDataRef iData)
	{ return new ZYadStreamRPos_CFType(iData); }

ZRef<ZYadSeqRPos> sMakeYadR(CFMutableArrayRef iArray)
	{ return new ZYadSeqRPos_CFType(iArray); }

ZRef<ZYadSeqRPos> sMakeYadR(CFArrayRef iArray)
	{ return new ZYadSeqRPos_CFType(iArray); }

ZRef<ZYadMapRPos> sMakeYadR(CFMutableDictionaryRef iDictionary)
	{ return new ZYadMapRPos_CFType(iDictionary); }

ZRef<ZYadMapRPos> sMakeYadR(CFDictionaryRef iDictionary)
	{ return new ZYadMapRPos_CFType(iDictionary); }

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

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
	{ fOutput = ZUtil_CFType::sDAsCFType(fDefault, iYadAtomR->AsAny()); }

void Visitor_GetVal::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{ fOutput = sReadAll_T<ZData_CFType>(iYadStreamR->GetStreamR()); }

void Visitor_GetVal::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{
	ZRef<CFMutableStringRef> result = ZUtil_CFType::sStringMutable();
	ZStrimW_CFString(result).CopyAllFrom(iYadStrimR->GetStrimR());
	fOutput = result;
	}

void Visitor_GetVal::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{
	ZSeq_CFType theSeq;

	while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
		theSeq.Append(this->GetVal(theChild));

	fOutput = theSeq;
	}

void Visitor_GetVal::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	ZMap_CFType theMap;

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

ZRef<CFTypeRef> sFromYadR(const ZRef<CFTypeRef>& iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iYadR))
		return theYadR->GetVal();

	return Visitor_GetVal(iDefault).GetVal(iYadR);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
