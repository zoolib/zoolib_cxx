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

#include "zoolib/ZData_Any.h"
#include "zoolib/ZData_CFType.h"
#include "zoolib/ZUtil_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFData.h)

namespace ZooLib {

using ZUtil_CFType::sData;
using ZUtil_CFType::sDataMutable;

// =================================================================================================
#pragma mark -
#pragma mark * ZData_CFType

ZAny ZData_CFType::AsAny() const
	{ return ZUtil_CFType::sAsAny(this->pData()); }

ZData_CFType::operator bool() const
	{ return this->GetSize(); }

ZData_CFType::ZData_CFType()
:	inherited(sDataMutable())
	{}

ZData_CFType::ZData_CFType(const ZData_CFType& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZData_CFType::~ZData_CFType()
	{}

ZData_CFType& ZData_CFType::operator=(const ZData_CFType& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZData_CFType::ZData_CFType(const ZRef<CFMutableDataRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZData_CFType::ZData_CFType(const ZRef<CFDataRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZData_CFType& ZData_CFType::operator=(const ZRef<CFMutableDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZData_CFType& ZData_CFType::operator=(const ZRef<CFDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZData_CFType::ZData_CFType(size_t iSize)
:	inherited(sDataMutable(iSize))
,	fMutable(true)
	{}

ZData_CFType::ZData_CFType(const void* iSource, size_t iSize)
:	inherited(sData(iSource, iSize))
,	fMutable(false)
	{}

size_t ZData_CFType::GetSize() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetLength(theData);
	return 0;
	}

void ZData_CFType::SetSize(size_t iSize)
	{
	if (this->GetSize() != iSize)
		::CFDataSetLength(this->pTouch(), iSize);
	}

const void* ZData_CFType::GetData() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetBytePtr(theData);
	return nullptr;
	}

void* ZData_CFType::GetDataMutable()
	{ return ::CFDataGetMutableBytePtr(this->pTouch()); }

void ZData_CFType::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	if (iCount)
		{
		const CFRange theRange = { iOffset, iCount };
		::CFDataReplaceBytes(this->pTouch(), theRange, static_cast<const UInt8*>(iSource), iCount);
		}
	}

void ZData_CFType::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZData_CFType::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	if (iCount)
		{
		CFDataRef theData = this->pData();
		ZAssert(theData);
		const CFRange theRange = { iOffset, iCount };
		::CFDataGetBytes(theData, theRange, static_cast<UInt8*>(oDest));
		}
	}

void ZData_CFType::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

CFDataRef ZData_CFType::pData() const
	{ return inherited::Get(); }

CFMutableDataRef ZData_CFType::pTouch()
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

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
