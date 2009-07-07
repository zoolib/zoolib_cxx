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

#include "zoolib/ZValData_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFData.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

ZRef<CFMutableDataRef> sDataMutable()
	{ return NoRetain(::CFDataCreateMutable(kCFAllocatorDefault, 0)); }

ZRef<CFMutableDataRef> sDataMutable(size_t iSize)
	{
	ZRef<CFMutableDataRef> theData = NoRetain(::CFDataCreateMutable(kCFAllocatorDefault, iSize));
	::CFDataSetLength(theData, iSize);
	return theData;
	}

ZRef<CFMutableDataRef> sDataMutable(const ZRef<CFDataRef>& iCFData)
	{ return NoRetain(::CFDataCreateMutableCopy(kCFAllocatorDefault, 0, iCFData)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_CFType

ZValData_CFType::operator bool() const
	{ return this->GetSize(); }

ZValData_CFType::ZValData_CFType()
:	fCFMutableDataRef(sDataMutable())
	{}

ZValData_CFType::ZValData_CFType(const ZValData_CFType& iOther)
:	fCFMutableDataRef(iOther.fCFMutableDataRef)
,	fCFDataRef(iOther.fCFDataRef)
	{}

ZValData_CFType::~ZValData_CFType()
	{}

ZValData_CFType& ZValData_CFType::operator=(const ZValData_CFType& iOther)
	{
	fCFMutableDataRef = iOther.fCFMutableDataRef;
	fCFDataRef = iOther.fCFDataRef;
	ZAssert(!fCFMutableDataRef || !fCFDataRef);
	return *this;
	}

ZValData_CFType::ZValData_CFType(const ZRef<CFMutableDataRef>& iOther)
:	fCFMutableDataRef(iOther)
	{}

ZValData_CFType::ZValData_CFType(const ZRef<CFDataRef>& iOther)
:	fCFDataRef(iOther)
	{}

ZValData_CFType& ZValData_CFType::operator=(const ZRef<CFMutableDataRef>& iOther)
	{
	fCFMutableDataRef = iOther;
	fCFDataRef.Clear();
	return *this;
	}

ZValData_CFType& ZValData_CFType::operator=(const ZRef<CFDataRef>& iOther)
	{
	fCFMutableDataRef.Clear();
	fCFDataRef = iOther;
	return *this;
	}

ZValData_CFType::ZValData_CFType(size_t iSize)
:	fCFMutableDataRef(sDataMutable(iSize))
	{}

ZValData_CFType::ZValData_CFType(const void* iSourceData, size_t iSize)
:	fCFDataRef(NoRetain(::CFDataCreate(
		kCFAllocatorDefault, static_cast<const UInt8*>(iSourceData), iSize)))
	{}

size_t ZValData_CFType::GetSize() const
	{
	if (fCFMutableDataRef)
		return ::CFDataGetLength(fCFMutableDataRef);
	return ::CFDataGetLength(fCFDataRef);
	}

void ZValData_CFType::SetSize(size_t iSize)
	{
	this->pTouch();
	::CFDataSetLength(fCFMutableDataRef, iSize);
	}

const void* ZValData_CFType::GetData() const
	{
	if (fCFMutableDataRef)
		return ::CFDataGetBytePtr(fCFMutableDataRef);
	return ::CFDataGetBytePtr(fCFDataRef);
	}

void* ZValData_CFType::GetData()
	{
	this->pTouch();
	return ::CFDataGetMutableBytePtr(fCFMutableDataRef);
	}

void ZValData_CFType::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	if (iCount)
		{
		this->pTouch();
		const CFRange theRange = { iOffset, iCount };
		::CFDataReplaceBytes(fCFMutableDataRef, theRange, static_cast<const UInt8*>(iSource), iCount);
		}
	}

void ZValData_CFType::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZValData_CFType::CopyTo(size_t iOffset, void* iDest, size_t iCount) const
	{
	const CFRange theRange = { iOffset, iCount };
	if (fCFMutableDataRef)
		::CFDataGetBytes(fCFMutableDataRef, theRange, static_cast<UInt8*>(iDest));
	else
		::CFDataGetBytes(fCFDataRef, theRange, static_cast<UInt8*>(iDest));
	}

void ZValData_CFType::CopyTo(void* iDest, size_t iCount) const
	{ this->CopyTo(0, iDest, iCount); }

ZValData_CFType::operator CFTypeRef() const
	{
	if (fCFMutableDataRef)
		return fCFMutableDataRef;
	return fCFDataRef;
	}

ZValData_CFType::operator CFDataRef() const
	{
	if (fCFMutableDataRef)
		return fCFMutableDataRef;
	return fCFDataRef;
	}

void ZValData_CFType::pTouch()
	{
	if (!fCFMutableDataRef || ::CFGetRetainCount(fCFMutableDataRef) > 1)
		{
		if (fCFMutableDataRef)
			{
			fCFMutableDataRef = sDataMutable(fCFMutableDataRef);
			}
		else
			{
			ZAssert(fCFDataRef);
			fCFMutableDataRef = sDataMutable(fCFDataRef);
			fCFDataRef.Clear();
			}
		}
	ZAssert(!fCFDataRef);
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
