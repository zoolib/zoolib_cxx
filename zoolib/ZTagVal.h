/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZTagVal__
#define __ZTagVal__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTagVal

template <class Value, class Tag>
class ZTagVal
	{
public:
	ZTagVal()
	:	fValue(Value())
		{}

	ZTagVal(const ZTagVal& iOther)
	:	fValue(iOther.fValue)
		{}

	~ZTagVal()
		{}

	ZTagVal& operator=(const ZTagVal& iOther)
		{
		fValue = iOther.fValue;
		return *this;
		}

	ZTagVal(const Value& iValue)
	:	fValue(iValue)
		{}

	ZTagVal& operator=(const Value& iValue)
		{
		fValue = iValue;
		return *this;
		}

	template <class P0>
	ZTagVal(const P0& i0)
	:	fValue(i0)
		{}

	template <class P0, class P1>
	ZTagVal(const P0& i0, const P1& i1)
	:	fValue(i0, i1)
		{}

	operator Value&()
		{ return fValue; }

	operator const Value&() const
		{ return fValue; }

	Value& GetMutable()
		{ return fValue; }

	const Value& Get() const
		{ return fValue; }

	void Set(const Value& iValue) const
		{ fValue = iValue; }

//	bool operator==(const ZTagVal& iOther) const
//		{ return fValue == iOther.fValue; }

//	bool operator!=(const ZTagVal& iOther) const
//		{ return fValue == iOther.fValue; }

	bool operator<(const ZTagVal& iOther) const
		{ return fValue < iOther.fValue; }

protected:
	Value fValue;
	};

} // namespace ZooLib

#endif // __ZTagVal__
