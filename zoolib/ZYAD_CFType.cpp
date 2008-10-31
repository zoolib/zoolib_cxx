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

using std::string;
using std::vector;

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
:	fCFTypeRef(iCFTypeRef)
	{
	::CFRetain(fCFTypeRef);
	}

ZYADReaderRep_CFType::~ZYADReaderRep_CFType()
	{
	::CFRelease(fCFTypeRef);
	}

bool ZYADReaderRep_CFType::HasValue()
	{ return true; }

ZType ZYADReaderRep_CFType::Type()
	{
	return ZUtil_CFType::sTypeOf(fCFTypeRef);
	}

ZRef<ZMapReaderRep> ZYADReaderRep_CFType::ReadMap()
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFDictionaryGetTypeID())
		return new ZMapReaderRep_CFType(static_cast<CFDictionaryRef>(fCFTypeRef));

	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_CFType::ReadList()
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFArrayGetTypeID())
		return new ZListReaderRep_CFType(static_cast<CFArrayRef>(fCFTypeRef));

	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_CFType::ReadRaw()
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFDataGetTypeID())
		return new ZStreamerRPos_T<ZStreamRPos_CFData>(static_cast<CFDataRef>(fCFTypeRef));

	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_CFType::ReadYAD()
	{
	return new ZYAD_CFType(fCFTypeRef);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_CFType definition

struct Applier_t
	{
	vector<CFStringRef>* fNames;
	vector<CFTypeRef>* fValues;
	};
	
static void sApplier(const void* iKey, const void* iValue, void* iRefcon)
	{
	Applier_t* theApplier = static_cast<Applier_t*>(iRefcon);
	theApplier->fNames->push_back(static_cast<CFStringRef>(iKey));
	theApplier->fValues->push_back(static_cast<CFTypeRef>(iValue));
	}

ZMapReaderRep_CFType::ZMapReaderRep_CFType(CFDictionaryRef iCFDictionaryRef)
:	fCFDictionaryRef(iCFDictionaryRef),
	fIndex(0)
	{
	::CFRetain(fCFDictionaryRef);
	Applier_t theApplier;
	theApplier.fNames = &fNames;
	theApplier.fValues = &fValues;
	::CFDictionaryApplyFunction(fCFDictionaryRef, sApplier, &theApplier);
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
		return ZUtil_CFType::sAsUTF8(fNames[fIndex]);
	return string();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_CFType::Read()
	{
	if (fIndex < fValues.size())
		return new ZYADReaderRep_CFType(fValues[fIndex++]);
	return ZRef<ZYADReaderRep>();
	}

void ZMapReaderRep_CFType::Skip()
	{
	if (fIndex < fValues.size())
		++fIndex;
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

size_t ZListReaderRep_CFType::Index() const
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

// =================================================================================================
#pragma mark -
#pragma mark * Test code

CFTypeRef sReadValue(ZYADReader iYADReader);

static CFDictionaryRef sReadDictionary(ZMapReader iMapReader)
	{
	CFMutableDictionaryRef result = ::CFDictionaryCreateMutable(
		kCFAllocatorDefault, 0,
		&kCFCopyStringDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

	while (iMapReader)
		{
		CFStringRef theStringRef = ZUtil_CFType::sCreateCFString_UTF8(iMapReader.Name());
		CFTypeRef theValue = ZYADUtil_CFType::sFromReader(iMapReader.Read());
		::CFDictionaryAddValue(result, theStringRef, theValue);
		::CFRelease(theStringRef);
		::CFRelease(theValue);
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

bool ZYADUtil_CFType::sFromReader(ZYADReader iYADReader, CFTypeRef& oCFTypeRef)
	{
	if (!iYADReader)
		return false;

	if (iYADReader.IsMap())
		{
		oCFTypeRef = sReadDictionary(iYADReader.ReadMap());
		}
	else if (iYADReader.IsList())
		{
		oCFTypeRef = sReadArray(iYADReader.ReadList());
		}
	else if (iYADReader.IsRaw())
		{
		oCFTypeRef = sReadData(iYADReader.ReadRaw()->GetStreamR());
		}
	else
		{
		ZRef<ZYAD> theYAD = iYADReader.ReadYAD();
		if (ZRef<ZYAD_CFType> theYAD_CFType = ZRefDynamicCast<ZYAD_CFType>(theYAD))
			{
			oCFTypeRef = theYAD_CFType->GetCFTypeRef();
			::CFRetain(oCFTypeRef);
			}
		else
			{
			oCFTypeRef = ZUtil_CFType::sCreateCFType(theYAD->GetTValue());
			}
		}
	return true;
	}

CFTypeRef ZYADUtil_CFType::sFromReader(ZYADReader iYADReader)
	{
	CFTypeRef theCFTypeRef;
	if (sFromReader(iYADReader, theCFTypeRef))
		return theCFTypeRef;
	return nil;
	}

#endif // ZCONFIG_SPI_Enabled(CFType)
