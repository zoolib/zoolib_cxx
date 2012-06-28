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

#ifndef __ZCountedVal_h__
#define __ZCountedVal_h__ 1
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZTagVal.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCountedVal

template <class Value, class Tag = Value>
class ZCountedVal
:	public ZTagVal<Value,Tag>
,	public ZCountedWithoutFinalize
	{
	typedef ZTagVal<Value,Tag> inherited;
public:
	ZCountedVal()
		{}

	ZCountedVal(const ZCountedVal& iOther)
	:	inherited(iOther)
		{}

	virtual ~ZCountedVal()
		{}

	template <class P0>
	ZCountedVal(const P0& i0)
	:	inherited(i0)
		{}

	template <class P0, class P1>
	ZCountedVal(const P0& i0, const P1& i1)
	:	inherited(i0, i1)
		{}

	ZRef<ZCountedVal> Clone()
		{ return new ZCountedVal(*this); }

	ZRef<ZCountedVal> Fresh()
		{
		if (this->IsShared())
			return this->Clone();
		return this;
		}
	};

// =================================================================================================
// MARK: - sCountedVal

template <class T>
ZRef<ZCountedVal<T> > sCountedVal()
	{ return new ZCountedVal<T>(); }

template <class T>
ZRef<ZCountedVal<T> > sCountedVal(const T& iOther)
	{ return new ZCountedVal<T>(iOther); }

template <class T, class P0>
ZRef<ZCountedVal<T> > sCountedVal(const P0& i0)
	{ return new ZCountedVal<T>(i0); }

template <class T, class P0, class P1>
ZRef<ZCountedVal<T> > sCountedVal(const P0& i0, const P1& i1)
	{ return new ZCountedVal<T>(i0, i1); }

// =================================================================================================
// MARK: - Accessors

template <class Value, class Tag>
const Value* sPGet(const ZRef<ZCountedVal<Value,Tag> >& iCountedVal)
	{
	if (iCountedVal)
		return &iCountedVal->Get();
	return nullptr;
	}

template <class Value, class Tag>
const ZQ<Value> sQGet(const ZRef<ZCountedVal<Value,Tag> >& iCountedVal)
	{
	if (const Value* theValue = sPGet(iCountedVal))
		return *theValue;
	return null;
	}

template <class Value, class Tag>
const Value& sDGet(const Value& iDefault, const ZRef<ZCountedVal<Value,Tag> >& iCountedVal)
	{
	if (const Value* theValue = sPGet(iCountedVal))
		return *theValue;
	return iDefault;
	}

template <class Value, class Tag>
const Value& sGet(const ZRef<ZCountedVal<Value,Tag> >& iCountedVal)
	{
	if (const Value* theValue = sPGet(iCountedVal))
		return *theValue;
	return sDefault<Value>();
	}

template <class Value, class Tag>
Value* sPMut(const ZRef<ZCountedVal<Value,Tag> >& ioCountedVal)
	{
	if (ioCountedVal)
		return &ioCountedVal->Mut();
	return nullptr;
	}

template <class Value, class Tag>
Value& sDMut(const Value& iDefault, ZRef<ZCountedVal<Value,Tag> >& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new ZCountedVal<Value,Tag>(iDefault);
	return ioCountedVal->Mut();
	}

template <class Value, class Tag>
Value& sMut(ZRef<ZCountedVal<Value,Tag> >& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new ZCountedVal<Value,Tag>();
	return ioCountedVal->Mut();
	}

template <class Value, class Tag>
Value& sFresh(ZRef<ZCountedVal<Value,Tag> >& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new ZCountedVal<Value,Tag>();
	else if (ioCountedVal->IsShared())
		ioCountedVal = ioCountedVal->Clone();
	return ioCountedVal->Mut();
	}

} // namespace ZooLib

#endif // __ZCountedVal_h__
