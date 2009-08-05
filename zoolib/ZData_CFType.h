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

#ifndef __ZValData_CFType__
#define __ZValData_CFType__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZData_Any.h"
#include "zoolib/ZRef_CFType.h"

NAMESPACE_ZOOLIB_BEGIN

class ZData_CFType;
typedef ZData_CFType ZData_CF;

// =================================================================================================
#pragma mark -
#pragma mark * ZData_CFType

class ZData_CFType
:	public ZRef<CFDataRef>
	{
	typedef ZRef<CFDataRef> inherited;

	class Rep;
public:
	ZData_Any AsData_Any() const;

	operator bool() const;

	ZData_CFType();
	ZData_CFType(const ZData_CFType& iOther);
	~ZData_CFType();
	ZData_CFType& operator=(const ZData_CFType& iOther);

	ZData_CFType(const ZRef<CFMutableDataRef>& iOther);
	ZData_CFType(const ZRef<CFDataRef>& iOther);

	ZData_CFType& operator=(const ZRef<CFMutableDataRef>& iOther);
	ZData_CFType& operator=(const ZRef<CFDataRef>& iOther);

	ZData_CFType(size_t iSize);
	ZData_CFType(const void* iSourceData, size_t iSize);

// ZValData protocol
	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetData() const;
	void* GetData();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* iDest, size_t iCount) const;
	void CopyTo(void* iDest, size_t iCount) const;

// Our protocol
	CFDataRef& OParam();

private:
	CFDataRef pData() const;
	CFMutableDataRef pTouch();
	bool fMutable;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZValData_CFType__
