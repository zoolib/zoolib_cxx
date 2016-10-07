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

#ifndef __ZooLib_Apple_Data_CF_h__
#define __ZooLib_Apple_Data_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Any.h"

#include "zoolib/Apple/ZRef_CF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Data_CF

class Data_CF
:	public ZRef<CFDataRef>
	{
	typedef ZRef<CFDataRef> inherited;

	class Rep;

public:
	Data_CF();
	Data_CF(const Data_CF& iOther);
	~Data_CF();
	Data_CF& operator=(const Data_CF& iOther);

	Data_CF(CFMutableDataRef iOther);
	Data_CF(CFDataRef iOther);

	Data_CF(const Adopt_T<CFMutableDataRef>& iOther);
	Data_CF(const Adopt_T<CFDataRef>& iOther);

	Data_CF& operator=(CFMutableDataRef iOther);
	Data_CF& operator=(CFDataRef iOther);

	Data_CF& operator=(const Adopt_T<CFMutableDataRef>& iOther);
	Data_CF& operator=(const Adopt_T<CFDataRef>& iOther);

	Data_CF(size_t iSize);
	Data_CF(const void* iSource, size_t iSize);

// ZData protocol
	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetPtr() const;
	void* GetPtrMutable();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* oDest, size_t iCount) const;
	void CopyTo(void* oDest, size_t iCount) const;

private:
	CFDataRef pData() const;
	CFMutableDataRef pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Data_CF_h__
