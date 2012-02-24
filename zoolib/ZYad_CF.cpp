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
// MARK: - ZYadStreamerRPos_CF

ZYadStreamerRPos_CF::ZYadStreamerRPos_CF(CFDataRef iDataRef)
:	ZYadR_CF(iDataRef)
,	ZStreamerRPos_CF(iDataRef)
	{}

ZYadStreamerRPos_CF::~ZYadStreamerRPos_CF()
	{}

bool ZYadStreamerRPos_CF::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
// MARK: - ZYadStrimmerR_CF

ZYadStrimmerR_CF::ZYadStrimmerR_CF(CFStringRef iStringRef)
:	ZYadR_CF(iStringRef)
,	ZStrimmerR_CFString(iStringRef)
	{}

// =================================================================================================
// MARK: - ZYadSeqAtRPos_CF

ZYadSeqAtRPos_CF::ZYadSeqAtRPos_CF(CFArrayRef iArray)
:	ZYadR_CF(iArray)
,	YadSeqBase_t(iArray)
	{}

ZYadSeqAtRPos_CF::ZYadSeqAtRPos_CF(CFArrayRef iArray, uint64 iPosition)
:	ZYadR_CF(iArray)
,	YadSeqBase_t(iArray, iPosition)
	{}

// =================================================================================================
// MARK: - ZYadMapAtRPos_CF

namespace { // anonymous

struct GatherContents_t
	{
	vector<CFStringRef>* fNames;
	vector<CFTypeRef>* fValues;
	};

void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	GatherContents_t* theParam = static_cast<GatherContents_t*>(iRefcon);
	theParam->fNames->push_back(static_cast<CFStringRef>(iKey));
	theParam->fValues->push_back(static_cast<CFTypeRef>(iValue));
	}

} // anonymous namespace

ZYadMapAtRPos_CF::ZYadMapAtRPos_CF(CFDictionaryRef iDictionary,
	uint64 iPosition,
	const std::vector<CFStringRef>& iNames,
	const std::vector<CFTypeRef>& iValues)
:	ZYadR_CF(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(iPosition)
,	fNames(iNames)
,	fValues(iValues)
	{}

ZYadMapAtRPos_CF::ZYadMapAtRPos_CF(CFDictionaryRef iDictionary)
:	ZYadR_CF(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(-1)
	{}

ZRef<ZYadR> ZYadMapAtRPos_CF::ReadInc(string& oName)
	{
	this->pSetupPosition();
	if (fPosition < fNames.size())
		{
		oName = ZUtil_CF::sAsUTF8(fNames.at(fPosition));
		return sYadR(fValues[fPosition++]);
		}
	return null;
	}

ZRef<ZYadMapRClone> ZYadMapAtRPos_CF::Clone()
	{ return new ZYadMapAtRPos_CF(fDictionary, fPosition, fNames, fValues); }

void ZYadMapAtRPos_CF::SetPosition(const std::string& iName)
	{
	this->pSetupPosition();
	for (fPosition = 0; fPosition < fNames.size(); ++fPosition)
		{
		if (iName == ZUtil_CF::sAsUTF8(fNames.at(fPosition)))
			break;
		}
	}

ZRef<ZYadR> ZYadMapAtRPos_CF::ReadAt(const std::string& iName)
	{
	CFTypeRef theVal;
	if (::CFDictionaryGetValueIfPresent(fDictionary, ZUtil_CF::sString(iName), &theVal))
		return sYadR(theVal);
	return null;
	}

void ZYadMapAtRPos_CF::pSetupPosition()
	{
	if (fPosition != uint64(-1))
		return;

	fPosition = 0;

	GatherContents_t theParam;
	theParam.fNames = &fNames;
	theParam.fValues = &fValues;
	::CFDictionaryApplyFunction(fDictionary, spGatherContents, &theParam);
	}

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(CFTypeRef iVal)
	{
	const ZVal_CF theVal = iVal;

	if (ZQ<ZMap_CF> theQ = theVal.QGet<ZMap_CF>())
		return new ZYadMapAtRPos_CF(*theQ);

	if (ZQ<ZSeq_CF> theQ = theVal.QGet<ZSeq_CF>())
		return new ZYadSeqAtRPos_CF(*theQ);

	if (ZQ<ZData_CF> theQ = theVal.QGet<ZData_CF>())
		return new ZYadStreamerRPos_CF(*theQ);

	if (ZQ<ZRef<CFStringRef> > theQ = theVal.QGet<ZRef<CFStringRef> >())
		return sYadR(*theQ);

	return new ZYadAtomR_CF(iVal);
	}

ZRef<ZYadR> sYadR(const ZRef<CFTypeRef>& iVal)
	{ return sYadR(iVal.Get()); }

ZRef<ZYadStrimmerR> sYadR(CFMutableStringRef iString)
	{ return new ZYadStrimmerR_CF(iString); }

ZRef<ZYadStrimmerR> sYadR(CFStringRef iString)
	{ return new ZYadStrimmerR_CF(iString); }

ZRef<ZYadStrimmerR> sYadR(const ZRef<CFStringRef>& iString)
	{ return sYadR(iString.Get()); }

ZRef<ZYadStreamerR> sYadR(CFMutableDataRef iData)
	{ return new ZYadStreamerRPos_CF(iData); }

ZRef<ZYadStreamerR> sYadR(CFDataRef iData)
	{ return new ZYadStreamerRPos_CF(iData); }

ZRef<ZYadStreamerR> sYadR(const ZRef<CFDataRef>& iData)
	{ return sYadR(iData.Get()); }

ZRef<ZYadSeqAtRPos> sYadR(CFMutableArrayRef iArray)
	{ return new ZYadSeqAtRPos_CF(iArray); }

ZRef<ZYadSeqAtRPos> sYadR(CFArrayRef iArray)
	{ return new ZYadSeqAtRPos_CF(iArray); }

ZRef<ZYadSeqAtRPos> sYadR(const ZRef<CFArrayRef>& iArray)
	{ return sYadR(iArray.Get()); }

ZRef<ZYadMapAtRPos> sYadR(CFMutableDictionaryRef iDictionary)
	{ return new ZYadMapAtRPos_CF(iDictionary); }

ZRef<ZYadMapAtRPos> sYadR(CFDictionaryRef iDictionary)
	{ return new ZYadMapAtRPos_CF(iDictionary); }

ZRef<ZYadMapAtRPos> sYadR(const ZRef<CFDictionaryRef>& iDictionary)
	{ return sYadR(iDictionary.Get()); }

// =================================================================================================
// MARK: - sFromYadR

namespace { // anonymous

class Visitor_GetVal : public ZVisitor_Yad_PreferAt
	{
public:
	Visitor_GetVal(ZRef<CFTypeRef> iDefault);

// From ZVisitor_Yad
	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR);
	virtual void Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR);
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

void Visitor_GetVal::Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR)
	{ fOutput = sReadAll_T<ZData_CF>(iYadStreamerR->GetStreamR()); }

void Visitor_GetVal::Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR)
	{
	ZRef<CFMutableStringRef> result = ZUtil_CF::sStringMutable();
	ZStrimW_CFString(result).CopyAllFrom(iYadStrimmerR->GetStrimR());
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
