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

#ifndef __ZData_Any_h__
#define __ZData_Any_h__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZCompare_T.h"
#include "zoolib/ZRef.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZData_Any

class ZData_Any
	{
	class Rep;

public:
	ZAny AsAny() const;

	ZData_Any();
	ZData_Any(const ZData_Any& iOther);
	~ZData_Any();
	ZData_Any& operator=(const ZData_Any& iOther);

	ZData_Any(size_t iSize);
	ZData_Any(const void* iSourceData, size_t iSize);

	int Compare(const ZData_Any& iOther) const;
	bool operator<(const ZData_Any& iOther) const;
	bool operator==(const ZData_Any& iOther) const;

	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetData() const;
	void* GetDataMutable();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* oDest, size_t iCount) const;
	void CopyTo(void* oDest, size_t iCount) const;

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

template <> inline int sCompare_T(const ZData_Any& iL, const ZData_Any& iR)
	{ return iL.Compare(iR); }

} // namespace ZooLib

#endif // __ZData_Any_h__
