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
#include "zoolib/ZData_CF.h"
#include "zoolib/ZUtil_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFData.h)

namespace ZooLib {

using ZUtil_CF::sData;
using ZUtil_CF::sDataMutable;

// =================================================================================================
#pragma mark -
#pragma mark * ZData_CF

ZAny ZData_CF::AsAny() const
	{ return ZUtil_CF::sAsAny(this->pData()); }

ZData_CF::operator bool() const
	{ return this->GetSize(); }

ZData_CF::ZData_CF()
:	inherited(sDataMutable())
	{}

ZData_CF::ZData_CF(const ZData_CF& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZData_CF::~ZData_CF()
	{}

ZData_CF& ZData_CF::operator=(const ZData_CF& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZData_CF::ZData_CF(CFMutableDataRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZData_CF::ZData_CF(CFDataRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZData_CF::ZData_CF(const Adopt_T<CFMutableDataRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZData_CF::ZData_CF(const Adopt_T<CFDataRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZData_CF& ZData_CF::operator=(CFMutableDataRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZData_CF& ZData_CF::operator=(CFDataRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZData_CF& ZData_CF::operator=(const Adopt_T<CFMutableDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZData_CF& ZData_CF::operator=(const Adopt_T<CFDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZData_CF::ZData_CF(size_t iSize)
:	inherited(sDataMutable(iSize))
,	fMutable(true)
	{}

ZData_CF::ZData_CF(const void* iSource, size_t iSize)
:	inherited(sData(iSource, iSize))
,	fMutable(false)
	{}

size_t ZData_CF::GetSize() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetLength(theData);
	return 0;
	}

void ZData_CF::SetSize(size_t iSize)
	{
	if (this->GetSize() != iSize)
		::CFDataSetLength(this->pTouch(), iSize);
	}

const void* ZData_CF::GetData() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetBytePtr(theData);
	return nullptr;
	}

void* ZData_CF::GetDataMutable()
	{ return ::CFDataGetMutableBytePtr(this->pTouch()); }

void ZData_CF::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	if (iCount)
		{
		const CFRange theRange = { iOffset, iCount };
		::CFDataReplaceBytes(this->pTouch(), theRange, static_cast<const UInt8*>(iSource), iCount);
		}
	}

void ZData_CF::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZData_CF::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	if (iCount)
		{
		CFDataRef theData = this->pData();
		ZAssert(theData);
		const CFRange theRange = { iOffset, iCount };
		::CFDataGetBytes(theData, theRange, static_cast<UInt8*>(oDest));
		}
	}

void ZData_CF::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

CFDataRef ZData_CF::pData() const
	{ return inherited::Get(); }

CFMutableDataRef ZData_CF::pTouch()
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
