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

#ifndef __ZData_NS__
#define __ZData_NS__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZAny.h"
#include "zoolib/ZRef_NS.h"

#import <Foundation/NSData.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZData_NS

class ZData_NS
:	public ZRef<NSData>
	{
	typedef ZRef<NSData> inherited;

	class Rep;
public:
	ZAny AsAny() const;

	operator bool() const;

	ZData_NS();
	ZData_NS(const ZData_NS& iOther);
	~ZData_NS();
	ZData_NS& operator=(const ZData_NS& iOther);

	ZData_NS(const ZRef<NSData>& iOther);
	ZData_NS(NSMutableData* iOther);
	ZData_NS(NSData* iOther);

	ZData_NS& operator=(const ZRef<NSData>& iOther);
	ZData_NS& operator=(NSMutableData* iOther);
	ZData_NS& operator=(NSData* iOther);

	ZData_NS(size_t iSize);
	ZData_NS(const void* iSource, size_t iSize);

// ZData protocol
	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetData() const;
	void* GetDataMutable();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* oDest, size_t iCount) const;
	void CopyTo(void* oDest, size_t iCount) const;

private:
	NSData* pData() const;
	NSMutableData* pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZData_NS__
