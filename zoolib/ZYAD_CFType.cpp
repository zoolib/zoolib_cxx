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

#include "zoolib/ZYAD_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZUtil_CFType.h"
#include "zoolib/ZStream_CFData.h"

#include <CoreFoundation/CFString.h>

using std::string;
using std::vector;


// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static bool sIsComplexString(const ZYADOptions& iOptions, const CFStringRef& iStringRef)
	{
	return false;
	}

static bool sIsComplex(const ZYADOptions& iOptions, CFTypeRef iCFTypeRef);

static bool sIsComplexArray(const ZYADOptions& iOptions, CFArrayRef iArrayRef)
	{
	size_t theSize = ::CFArrayGetCount(iArrayRef);
	if (theSize == 0)
		return false;

	if (theSize == 1)
		return sIsComplex(iOptions, ::CFArrayGetValueAtIndex(iArrayRef, 0));

	return true;	
	}

namespace ZANONYMOUS {

struct CheckForComplex_t
	{
	bool fIsComplex;
	const ZYADOptions* fOptions;
	};
	
static void sCheckForComplex(const void* iKey, const void* iValue, void* iRefcon)
	{
	CheckForComplex_t* theParam = static_cast<CheckForComplex_t*>(iRefcon);
	theParam->fIsComplex = sIsComplex(*theParam->fOptions, static_cast<CFTypeRef>(iValue));
	}

} // anonymous namespace

static bool sIsComplexDictionary(const ZYADOptions& iOptions, CFDictionaryRef iDictionaryRef)
	{
	size_t theSize = ::CFDictionaryGetCount(iDictionaryRef);
	if (theSize == 0)
		return false;

	if (theSize == 1)
		{
		CheckForComplex_t theParams;
		theParams.fOptions = &iOptions;
		::CFDictionaryApplyFunction(iDictionaryRef, sCheckForComplex, &theParams);
		return theParams.fIsComplex;
		}

	return true;
	}

static bool sIsComplex(const ZYADOptions& iOptions, CFTypeRef iCFTypeRef)
	{
	const CFTypeID theTypeID = ::CFGetTypeID(iCFTypeRef);

	if (theTypeID == ::CFDictionaryGetTypeID())
		return sIsComplexDictionary(iOptions, (CFDictionaryRef)iCFTypeRef);

	if (theTypeID == ::CFArrayGetTypeID())
		return sIsComplexArray(iOptions, (CFArrayRef)iCFTypeRef);
	
	if (theTypeID == ::CFStringGetTypeID())
		return sIsComplexString(iOptions, (CFStringRef)iCFTypeRef);

	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_CFType

ZYAD_CFType::ZYAD_CFType(CFTypeRef iCFTypeRef)
:	fCFTypeRef(iCFTypeRef)
	{
	::CFRetain(fCFTypeRef);
	}

ZYAD_CFType::~ZYAD_CFType()
	{
	::CFRelease(fCFTypeRef);
	}

bool ZYAD_CFType::GetTValue(ZTValue& oTV)
	{
	oTV = ZUtil_CFType::sAsTV(fCFTypeRef);
	return true;
	}

CFTypeRef ZYAD_CFType::GetCFTypeRef()
	{ return fCFTypeRef; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_CFType definition

ZYADReaderRep_CFType::ZYADReaderRep_CFType(CFTypeRef iCFTypeRef)
:	fCFTypeRef(iCFTypeRef),
	fHasValue(true)
	{
	::CFRetain(fCFTypeRef);
	}

ZYADReaderRep_CFType::~ZYADReaderRep_CFType()
	{
	::CFRelease(fCFTypeRef);
	}

bool ZYADReaderRep_CFType::HasValue()
	{ return fHasValue; }

ZType ZYADReaderRep_CFType::Type()
	{
	if (fHasValue)
		return ZUtil_CFType::sTypeOf(fCFTypeRef);
	return eZType_Null;
	}

ZRef<ZMapReaderRep> ZYADReaderRep_CFType::ReadMap()
	{
	if (fHasValue && ::CFGetTypeID(fCFTypeRef) == ::CFDictionaryGetTypeID())
		{
		fHasValue = false;
		return new ZMapReaderRep_CFType(static_cast<CFDictionaryRef>(fCFTypeRef));
		}

	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_CFType::ReadList()
	{
	if (fHasValue && ::CFGetTypeID(fCFTypeRef) == ::CFArrayGetTypeID())
		{
		fHasValue = false;
		return new ZListReaderRep_CFType(static_cast<CFArrayRef>(fCFTypeRef));
		}

	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_CFType::ReadRaw()
	{
	if (fHasValue && ::CFGetTypeID(fCFTypeRef) == ::CFDataGetTypeID())
		{
		fHasValue = false;
		return new ZStreamerRPos_T<ZStreamRPos_CFData>(static_cast<CFDataRef>(fCFTypeRef));
		}

	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_CFType::ReadYAD()
	{
	if (fHasValue)
		{
		fHasValue = false;
		return new ZYAD_CFType(fCFTypeRef);
		}

	return ZRef<ZYAD>();
	}

void ZYADReaderRep_CFType::Skip()
	{
	fHasValue = false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_CFType definition

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

ZMapReaderRep_CFType::ZMapReaderRep_CFType(CFDictionaryRef iCFDictionaryRef)
:	fCFDictionaryRef(iCFDictionaryRef),
	fIndex(0)
	{
	::CFRetain(fCFDictionaryRef);
	GatherContents_t theParam;
	theParam.fNames = &fNames;
	theParam.fValues = &fValues;
	::CFDictionaryApplyFunction(fCFDictionaryRef, sGatherContents, &theParam);
	}

ZMapReaderRep_CFType::~ZMapReaderRep_CFType()
	{
	::CFRelease(fCFDictionaryRef);
	}

bool ZMapReaderRep_CFType::HasValue()
	{
	return fIndex < fNames.size();
	}

string ZMapReaderRep_CFType::Name()
	{
	if (fIndex < fNames.size())
		return ZUtil_CFType::sAsUTF8(fNames.at(fIndex));
	return string();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_CFType::Read()
	{
	if (fIndex < fValues.size())
		return new ZYADReaderRep_CFType(CFTypeRef(fValues.at(fIndex++)));
	return ZRef<ZYADReaderRep>();
	}

void ZMapReaderRep_CFType::Skip()
	{
	if (fIndex < fValues.size())
		++fIndex;
	}

bool ZMapReaderRep_CFType::IsSimple(const ZYADOptions& iOptions)
	{ return !sIsComplexDictionary(iOptions, fCFDictionaryRef); }

bool ZMapReaderRep_CFType::CanRandomAccess()
	{ return true; }

size_t ZMapReaderRep_CFType::Count()
	{ return fValues.size(); }

ZRef<ZYADReaderRep> ZMapReaderRep_CFType::ReadWithName(const string& iName)
	{
	CFStringRef theKey = ZUtil_CFType::sCreateCFString_UTF8(iName);
	CFTypeRef result = ::CFDictionaryGetValue(fCFDictionaryRef, theKey);
	::CFRelease(theKey);
	if (result)
		return new ZYADReaderRep_CFType(result);

	return ZRef<ZYADReaderRep>();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_CFType::ReadAtIndex(size_t iIndex)
	{
	if (iIndex < fValues.size())
		return new ZYADReaderRep_CFType(CFTypeRef(fValues.at(iIndex)));

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_CFType definition

ZListReaderRep_CFType::ZListReaderRep_CFType(CFArrayRef iCFArrayRef)
:	fCFArrayRef(iCFArrayRef),
	fIndex(0)
	{
	::CFRetain(fCFArrayRef);
	}

ZListReaderRep_CFType::~ZListReaderRep_CFType()
	{
	::CFRelease(fCFArrayRef);
	}

bool ZListReaderRep_CFType::HasValue()
	{
	return fIndex < ::CFArrayGetCount(fCFArrayRef);
	}

size_t ZListReaderRep_CFType::Index()
	{
	return fIndex;
	}

ZRef<ZYADReaderRep> ZListReaderRep_CFType::Read()
	{
	CFIndex theSize = ::CFArrayGetCount(fCFArrayRef);
	if (fIndex < theSize)
		{
		CFTypeRef theValue = ::CFArrayGetValueAtIndex(fCFArrayRef, fIndex++);
		return new ZYADReaderRep_CFType(theValue);
		}

	return ZRef<ZYADReaderRep>();
	}

void ZListReaderRep_CFType::Skip()
	{
	if (fIndex < ::CFArrayGetCount(fCFArrayRef))
		++fIndex;
	}

bool ZListReaderRep_CFType::IsSimple(const ZYADOptions& iOptions)
	{ return !sIsComplexArray(iOptions, fCFArrayRef); }

bool ZListReaderRep_CFType::CanRandomAccess()
	{ return true; }

size_t ZListReaderRep_CFType::Count()
	{ return ::CFArrayGetCount(fCFArrayRef); }

ZRef<ZYADReaderRep> ZListReaderRep_CFType::ReadWithIndex(size_t iIndex)
	{
	if (iIndex < ::CFArrayGetCount(fCFArrayRef))
		{
		CFTypeRef theValue = ::CFArrayGetValueAtIndex(fCFArrayRef, iIndex);
		return new ZYADReaderRep_CFType(theValue);
		}

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Test code

static CFDictionaryRef sReadDictionary(ZMapReader iMapReader)
	{
	CFMutableDictionaryRef result = ::CFDictionaryCreateMutable(
		kCFAllocatorDefault, 0,
		&kCFCopyStringDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

	while (iMapReader)
		{
		CFStringRef theStringRef = ZUtil_CFType::sCreateCFString_UTF8(iMapReader.Name());
		if (CFTypeRef theValue = ZYADUtil_CFType::sFromReader(iMapReader.Read()))
			{
			::CFDictionarySetValue(result, theStringRef, theValue);
			::CFRelease(theValue);
			}
		::CFRelease(theStringRef);
		}

	return result;
	}

static CFArrayRef sReadArray(ZListReader iListReader)
	{
	CFMutableArrayRef result = ::CFArrayCreateMutable(
		kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

	while (iListReader)
		{
		CFTypeRef theValue = ZYADUtil_CFType::sFromReader(iListReader.Read());
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

CFTypeRef ZYADUtil_CFType::sFromReader(ZYADReader iYADReader)
	{
	if (!iYADReader)
		return nil;

	if (iYADReader.IsMap())
		{
		return sReadDictionary(iYADReader.ReadMap());
		}
	else if (iYADReader.IsList())
		{
		return sReadArray(iYADReader.ReadList());
		}
	else if (iYADReader.IsRaw())
		{
		return sReadData(iYADReader.ReadRaw()->GetStreamR());
		}
	else
		{
		ZRef<ZYAD> theYAD = iYADReader.ReadYAD();
		if (ZRef<ZYAD_CFType> theYAD_CFType = ZRefDynamicCast<ZYAD_CFType>(theYAD))
			{
			CFTypeRef result = theYAD_CFType->GetCFTypeRef();
			::CFRetain(result);
			return result;
			}
		else
			{
			return ZUtil_CFType::sCreateCFType(theYAD->GetTValue());
			}
		}
	}

#endif // ZCONFIG_SPI_Enabled(CFType)
