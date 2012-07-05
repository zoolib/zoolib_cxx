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

#ifndef __ZThreadVal_h__
#define __ZThreadVal_h__ 1
#include "zconfig.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZTagVal.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZThreadVal

template <class Value, class Tag = Value>
class ZThreadVal
:	public ZTagVal<Value,Tag>
	{
	typedef ZTagVal<Value,Tag> inherited;
public:
	ZThreadVal()
	:	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	ZThreadVal(const ZThreadVal& iOther)
	:	inherited(iOther)
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	~ZThreadVal()
		{ ZTSS::sSet(spKey(), fPrior); }

	ZThreadVal& operator=(const ZThreadVal& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

	ZThreadVal(const Value& iValue)
	:	inherited(iValue)
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	ZThreadVal& operator=(const Value& iValue)
		{
		inherited::operator=(iValue);
		return *this;
		}

	//--

	static const Value* sPGet()
		{
		if (ZThreadVal* theTV = spGet())
			return &theTV->Get();
		return null;
		}

	static ZQ<Value> sQGet()
		{
		if (ZThreadVal* theTV = spGet())
			return theTV->Get();
		return null;
		}

	static Value sDGet(const Value& iDefault)
		{
		if (ZThreadVal* theTV = spGet())
			return theTV->Get();
		return iDefault;
		}

	static const Value& sGet()
		{
		if (ZThreadVal* theTV = spGet())
			return theTV->Get();
		return sDefault<Value>();
		}

	static Value* sPMut()
		{
		if (ZThreadVal* theTV = spGet())
			return &theTV->Mut();
		return null;
		}

	static Value& sMut()
		{
		ZThreadVal* theTV = spGet();
		ZAssert(theTV);
		return theTV->Mut();
		}

private:
	static ZThreadVal* spGet()
		{ return ((ZThreadVal*)(ZTSS::sGet(spKey()))); }

	static const ZTSS::Key& spKey()
		{
		// This method is a sneaky way to have static storage for a template class.
		static ZTSS::Key spKey = ZTSS::sCreate();
		return spKey;
		}

	ZThreadVal* fPrior;
	};

// =================================================================================================
// MARK: - sThreadVal

// Not sure about this one yet.
template <class T>
const T& sThreadVal()
	{ return ZThreadVal<T>::sGet(); }

} // namespace ZooLib

#endif // __ZThreadVal_h__
