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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZStream_ValData_T.h"
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
#pragma mark * Factory

namespace ZANONYMOUS {

// ZRef<CFTypeRef> <-- ZRef<ZYadR>
class Maker0
:	public ZFunctionChain_T<ZRef<CFTypeRef>, ZRef<ZYadR> >
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iParam))
			{
			oResult = theYadR->GetVal();
			return true;
			}
		return false;
		}	
	} sMaker0;

#warning NDY

#if 0
// ZRef<CFTypeRef> <-- ZRef<ZYadR>
class Maker1
:	public ZFunctionChain_T<ZRef<CFTypeRef>, ZRef<ZYadR> >
	{
public:
	Maker1() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		ZVal_ZooLib theVal;
		if (ZFunctionChain_T<ZVal_ZooLib, ZRef<ZYadR> >::sInvoke(theVal, iParam))
			{
			oResult = ZUtil_CFType::sType(theVal);
			return true;
			}
		return false;
		}	
	} sMaker1;

// ZVal_ZooLib <-- ZRef<ZYadR>
class Maker2
:	public ZFunctionChain_T<ZVal_ZooLib, ZRef<ZYadR> >
	{
public:
	Maker2() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iParam))
			{
			oResult = ZUtil_CFType::sAsTV(theYadR->GetCFTypeRef());
			return true;
			}
		return false;
		}	
	} sMaker2;

#endif

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_CFType

ZYadR_CFType::ZYadR_CFType(ZRef<CFTypeRef> iCFTypeRef)
:	fVal(iCFTypeRef)
	{}

ZYadR_CFType::~ZYadR_CFType()
	{}

const ZRef<CFTypeRef>& ZYadR_CFType::GetVal()
	{ return fVal; }

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
:	ZYadR_CFType(iArray),
	fList(iArray),
	fPosition(0)
	{}

ZYadListRPos_CFType::ZYadListRPos_CFType(const ZRef<CFArrayRef>& iArray, uint64 iPosition)
:	ZYadR_CFType(iArray),
	fList(iArray),
	fPosition(iPosition)
	{}

ZRef<ZYadR> ZYadListRPos_CFType::ReadInc()
	{
	if (fPosition < fList.Count())
		return ZYad_CFType::sMakeYadR(fList.Get(fPosition++));

	return ZRef<ZYadR>();
	}

uint64 ZYadListRPos_CFType::GetPosition()
	{ return fPosition; }

uint64 ZYadListRPos_CFType::GetSize()
	{ return fList.Count(); }

void ZYadListRPos_CFType::SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

ZRef<ZYadListRPos> ZYadListRPos_CFType::Clone()
	{ return new ZYadListRPos_CFType(fList, fPosition); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CFType

namespace ZANONYMOUS {

struct GatherContents_t
	{
	vector<CFStringRef>* fNames;
	vector<CFTypeRef>* fValues;
	};
	
static void sGatherContents(const void* iKey, const void* iValue, void* iRefcon)
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
	::CFDictionaryApplyFunction(fDictionary, sGatherContents, &theParam);
	}

ZRef<ZYadR> ZYadMapRPos_CFType::ReadInc(string& oName)
	{
	if (fPosition < fNames.size())	
		{
		oName = ZUtil_CFType::sAsUTF8(fNames.at(fPosition));
		return ZYad_CFType::sMakeYadR(fValues[fPosition++]);
		}
	return ZRef<ZYadR>();
	}

void ZYadMapRPos_CFType::SetPosition(const std::string& iName)
	{
	for (fPosition = 0; fPosition < fNames.size(); ++fPosition)
		{
		if (iName == ZUtil_CFType::sAsUTF8(fNames.at(fPosition)))
			break;
		}
	}

ZRef<ZYadMapRPos> ZYadMapRPos_CFType::Clone()
	{ return new ZYadMapRPos_CFType(fDictionary, fPosition, fNames, fValues); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_CFType

ZRef<ZYadR> ZYad_CFType::sMakeYadR(const ZRef<CFTypeRef>& iVal)
	{
	const ZVal_CFType theVal = iVal;

	ZValMap_CFType asMap;
	if (theVal.QGetMap(asMap))
		return new ZYadMapRPos_CFType(asMap);
		
	ZValList_CFType asList;
	if (theVal.QGetList(asList))
		return new ZYadListRPos_CFType(asList);
		
	ZValData_CFType asData;
	if (theVal.QGetData(asData))
		return new ZYadStreamRPos_CFType(asData);

	ZRef<CFStringRef> asCFString;
	if (theVal.QGetCFString(asCFString))
		return new ZYadStrimR_CFType(asCFString);

	return new ZYadR_CFType(iVal);
	}

ZRef<CFTypeRef> ZYad_CFType::sFromYadR(ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return ZRef<CFTypeRef>();
		}
	else if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iYadR))
		{
		return theYadR->GetVal();
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		ZValMap_CFType theMap;

		string theName;
		while (ZRef<ZYadR> theYadR = theYadMapR->ReadInc(theName))
			theMap.Set(theName, ZYad_CFType::sFromYadR(theYadR));

		return theMap;
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		ZValList_CFType theList;

		while (ZRef<ZYadR> theYadR = theYadListR->ReadInc())
			theList.Append(ZYad_CFType::sFromYadR(theYadR));

		return theList;
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		return sReadAll_T<ZValData_CFType>(theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		ZRef<CFMutableStringRef> result = ZUtil_CFType::sStringMutable();
		ZStrimW_CFString(result).CopyAllFrom(theYadStrimR->GetStrimR());
		return ZRef<CFTypeRef>(result);
		}
	else
		{
		return ZFunctionChain_T<ZRef<CFTypeRef>, ZRef<ZYadR> >::sInvoke(iYadR);
		}
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
