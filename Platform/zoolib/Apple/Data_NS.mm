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

#include "zoolib/Apple/Data_NS.h"
#include "zoolib/Apple/Util_NS.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

namespace ZooLib {

using Util_NS::sDataMutable;

// =================================================================================================
#pragma mark -
#pragma mark Data_NS

Data_NS::Data_NS()
:	inherited(sDataMutable())
	{}

Data_NS::Data_NS(const Data_NS& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

Data_NS::~Data_NS()
	{}

Data_NS& Data_NS::operator=(const Data_NS& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

Data_NS::Data_NS(const Adopt_T<NSMutableData>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Data_NS::Data_NS(const Adopt_T<NSData>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Data_NS::Data_NS(NSMutableData* iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Data_NS::Data_NS(NSData* iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Data_NS& Data_NS::operator=(const Adopt_T<NSMutableData>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Data_NS& Data_NS::operator=(const Adopt_T<NSData>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Data_NS& Data_NS::operator=(NSMutableData* iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Data_NS& Data_NS::operator=(NSData* iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Data_NS::Data_NS(size_t iSize)
:	inherited(sDataMutable(iSize))
,	fMutable(true)
	{}

Data_NS::Data_NS(const void* iSource, size_t iSize)
:	inherited(sDataMutable(iSource, iSize))
,	fMutable(true)
	{}

size_t Data_NS::GetSize() const
	{
	if (NSData* theData = this->pData())
		return [theData length];
	return 0;
	}

void Data_NS::SetSize(size_t iSize)
	{
	if (this->GetSize() != iSize)
		[this->pTouch() setLength:iSize];
	}

const void* Data_NS::GetPtr() const
	{
	if (NSData* theData = this->pData())
		return [theData bytes];
	return nullptr;
	}

void* Data_NS::GetPtrMutable()
	{ return [this->pTouch() mutableBytes]; }

void Data_NS::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	if (iCount)
		{
		const NSRange theRange = { iOffset, iCount };
		[this->pTouch() replaceBytesInRange:theRange withBytes:iSource];
		}
	}

void Data_NS::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void Data_NS::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	if (iCount)
		{
		NSData* theData = this->pData();
		ZAssert(theData);
		const NSRange theRange = { iOffset, iCount };
		[theData getBytes:oDest range:theRange];
		}
	}

void Data_NS::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

NSData* Data_NS::pData() const
	{ return inherited::Get(); }

NSMutableData* Data_NS::pTouch()
	{
	ZRef<NSMutableData> theMutableData;
	if (NSData* theData = this->pData())
		{
		if (not fMutable || sRetainCount(theData) > 1)
			{
			theMutableData = sDataMutable(theData);
			inherited::operator=(theMutableData);
			}
		else
			{
			theMutableData = static_cast<NSMutableData*>(theData);
			}
		}
	else
		{
		theMutableData = sDataMutable();
		inherited::operator=(theMutableData);
		}
	fMutable = true;
	return theMutableData.Get();
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
