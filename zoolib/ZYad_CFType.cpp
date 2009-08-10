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

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_CFType

ZYadPrimR_CFType::ZYadPrimR_CFType(const ZRef<CFTypeRef>& iVal)
:	ZYadR_CFType(iVal)
	{}

ZAny ZYadPrimR_CFType::AsAny()
	{ return ZUtil_CFType::sAsVal_Any(this->GetVal()).AsAny(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_CFType

ZYadStreamRPos_CFType::ZYadStreamRPos_CFType(const ZRef<CFDataRef>& iDataRef)
:	ZYadR_CFType(iDataRef),
	ZStreamerRPos_CFType(iDataRef)
	{}

ZYadStreamRPos_CFType::~ZYadStreamRPos_CFType()
	{}

bool ZYadStreamRPos_CFType::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_CFType

ZYadStrimR_CFType::ZYadStrimR_CFType(ZRef<CFStringRef> iStringRef)
:	ZYadR_CFType(iStringRef),
	ZStrimmerR_CFString(iStringRef)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_CFType

ZYadListRPos_CFType::ZYadListRPos_CFType(const ZRef<CFArrayRef>& iArray)
:	ZYadR_CFType(iArray)
,	YadListBase_t(iArray)
	{}

ZYadListRPos_CFType::ZYadListRPos_CFType(const ZRef<CFArrayRef>& iArray, uint64 iPosition)
:	ZYadR_CFType(iArray)
,	YadListBase_t(iArray, iPosition)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CFType

namespace ZANONYMOUS {

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

ZYadMapRPos_CFType::ZYadMapRPos_CFType(const ZRef<CFDictionaryRef>& iDictionary,
	uint64 iPosition,
	const std::vector<CFStringRef>& iNames,
	const std::vector<CFTypeRef>& iValues)
:	ZYadR_CFType(iDictionary),
	fDictionary(iDictionary),
	fPosition(iPosition),
	fNames(iNames),
	fValues(iValues)
	{}

ZYadMapRPos_CFType::ZYadMapRPos_CFType(const ZRef<CFDictionaryRef>& iDictionary)
:	ZYadR_CFType(iDictionary),
	fDictionary(iDictionary),
	fPosition(0)
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
	return ZRef<ZYadR>();
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

ZRef<ZYadR> sMakeYadR(const ZRef<CFTypeRef>& iVal)
	{
	const ZVal_CFType theVal = iVal;

	ZMap_CFType asMap;
	if (theVal.QGetMap(asMap))
		return new ZYadMapRPos_CFType(asMap);
		
	ZList_CFType asList;
	if (theVal.QGetList(asList))
		return new ZYadListRPos_CFType(asList);
		
	ZData_CFType asData;
	if (theVal.QGetData(asData))
		return new ZYadStreamRPos_CFType(asData);

	ZRef<CFStringRef> asCFString;
	if (theVal.QGetCFString(asCFString))
		return new ZYadStrimR_CFType(asCFString);

	return new ZYadPrimR_CFType(iVal);
	}

ZRef<ZYadStrimR> sMakeYadR(const ZRef<CFMutableStringRef>& iString)
	{ return new ZYadStrimR_CFType(iString); }

ZRef<ZYadStrimR> sMakeYadR(const ZRef<CFStringRef>& iString)
	{ return new ZYadStrimR_CFType(iString); }

ZRef<ZYadStreamR> sMakeYadR(const ZRef<CFMutableDataRef>& iData)
	{ return new ZYadStreamRPos_CFType(iData); }

ZRef<ZYadStreamR> sMakeYadR(const ZRef<CFDataRef>& iData)
	{ return new ZYadStreamRPos_CFType(iData); }

ZRef<ZYadListR> sMakeYadR(const ZRef<CFMutableArrayRef>& iList)
	{ return new ZYadListRPos_CFType(iList); }

ZRef<ZYadListR> sMakeYadR(const ZRef<CFArrayRef>& iList)
	{ return new ZYadListRPos_CFType(iList); }

ZRef<ZYadMapR> sMakeYadR(const ZRef<CFMutableDictionaryRef>& iMap)
	{ return new ZYadMapRPos_CFType(iMap); }

ZRef<ZYadMapR> sMakeYadR(const ZRef<CFDictionaryRef>& iMap)
	{ return new ZYadMapRPos_CFType(iMap); }

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

namespace ZANONYMOUS {

class YadVisitor_GetValCFType : public ZYadVisitor
	{
public:
	YadVisitor_GetValCFType(ZRef<CFTypeRef> iDefault);

// From ZYadVisitor
	virtual bool Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR);
	virtual bool Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual bool Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual bool Visit_YadListR(ZRef<ZYadListR> iYadListR);
	virtual bool Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);

	ZRef<CFTypeRef> fDefault;
	ZRef<CFTypeRef> fOutput;
	};

} // anonymous namespace

ZRef<CFTypeRef> sFromYadR(ZRef<ZYadR> iYadR, const ZRef<CFTypeRef>& iDefault)
	{
	if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iYadR))
		return theYadR->GetVal();

	YadVisitor_GetValCFType theVisitor(iDefault);
	iYadR->Accept(theVisitor);
	return theVisitor.fOutput;
	}

YadVisitor_GetValCFType::YadVisitor_GetValCFType(ZRef<CFTypeRef> iDefault)
:	fDefault(iDefault)
	{}

bool YadVisitor_GetValCFType::Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR)
	{
	fOutput = ZUtil_CFType::sAsCFType(iYadPrimR->AsAny(), fDefault);
	return true;
	}

bool YadVisitor_GetValCFType::Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR)
	{
	fOutput = sReadAll_T<ZData_CFType>(iYadStreamR->GetStreamR());
	return true;
	}

bool YadVisitor_GetValCFType::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{
	ZRef<CFMutableStringRef> result = ZUtil_CFType::sStringMutable();
	ZStrimW_CFString(result).CopyAllFrom(iYadStrimR->GetStrimR());
	fOutput = result;
	return true;
	}

bool YadVisitor_GetValCFType::Visit_YadListR(ZRef<ZYadListR> iYadListR)
	{
	ZList_CFType theList;

	while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
		theList.Append(sFromYadR(theChild, fDefault));

	fOutput = theList;
	return true;
	}

bool YadVisitor_GetValCFType::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
	{
	ZMap_CFType theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		theMap.Set(theName, sFromYadR(theChild, fDefault));

	fOutput = theMap;
	return true;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
