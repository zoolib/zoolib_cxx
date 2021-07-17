// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_CountedVal_h__
#define __ZooLib_CountedVal_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"
#include "zoolib/TagVal.h"

#include "zoolib/ZP.h"
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedVal

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

	ZP<CountedVal> Clone()
		{ return new CountedVal(*this); }

	ZP<CountedVal> Fresh()
		{
		if (this->IsShared())
			return this->Clone();
		return this;
		}
	};

// =================================================================================================
#pragma mark - sCountedVal

template <class T>
ZP<CountedVal<T>> sCountedVal()
	{ return new CountedVal<T>; }

template <class P0, class T = P0>
ZP<CountedVal<T>> sCountedVal(const P0& i0)
	{ return new CountedVal<T>(i0); }

template <class T, class P0, class P1>
ZP<CountedVal<T>> sCountedVal(const P0& i0, const P1& i1)
	{ return new CountedVal<T>(i0, i1); }

// =================================================================================================
#pragma mark - Accessors

template <class Type_p, class Tag_p>
const Type_p* sPGet(const ZP<CountedVal<Type_p,Tag_p>>& iCountedVal)
	{
	if (iCountedVal)
		return &iCountedVal->Get();
	return nullptr;
	}

template <class Type_p, class Tag_p>
const ZQ<Type_p> sQGet(const ZP<CountedVal<Type_p,Tag_p>>& iCountedVal)
	{
	if (const Type_p* theVal = sPGet(iCountedVal))
		return *theVal;
	return null;
	}

template <class Type_p, class Tag_p>
const Type_p& sGet(const ZP<CountedVal<Type_p,Tag_p>>& iCountedVal)
	{
	if (const Type_p* theVal = sPGet(iCountedVal))
		return *theVal;
	return sDefault<Type_p>();
	}

template <class Type_p, class Tag_p>
Type_p* sPMut(const ZP<CountedVal<Type_p,Tag_p>>& ioCountedVal)
	{
	if (ioCountedVal)
		return &ioCountedVal->Mut();
	return nullptr;
	}

template <class Type_p, class Tag_p>
Type_p& sMut(ZP<CountedVal<Type_p,Tag_p>>& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new CountedVal<Type_p,Tag_p>;
	return ioCountedVal->Mut();
	}

template <class Type_p, class Tag_p>
Type_p& sFresh(ZP<CountedVal<Type_p,Tag_p>>& ioCountedVal)
	{
	if (not ioCountedVal)
		ioCountedVal = new CountedVal<Type_p,Tag_p>;
	else if (ioCountedVal->IsShared())
		ioCountedVal = ioCountedVal->Clone();
	return ioCountedVal->Mut();
	}

} // namespace ZooLib

#endif // __ZooLib_CountedVal_h__
