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

#ifndef __ZooLib_CountedVal_h__
#define __ZooLib_CountedVal_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"
#include "zoolib/TagVal.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark CountedVal

template <class Type_p, class Tag_p = Type_p>
class CountedVal
:	public TagVal<Type_p,Tag_p>
,	public CountedWithoutFinalize
	{
	typedef TagVal<Type_p,Tag_p> inherited;
public:
	CountedVal()
		{}

	CountedVal(const CountedVal& iOther)
	:	inherited(iOther)
		{}

	virtual ~CountedVal()
		{}

	template <class P0>
	CountedVal(const P0& i0)
	:	inherited(i0)
		{}

	template <class P0, class P1>
	CountedVal(const P0& i0, const P1& i1)
	:	inherited(i0, i1)
		{}

	ZRef<CountedVal> Clone()
		{ return new CountedVal(*this); }

	ZRef<CountedVal> Fresh()
		{
		if (this->IsShared())
			return this->Clone();
		return this;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark sCountedVal

template <class T>
ZRef<CountedVal<T> > sCountedVal()
	{ return new CountedVal<T>; }

template <class P0, class T = P0>
ZRef<CountedVal<T> > sCountedVal(const P0& i0)
	{ return new CountedVal<T>(i0); }

template <class T, class P0, class P1>
ZRef<CountedVal<T> > sCountedVal(const P0& i0, const P1& i1)
	{ return new CountedVal<T>(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark Accessors

template <class Type_p, class Tag_p>
const Type_p* sPGet(const ZRef<CountedVal<Type_p,Tag_p> >& iCountedVal)
	{
	if (iCountedVal)
		return &iCountedVal->Get();
	return nullptr;
	}

template <class Type_p, class Tag_p>
const ZQ<Type_p> sQGet(const ZRef<CountedVal<Type_p,Tag_p> >& iCountedVal)
	{
	if (const Type_p* theVal = sPGet(iCountedVal))
		return *theVal;
	return null;
	}

template <class Type_p, class Tag_p>
const Type_p& sDGet(const Type_p& iDefault, const ZRef<CountedVal<Type_p,Tag_p> >& iCountedVal)
	{
	if (const Type_p* theVal = sPGet(iCountedVal))
		return *theVal;
	return iDefault;
	}

template <class Type_p, class Tag_p>
const Type_p& sGet(const ZRef<CountedVal<Type_p,Tag_p> >& iCountedVal)
	{
	if (const Type_p* theVal = sPGet(iCountedVal))
		return *theVal;
	return sDefault<Type_p>();
	}

template <class Type_p, class Tag_p>
Type_p* sPMut(const ZRef<CountedVal<Type_p,Tag_p> >& ioCountedVal)
	{
	if (ioCountedVal)
		return &ioCountedVal->Mut();
	return nullptr;
	}

template <class Type_p, class Tag_p>
Type_p& sDMut(const Type_p& iDefault, ZRef<CountedVal<Type_p,Tag_p> >& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new CountedVal<Type_p,Tag_p>(iDefault);
	return ioCountedVal->Mut();
	}

template <class Type_p, class Tag_p>
Type_p& sMut(ZRef<CountedVal<Type_p,Tag_p> >& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new CountedVal<Type_p,Tag_p>;
	return ioCountedVal->Mut();
	}

template <class Type_p, class Tag_p>
Type_p& sFresh(ZRef<CountedVal<Type_p,Tag_p> >& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new CountedVal<Type_p,Tag_p>;
	else if (ioCountedVal->IsShared())
		ioCountedVal = ioCountedVal->Clone();
	return ioCountedVal->Mut();
	}

} // namespace ZooLib

#endif // __ZooLib_CountedVal_h__
