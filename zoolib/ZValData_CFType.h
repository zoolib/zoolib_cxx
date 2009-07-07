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

#include "zoolib/ZRef_CFType.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_CFType

class ZValData_CFType
:	public ZRef<CFDataRef>
	{
	class Rep;

public:
	operator bool() const;

	ZValData_CFType();
	ZValData_CFType(const ZValData_CFType& iOther);
	~ZValData_CFType();
	ZValData_CFType& operator=(const ZValData_CFType& iOther);

	ZValData_CFType(const ZRef<CFMutableDataRef>& iOther);
	ZValData_CFType(const ZRef<CFDataRef>& iOther);

	ZValData_CFType& operator=(const ZRef<CFMutableDataRef>& iOther);
	ZValData_CFType& operator=(const ZRef<CFDataRef>& iOther);

	ZValData_CFType(size_t iSize);
	ZValData_CFType(const void* iSourceData, size_t iSize);

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
	ZRef<CFDataRef> Data() const;
	ZRef<CFMutableDataRef> DataMutable();

private:
	CFDataRef pData() const;
	CFMutableDataRef pTouch();
	bool fMutable;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZValData_CFType__
