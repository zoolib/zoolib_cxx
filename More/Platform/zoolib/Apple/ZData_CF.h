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

#ifndef __ZData_CF_h__
#define __ZData_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZAny.h"
#include "zoolib/ZRef_CF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZData_CF

class ZData_CF
:	public ZRef<CFDataRef>
	{
	typedef ZRef<CFDataRef> inherited;

	class Rep;

public:
	ZData_CF();
	ZData_CF(const ZData_CF& iOther);
	~ZData_CF();
	ZData_CF& operator=(const ZData_CF& iOther);

	ZData_CF(CFMutableDataRef iOther);
	ZData_CF(CFDataRef iOther);

	ZData_CF(const Adopt_T<CFMutableDataRef>& iOther);
	ZData_CF(const Adopt_T<CFDataRef>& iOther);

	ZData_CF& operator=(CFMutableDataRef iOther);
	ZData_CF& operator=(CFDataRef iOther);

	ZData_CF& operator=(const Adopt_T<CFMutableDataRef>& iOther);
	ZData_CF& operator=(const Adopt_T<CFDataRef>& iOther);

	ZData_CF(size_t iSize);
	ZData_CF(const void* iSource, size_t iSize);

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

#endif // __ZData_CF_h__
