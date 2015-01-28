/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZooLib_ThreadVal_h__
#define __ZooLib_ThreadVal_h__ 1
#include "zconfig.h"

#include "zoolib/TagVal.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ThreadVal

template <class Type_p, class Tag_p = Type_p>
class ThreadVal
:	public TagVal<Type_p,Tag_p>
	{
	typedef TagVal<Type_p,Tag_p> inherited;
public:
	ThreadVal()
	:	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	ThreadVal(const ThreadVal& iOther)
	:	inherited(iOther)
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	~ThreadVal()
		{ ZTSS::sSet(spKey(), fPrior); }

	ThreadVal& operator=(const ThreadVal& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

	ThreadVal(const Type_p& iVal)
	:	inherited(iVal)
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	ThreadVal& operator=(const Type_p& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	// -----

	static const Type_p* sPGet()
		{
		if (ThreadVal* theTV = spGet())
			return &theTV->Get();
		return nullptr;
		}

	static ZQ<Type_p> sQGet()
		{
		if (ThreadVal* theTV = spGet())
			return theTV->Get();
		return null;
		}

	static Type_p sDGet(const Type_p& iDefault)
		{
		if (ThreadVal* theTV = spGet())
			return theTV->Get();
		return iDefault;
		}

	static const Type_p& sGet()
		{
		if (ThreadVal* theTV = spGet())
			return theTV->Get();
		return sDefault<Type_p>();
		}

	static Type_p* sPMut()
		{
		if (ThreadVal* theTV = spGet())
			return &theTV->Mut();
		return nullptr;
		}

	static Type_p& sMut()
		{
		ThreadVal* theTV = spGet();
		ZAssert(theTV);
		return theTV->Mut();
		}

private:
	static ThreadVal* spGet()
		{ return ((ThreadVal*)(ZTSS::sGet(spKey()))); }

	static ZTSS::Key spKey()
		{
		static ZAtomicPtr_t spKey;
		return ZTSS::sKey(spKey);
		}

	ThreadVal* fPrior;
	};

// =================================================================================================
#pragma mark -
#pragma mark sThreadVal

template <class Type_p, class Tag_p>
ZQ<Type_p> sQThreadVal()
	{ return ThreadVal<Type_p,Tag_p>::sQGet(); }

template <class Type_p>
ZQ<Type_p> sQThreadVal()
	{ return ThreadVal<Type_p,Type_p>::sQGet(); }

template <class Type_p, class Tag_p>
Type_p* sPThreadVal()
	{ return ThreadVal<Type_p,Tag_p>::sPMut(); }

template <class Type_p>
Type_p* sPThreadVal()
	{ return ThreadVal<Type_p,Type_p>::sPMut(); }

template <class Type_p, class Tag_p>
Type_p& sThreadVal()
	{ return ThreadVal<Type_p,Tag_p>::sMut(); }

template <class Type_p>
Type_p& sThreadVal()
	{ return ThreadVal<Type_p,Type_p>::sMut(); }

} // namespace ZooLib

#endif // __ZooLib_ThreadVal_h__
