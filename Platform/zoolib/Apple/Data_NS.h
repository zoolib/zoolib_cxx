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

#ifndef __ZooLib_Apple_Data_NS_h__
#define __ZooLib_Apple_Data_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/Any.h"

#include "zoolib/Apple/ZRef_NS.h"

#import <Foundation/NSData.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Data_NS

class Data_NS
:	public ZRef<NSData>
	{
	typedef ZRef<NSData> inherited;
public:
	Data_NS();
	Data_NS(const Data_NS& iOther);
	~Data_NS();
	Data_NS& operator=(const Data_NS& iOther);

	Data_NS(NSMutableData* iOther);
	Data_NS(NSData* iOther);

	Data_NS(const Adopt_T<NSMutableData>& iOther);
	Data_NS(const Adopt_T<NSData>& iOther);

	Data_NS& operator=(NSMutableData* iOther);
	Data_NS& operator=(NSData* iOther);

	Data_NS& operator=(const Adopt_T<NSMutableData>& iOther);
	Data_NS& operator=(const Adopt_T<NSData>& iOther);

	Data_NS(size_t iSize);
	Data_NS(const void* iSource, size_t iSize);

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
	NSData* pData() const;
	NSMutableData* pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZooLib_Apple_Data_NS_h__
