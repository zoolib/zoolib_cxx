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
#include "zoolib/ZStream_CFData.h"
#include "zoolib/ZUtil_CFType.h"

#include <CoreFoundation/CFString.h>

using std::min;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_CFType

ZYad_CFType::ZYad_CFType(CFTypeRef iCFTypeRef)
:	fCFTypeRef(iCFTypeRef)
	{
	::CFRetain(fCFTypeRef);
	}

ZYad_CFType::~ZYad_CFType()
	{ ::CFRelease(fCFTypeRef); }

bool ZYad_CFType::GetTValue(ZTValue& oTV)
	{
	oTV = ZUtil_CFType::sAsTV(fCFTypeRef);
	return true;
	}

CFTypeRef ZYad_CFType::GetCFTypeRef()
	{ return fCFTypeRef; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_TValue

ZYadPrimR_CFType::ZYadPrimR_CFType(CFTypeRef iCFTypeRef)
:	fCFTypeRef(iCFTypeRef)
	{
	::CFRetain(fCFTypeRef);
	}

ZYadPrimR_CFType::~ZYadPrimR_CFType()
	{ ::CFRelease(fCFTypeRef); }

ZRef<ZYad> ZYadPrimR_CFType::ReadYad()
	{ return new ZYad_CFType(fCFTypeRef); }

bool ZYadPrimR_CFType::IsSimple(const ZYadOptions& iOptions)
	{
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawRPos_MemoryBlock

ZYadRawRPos_CFData::ZYadRawRPos_CFData(CFDataRef iCFDataRef)
:	ZStreamerRPos_CFData(iCFDataRef),
	fCFDataRef(iCFDataRef)
	{
	::CFRetain(fCFDataRef);
	}

ZYadRawRPos_CFData::~ZYadRawRPos_CFData()
	{ ::CFRelease(fCFDataRef); }

ZRef<ZYad> ZYadRawRPos_CFData::ReadYad()
	{ return new ZYad_CFType(fCFDataRef); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_CFArray

ZYadListRPos_CFArray::ZYadListRPos_CFArray(CFArrayRef iCFArrayRef)
:	fCFArrayRef(iCFArrayRef),
	fPosition(0)
	{
	::CFRetain(fCFArrayRef);
	}

ZYadListRPos_CFArray::~ZYadListRPos_CFArray()
	{ ::CFRelease(fCFArrayRef); }

bool ZYadListRPos_CFArray::HasChild()
	{ return fPosition < ::CFArrayGetCount(fCFArrayRef); }

ZRef<ZYad> ZYadListRPos_CFArray::ReadYad()
	{
	CFIndex theSize = ::CFArrayGetCount(fCFArrayRef);
	if (fPosition < theSize)
		{
		CFTypeRef theValue = ::CFArrayGetValueAtIndex(fCFArrayRef, fPosition++);
		return new ZYad_CFType(theValue);
		}

	return ZRef<ZYad>();
	}

ZRef<ZYadR> ZYadListRPos_CFArray::NextChild()
	{
	CFIndex theSize = ::CFArrayGetCount(fCFArrayRef);
	if (fPosition < theSize)
		{
		CFTypeRef theValue = ::CFArrayGetValueAtIndex(fCFArrayRef, fPosition++);
		return ZYadUtil_CFType::sMakeYadR(theValue);
		}

	return ZRef<ZYadR>();
	}

size_t ZYadListRPos_CFArray::GetPosition()
	{ return fPosition; }

size_t ZYadListRPos_CFArray::GetSize()
	{ return ::CFArrayGetCount(fCFArrayRef); }

void ZYadListRPos_CFArray::SetPosition(size_t iPosition)
	{ fPosition = min(iPosition, size_t(::CFArrayGetCount(fCFArrayRef))); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos_CFDictionary

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

ZYadListMapRPos_CFDictionary::ZYadListMapRPos_CFDictionary(CFDictionaryRef iCFDictionaryRef)
:	fCFDictionaryRef(iCFDictionaryRef),
	fPosition(0)
	{
	::CFRetain(fCFDictionaryRef);
	GatherContents_t theParam;
	theParam.fNames = &fNames;
	theParam.fValues = &fValues;
	::CFDictionaryApplyFunction(fCFDictionaryRef, sGatherContents, &theParam);
	}

ZYadListMapRPos_CFDictionary::~ZYadListMapRPos_CFDictionary()
	{
	::CFRelease(fCFDictionaryRef);	
	}

bool ZYadListMapRPos_CFDictionary::HasChild()
	{ return fPosition < fNames.size(); }

ZRef<ZYad> ZYadListMapRPos_CFDictionary::ReadYad()
	{
	if (fPosition < fNames.size())
		return new ZYad_CFType(fValues[fPosition++]);
	return ZRef<ZYad>();
	}

ZRef<ZYadR> ZYadListMapRPos_CFDictionary::NextChild()
	{
	if (fPosition < fNames.size())
		return ZYadUtil_CFType::sMakeYadR(fValues[fPosition++]);
	return ZRef<ZYadR>();
	}

size_t ZYadListMapRPos_CFDictionary::GetPosition()
	{ return fPosition; }

size_t ZYadListMapRPos_CFDictionary::GetSize()
	{ return fNames.size(); }

void ZYadListMapRPos_CFDictionary::SetPosition(size_t iPosition)
	{ fPosition = min(iPosition, fNames.size()); }

std::string ZYadListMapRPos_CFDictionary::Name()
	{
	if (fPosition < fNames.size())
		return ZUtil_CFType::sAsUTF8(fNames.at(fPosition));
	return string();
	}

void ZYadListMapRPos_CFDictionary::SetPosition(const std::string& iName)
	{
	#warning NDY
	ZUnimplemented();
//	fPosition = find(
//	fIter = fTuple.IteratorOf(iName);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_CFType

ZRef<ZYadR> ZYadUtil_CFType::sMakeYadR(CFTypeRef iCFTypeRef)
	{
	CFTypeID theTypeID = ::CFGetTypeID(iCFTypeRef);

	if (theTypeID == ::CFDictionaryGetTypeID())
		return new ZYadListMapRPos_CFDictionary(static_cast<CFDictionaryRef>(iCFTypeRef));

	if (theTypeID == ::CFArrayGetTypeID())
		return new ZYadListRPos_CFArray(static_cast<CFArrayRef>(iCFTypeRef));

	if (theTypeID == ::CFDataGetTypeID())
		return new ZYadRawRPos_CFData(static_cast<CFDataRef>(iCFTypeRef));

	return new ZYadPrimR_CFType(iCFTypeRef);
	}


static CFDictionaryRef sReadDictionary(ZRef<ZYadMapR> iYadMapR)
	{
	CFMutableDictionaryRef result = ::CFDictionaryCreateMutable(
		kCFAllocatorDefault, 0,
		&kCFCopyStringDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

	while (iYadMapR->HasChild())
		{
		CFStringRef theStringRef = ZUtil_CFType::sCreateCFString_UTF8(iYadMapR->Name());
		if (CFTypeRef theValue = ZYadUtil_CFType::sFromYadR(iYadMapR->NextChild()))
			{
			::CFDictionarySetValue(result, theStringRef, theValue);
			::CFRelease(theValue);
			}
		::CFRelease(theStringRef);
		}

	return result;
	}

static CFArrayRef sReadArray(ZRef<ZYadListR> iYadListR)
	{
	CFMutableArrayRef result = ::CFArrayCreateMutable(
		kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

	while (iYadListR->HasChild())
		{
		CFTypeRef theValue = ZYadUtil_CFType::sFromYadR(iYadListR->NextChild());
		::CFArrayAppendValue(result, theValue);
		::CFRelease(theValue);
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

CFTypeRef ZYadUtil_CFType::sFromYadR(ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		return nil;

	if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
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
		ZRef<ZYad> theYad = iYadR->ReadYad();
		if (ZRef<ZYad_CFType> theYad_CFType = ZRefDynamicCast<ZYad_CFType>(theYad))
			{
			CFTypeRef result = theYad_CFType->GetCFTypeRef();
			::CFRetain(result);
			return result;
			}
		else
			{
			return ZUtil_CFType::sCreateCFType(theYad->GetTValue());
			}
		}
	}
