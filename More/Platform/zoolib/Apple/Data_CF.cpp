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

#include "zoolib/Data_Any.h"

#include "zoolib/Apple/Data_CF.h"
#include "zoolib/Apple/Util_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFData.h)

namespace ZooLib {

using Util_CF::sData;
using Util_CF::sDataMutable;

// =================================================================================================
#pragma mark -
#pragma mark Data_CF

Data_CF::Data_CF()
:	inherited(sDataMutable())
	{}

Data_CF::Data_CF(const Data_CF& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

Data_CF::~Data_CF()
	{}

Data_CF& Data_CF::operator=(const Data_CF& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

Data_CF::Data_CF(CFMutableDataRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Data_CF::Data_CF(CFDataRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Data_CF::Data_CF(const Adopt_T<CFMutableDataRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Data_CF::Data_CF(const Adopt_T<CFDataRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Data_CF& Data_CF::operator=(CFMutableDataRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Data_CF& Data_CF::operator=(CFDataRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Data_CF& Data_CF::operator=(const Adopt_T<CFMutableDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Data_CF& Data_CF::operator=(const Adopt_T<CFDataRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Data_CF::Data_CF(size_t iSize)
:	inherited(sDataMutable(iSize))
,	fMutable(true)
	{}

Data_CF::Data_CF(const void* iSource, size_t iSize)
:	inherited(sData(iSource, iSize))
,	fMutable(false)
	{}

size_t Data_CF::GetSize() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetLength(theData);
	return 0;
	}

void Data_CF::SetSize(size_t iSize)
	{
	if (this->GetSize() != iSize)
		::CFDataSetLength(this->pTouch(), iSize);
	}

const void* Data_CF::GetPtr() const
	{
	if (CFDataRef theData = this->pData())
		return ::CFDataGetBytePtr(theData);
	return nullptr;
	}

void* Data_CF::GetPtrMutable()
	{ return ::CFDataGetMutableBytePtr(this->pTouch()); }

void Data_CF::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	if (iCount)
		{
		const CFRange theRange = { CFIndex(iOffset), CFIndex(iCount) };
		::CFDataReplaceBytes(this->pTouch(), theRange, static_cast<const UInt8*>(iSource), iCount);
		}
	}

void Data_CF::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void Data_CF::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	if (iCount)
		{
		CFDataRef theData = this->pData();
		ZAssert(theData);
		const CFRange theRange = { CFIndex(iOffset), CFIndex(iCount) };
		::CFDataGetBytes(theData, theRange, static_cast<UInt8*>(oDest));
		}
	}

void Data_CF::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

CFDataRef Data_CF::pData() const
	{ return inherited::Get(); }

CFMutableDataRef Data_CF::pTouch()
	{
	ZRef<CFMutableDataRef> theMutableData;
	if (CFDataRef theData = this->pData())
		{
		if (not fMutable || ::CFGetRetainCount(theData) > 1)
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
