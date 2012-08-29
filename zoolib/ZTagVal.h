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

#ifndef __ZTagVal_h__
#define __ZTagVal_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZTagVal

template <class Type_p, class Tag_p>
class ZTagVal
	{
private:
	template <class OtherTag>
	ZTagVal(const ZTagVal<Type_p,OtherTag>& iOther);

	template <class OtherTag>
	ZTagVal& operator=(const ZTagVal<Type_p,OtherTag>& iOther);

public:
	ZTagVal()
	:	fVal(Type_p())
		{}

	ZTagVal(const ZTagVal& iOther)
	:	fVal(iOther.fVal)
		{}

	~ZTagVal()
		{}

	ZTagVal& operator=(const ZTagVal& iOther)
		{
		fVal = iOther.fVal;
		return *this;
		}

	template <class P0>
	ZTagVal(const P0& i0)
	:	fVal(i0)
		{}

	template <class P0>
	ZTagVal& operator=(const P0& i0)
		{
		fVal = i0;
		return *this;
		}

	template <class P0, class P1>
	ZTagVal(const P0& i0, const P1& i1)
	:	fVal(i0, i1)
		{}

	operator const Type_p&() const
		{ return fVal; }

	operator Type_p&()
		{ return fVal; }

	const Type_p& Get() const
		{ return fVal; }

	Type_p& Mut()
		{ return fVal; }

	Type_p& Set(const Type_p& iVal)
		{ return fVal = iVal; }

	bool operator==(const ZTagVal& iOther) const
		{ return fVal == iOther.fVal; }

	bool operator!=(const ZTagVal& iOther) const
		{ return not (fVal == iOther.fVal); }

	bool operator<(const ZTagVal& iOther) const
		{ return fVal < iOther.fVal; }

	bool operator>(const ZTagVal& iOther) const
		{ return iOther.fVal < fVal; }

	bool operator<=(const ZTagVal& iOther) const
		{ return not (iOther.fVal < fVal) ; }

	bool operator>=(const ZTagVal& iOther) const
		{ return not (fVal < iOther.fVal) ; }

protected:
	Type_p fVal;
	};

// =================================================================================================
// MARK: - Accessors

template <class Type_p, class Tag_p>
const Type_p* sPGet(const ZTagVal<Type_p,Tag_p>& iTagVal)
	{ return &iTagVal.Get(); }

template <class Type_p, class Tag_p>
const Type_p& sGet(const ZTagVal<Type_p,Tag_p>& iTagVal)
	{ return iTagVal.Get(); }

template <class Type_p, class Tag_p>
Type_p* sPMut(ZTagVal<Type_p,Tag_p>& ioTagVal)
	{ return &ioTagVal.Mut(); }

template <class Type_p, class Tag_p>
Type_p& sMut(ZTagVal<Type_p,Tag_p>& ioTagVal)
	{ return ioTagVal.Mut(); }

template <class Type_p, class Tag_p>
Type_p& sSet(ZTagVal<Type_p,Tag_p>& ioTagVal, const Type_p& iVal)
	{ return ioTagVal.Set(iVal); }

} // namespace ZooLib

#endif // __ZTagVal_h__
