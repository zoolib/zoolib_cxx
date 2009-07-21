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

#ifndef __ZValData_ZooLib__
#define __ZValData_ZooLib__ 1
#include "zconfig.h"

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZTypes.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_ZooLib

class ZValData_ZooLib
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValData_ZooLib,
    	operator_bool_generator_type, operator_bool_type);
	class Rep;

public:
	operator operator_bool_type() const;

	ZValData_ZooLib();
	ZValData_ZooLib(const ZValData_ZooLib& iOther);
	~ZValData_ZooLib();
	ZValData_ZooLib& operator=(const ZValData_ZooLib& iOther);

	ZValData_ZooLib(size_t iSize);
	ZValData_ZooLib(const void* iSourceData, size_t iSize);

	int Compare(const ZValData_ZooLib& iOther) const;
	bool operator<(const ZValData_ZooLib& iOther) const;
	bool operator==(const ZValData_ZooLib& iOther) const;

	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetData() const;
	void* GetData();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* iDest, size_t iCount) const;
	void CopyTo(void* iDest, size_t iCount) const;

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

template <> inline int sCompare_T(const ZValData_ZooLib& iL, const ZValData_ZooLib& iR)
	{ return iL.Compare(iR); }

NAMESPACE_ZOOLIB_END

#endif // __ZValData_ZooLib__
