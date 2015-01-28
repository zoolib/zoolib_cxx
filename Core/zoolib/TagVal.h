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

#ifndef __ZooLib_TagVal_h__
#define __ZooLib_TagVal_h__ 1
#include "zconfig.h"

#include "zoolib/Util_Relops.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark TagVal

template <class Type_p, class Tag_p>
class TagVal
	{
private:
	template <class OtherTag>
	TagVal(const TagVal<Type_p,OtherTag>& iOther);

	template <class OtherTag>
	TagVal& operator=(const TagVal<Type_p,OtherTag>& iOther);

public:
	typedef Tag_p Tag_t;
	typedef Type_p Type_t;

	TagVal()
	:	fVal(Type_p())
		{}

	TagVal(const TagVal& iOther)
	:	fVal(iOther.fVal)
		{}

	~TagVal()
		{}

	TagVal& operator=(const TagVal& iOther)
		{
		fVal = iOther.fVal;
		return *this;
		}

	template <class P0>
	TagVal(const P0& i0)
	:	fVal(i0)
		{}

	template <class P0>
	TagVal& operator=(const P0& i0)
		{
		fVal = i0;
		return *this;
		}

	template <class P0, class P1>
	TagVal(const P0& i0, const P1& i1)
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

	bool operator==(const TagVal& iOther) const
		{ return fVal == iOther.fVal; }

	bool operator<(const TagVal& iOther) const
		{ return fVal < iOther.fVal; }

protected:
	Type_p fVal;
	};

template <class Type_p, class Tag_p>
struct RelopsTraits_HasEQ<TagVal<Type_p,Tag_p> > : public RelopsTraits_Has {};

template <class Type_p, class Tag_p>
struct RelopsTraits_HasLT<TagVal<Type_p,Tag_p> > : public RelopsTraits_Has {};

// =================================================================================================
#pragma mark -
#pragma mark Accessors

template <class Type_p, class Tag_p>
const Type_p* sPGet(const TagVal<Type_p,Tag_p>& iTagVal)
	{ return &iTagVal.Get(); }

template <class Type_p, class Tag_p>
const Type_p& sGet(const TagVal<Type_p,Tag_p>& iTagVal)
	{ return iTagVal.Get(); }

template <class Type_p, class Tag_p>
Type_p* sPMut(TagVal<Type_p,Tag_p>& ioTagVal)
	{ return &ioTagVal.Mut(); }

template <class Type_p, class Tag_p>
Type_p& sMut(TagVal<Type_p,Tag_p>& ioTagVal)
	{ return ioTagVal.Mut(); }

template <class Type_p, class Tag_p>
Type_p& sSet(TagVal<Type_p,Tag_p>& ioTagVal, const Type_p& iVal)
	{ return ioTagVal.Set(iVal); }

} // namespace ZooLib

#endif // __ZooLib_TagVal_h__
