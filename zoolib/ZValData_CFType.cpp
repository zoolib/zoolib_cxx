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
#include "zoolib/ZUtil_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFData.h)

NAMESPACE_ZOOLIB_BEGIN

using ZUtil_CFType::sDataMutable;

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_CFType

ZValData_CFType::operator bool() const
	{ return this->GetSize(); }

ZValData_CFType::ZValData_CFType()
:	inherited(sDataMutable())
	{}

ZValData_CFType::ZValData_CFType(const ZValData_CFType& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZValData_CFType::~ZValData_CFType()
	{}

ZValData_CFType& ZValData_CFType::operator=(const ZValData_CFType& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZValData_CFType::ZValData_CFType(const ZRef<CFMutableDataRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZValData_CFType::ZValData_CFType(const ZRef<CFDataRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZValData_CFType& ZValData_CFType::operator=(const ZRef<CFMutableDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZValData_CFType& ZValData_CFType::operator=(const ZRef<CFDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZValData_CFType::ZValData_CFType(size_t iSize)
:	inherited(sDataMutable(iSize))
,	fMutable(true)
	{}

ZValData_CFType::ZValData_CFType(const void* iSourceData, size_t iSize)
:	inherited(NoRetain(::CFDataCreate(
		kCFAllocatorDefault, static_cast<const UInt8*>(iSourceData), iSize)))
,	fMutable(true)
	{}

size_t ZValData_CFType::GetSize() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetLength(theData);
	return 0;
	}

void ZValData_CFType::SetSize(size_t iSize)
	{
	CFMutableDataRef theData = this->pTouch();
	::CFDataSetLength(theData, iSize);
	}

const void* ZValData_CFType::GetData() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetBytePtr(theData);
	return nullptr;
	}

void* ZValData_CFType::GetData()
	{
	CFMutableDataRef theData = this->pTouch();
	return ::CFDataGetMutableBytePtr(theData);
	}

void ZValData_CFType::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	if (iCount)
		{
		CFMutableDataRef theData = this->pTouch();
		const CFRange theRange = { iOffset, iCount };
		::CFDataReplaceBytes(theData, theRange, static_cast<const UInt8*>(iSource), iCount);
		}
	}

void ZValData_CFType::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZValData_CFType::CopyTo(size_t iOffset, void* iDest, size_t iCount) const
	{
	const CFRange theRange = { iOffset, iCount };
	CFDataRef theData = this->pData();
	ZAssert(theData);
	::CFDataGetBytes(theData, theRange, static_cast<UInt8*>(iDest));
	}

void ZValData_CFType::CopyTo(void* iDest, size_t iCount) const
	{ this->CopyTo(0, iDest, iCount); }

CFDataRef ZValData_CFType::pData() const
	{ return inherited::Get(); }

CFMutableDataRef ZValData_CFType::pTouch()
	{
	ZRef<CFMutableDataRef> theMutableData;
	if (CFDataRef theData = this->pData())
		{
		if (!fMutable || ::CFGetRetainCount(theData) > 1)
			{
			theMutableData = sDataMutable(theData);
			inherited::operator=(theMutableData);
			}
		else
			{
			theMutableData = const_cast<CFMutableDataRef>(theData);
			}
		}
	else
		{
		theMutableData = sDataMutable();
		inherited::operator=(theMutableData);
		}
	fMutable = true;
	return theMutableData;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
