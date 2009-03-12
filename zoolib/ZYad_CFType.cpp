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

#include "zoolib/ZFactoryChain.h"
#include "zoolib/ZStream_CFData.h"
#include "zoolib/ZUtil_CFType.h"

#include <CoreFoundation/CFString.h>

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::string;
using std::vector;

ZOOLIB_FACTORYCHAIN_HEAD(CFTypeRef, ZRef<ZYadR>);

// =================================================================================================
#pragma mark -
#pragma mark * Factory

namespace ZANONYMOUS {

class Maker0
:	public ZFactoryChain_T<CFTypeRef, ZRef<ZYadR> >
	{
public:
	Maker0()
	:	ZFactoryChain_T<Result_t, Param_t>(true)
		{}

	virtual bool Make(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iParam))
			{
			oResult = theYadR->GetCFTypeRef();
			::CFRetain(oResult);
			return true;
			}
		return false;
		}	
	} sMaker0;

class Maker1
:	public ZFactoryChain_T<CFTypeRef, ZRef<ZYadR> >
	{
public:
	Maker1()
	:	ZFactoryChain_T<Result_t, Param_t>(false)
		{}

	virtual bool Make(Result_t& oResult, Param_t iParam)
		{
		ZTValue theTValue;
		if (ZFactoryChain_T<ZTValue, ZRef<ZYadR> >::sMake(theTValue, iParam))
			{
			oResult = ZUtil_CFType::sCreateCFType(theTValue);
			return true;
			}
		return false;
		}	
	} sMaker1;

class Maker2
:	public ZFactoryChain_T<ZTValue, ZRef<ZYadR> >
	{
public:
	Maker2()
	:	ZFactoryChain_T<Result_t, Param_t>(false)
		{}

	virtual bool Make(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<ZYadR_CFType> theYadR = ZRefDynamicCast<ZYadR_CFType>(iParam))
			{
			oResult = ZUtil_CFType::sAsTV(theYadR->GetCFTypeRef());
			return true;
			}
		return false;
		}	
	} sMaker2;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_CFType

ZYadR_CFType::ZYadR_CFType(CFTypeRef iCFTypeRef)
:	fCFTypeRef(iCFTypeRef)
	{}

ZYadR_CFType::~ZYadR_CFType()
	{}

CFTypeRef ZYadR_CFType::GetCFTypeRef()
	{ return fCFTypeRef; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_CFType

ZYadPrimR_CFType::ZYadPrimR_CFType(CFTypeRef iCFTypeRef)
:	ZYadR_CFType(iCFTypeRef)
	{}

ZYadPrimR_CFType::~ZYadPrimR_CFType()
	{}

bool ZYadPrimR_CFType::IsSimple(const ZYadOptions& iOptions)
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawRPos_MemoryBlock

ZYadRawRPos_CFData::ZYadRawRPos_CFData(CFDataRef iCFDataRef)
:	ZYadR_CFType(iCFDataRef),
	ZStreamerRPos_CFData(iCFDataRef)
	{}

ZYadRawRPos_CFData::~ZYadRawRPos_CFData()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_CFArray

ZYadListRPos_CFArray::ZYadListRPos_CFArray(CFArrayRef iCFArrayRef)
:	ZYadR_CFType(iCFArrayRef),
	fCFArrayRef(iCFArrayRef),
	fPosition(0)
	{}

ZYadListRPos_CFArray::ZYadListRPos_CFArray(CFArrayRef iCFArrayRef, uint64 iPosition)
:	ZYadR_CFType(iCFArrayRef),
	fCFArrayRef(iCFArrayRef),
	fPosition(iPosition)
	{}

ZRef<ZYadR> ZYadListRPos_CFArray::ReadInc()
	{
	CFIndex theSize = ::CFArrayGetCount(fCFArrayRef);
	if (fPosition < theSize)
		{
		CFTypeRef theValue = ::CFArrayGetValueAtIndex(fCFArrayRef, fPosition++);
		return ZYad_CFType::sMakeYadR(theValue);
		}
	return ZRef<ZYadR>();
	}

uint64 ZYadListRPos_CFArray::GetPosition()
	{ return fPosition; }

uint64 ZYadListRPos_CFArray::GetSize()
	{ return ::CFArrayGetCount(fCFArrayRef); }

void ZYadListRPos_CFArray::SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

ZRef<ZYadListRPos> ZYadListRPos_CFArray::Clone()
	{ return new ZYadListRPos_CFArray(fCFArrayRef, fPosition); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CFDictionary

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

ZYadMapRPos_CFDictionary::ZYadMapRPos_CFDictionary(CFDictionaryRef iCFDictionaryRef,
	uint64 iPosition,
	const std::vector<CFStringRef>& iNames,
	const std::vector<CFTypeRef>& iValues)
:	ZYadR_CFType(iCFDictionaryRef),
	fCFDictionaryRef(iCFDictionaryRef),
	fPosition(iPosition),
	fNames(iNames),
	fValues(iValues)
	{}

ZYadMapRPos_CFDictionary::ZYadMapRPos_CFDictionary(CFDictionaryRef iCFDictionaryRef)
:	ZYadR_CFType(iCFDictionaryRef),
	fCFDictionaryRef(iCFDictionaryRef),
	fPosition(0)
	{
	GatherContents_t theParam;
	theParam.fNames = &fNames;
	theParam.fValues = &fValues;
	::CFDictionaryApplyFunction(fCFDictionaryRef, sGatherContents, &theParam);
	}

ZRef<ZYadR> ZYadMapRPos_CFDictionary::ReadInc(string& oName)
	{
	if (fPosition < fNames.size())	
		{
		oName = ZUtil_CFType::sAsUTF8(fNames.at(fPosition));
		return ZYad_CFType::sMakeYadR(fValues[fPosition++]);
		}
	return ZRef<ZYadR>();
	}

void ZYadMapRPos_CFDictionary::SetPosition(const std::string& iName)
	{
	for (fPosition = 0; fPosition < fNames.size(); ++fPosition)
		{
		if (iName == ZUtil_CFType::sAsUTF8(fNames.at(fPosition)))
			break;
		}
	}

ZRef<ZYadMapRPos> ZYadMapRPos_CFDictionary::Clone()
	{ return new ZYadMapRPos_CFDictionary(fCFDictionaryRef, fPosition, fNames, fValues); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_CFType

ZRef<ZYadR> ZYad_CFType::sMakeYadR(CFTypeRef iCFTypeRef)
	{
	CFTypeID theTypeID = ::CFGetTypeID(iCFTypeRef);

	if (theTypeID == ::CFDictionaryGetTypeID())
		return new ZYadMapRPos_CFDictionary(static_cast<CFDictionaryRef>(iCFTypeRef));

	if (theTypeID == ::CFArrayGetTypeID())
		return new ZYadListRPos_CFArray(static_cast<CFArrayRef>(iCFTypeRef));

	if (theTypeID == ::CFDataGetTypeID())
		return new ZYadRawRPos_CFData(static_cast<CFDataRef>(iCFTypeRef));

	return new ZYadPrimR_CFType(iCFTypeRef);
	}


static ZRef<CFDictionaryRef> sReadDictionary(ZRef<ZYadMapR> iYadMapR)
	{
	ZRef<CFMutableDictionaryRef> result = NoRetain(::CFDictionaryCreateMutable(
		kCFAllocatorDefault, 0,
		&kCFCopyStringDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks));

	string theName;
	while (ZRef<ZYadR> theYadR = iYadMapR->ReadInc(theName))
		{
		ZRef<CFStringRef> theStringRef
			= NoRetain(ZUtil_CFType::sCreateCFString_UTF8(theName));
		ZRef<CFTypeRef> theValue = NoRetain(ZYad_CFType::sFromYadR(theYadR));
		::CFDictionarySetValue(result, theStringRef, theValue);
		}

	return result;
	}

static CFArrayRef sReadArray(ZRef<ZYadListR> iYadListR)
	{
	CFMutableArrayRef result = ::CFArrayCreateMutable(
		kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

	while (ZRef<ZYadR> theYadR = iYadListR->ReadInc())
		{
		ZRef<CFTypeRef> theValue = NoRetain(ZYad_CFType::sFromYadR(theYadR));
		::CFArrayAppendValue(result, theValue);
		}

	return result;
	}

static CFDataRef sReadData(const ZStreamR& iStreamR)
	{
	size_t expectedSize = 0;
	if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		expectedSize = theStreamRPos->GetSize();

	CFMutableDataRef result = ::CFDataCreateMutable(kCFAllocatorDefault, expectedSize);
	ZStreamRWPos_CFData w(result);
	w.CopyAllFrom(iStreamR);
	w.Truncate();
	return result;
	}

CFTypeRef ZYad_CFType::sFromYadR(ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return nil;
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		return sReadDictionary(theYadMapR);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		return sReadArray(theYadListR);
		}
	else if (ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR))
		{
		return sReadData(theYadRawR->GetStreamR());
		}
	else
		{
		return ZFactoryChain_T<CFTypeRef, ZRef<ZYadR> >::sMake(iYadR);
		}
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
