/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZCallback_T__
#define __ZCallback_T__ 1
#include "zconfig.h"

#include "zoolib/ZWeakRef.h"
#include "zoolib/ZSafeSet.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallback_T0

class ZCallback_T0
:	public ZCounted
,	public ZWeakReferee
	{
public:
	virtual void Invoke() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallback_T1

template <class T0>
class ZCallback_T1
:	public ZCounted
,	public ZWeakReferee
	{
public:
	virtual void Invoke(T0 iParam) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallbackSet_T0

class ZCallbackSet_T0
	{
public:
	typedef ZCallback_T0 Callback_t;

	void Register(ZRef<Callback_t> iCallback)
		{ fCallbacks.Insert(iCallback); }

	void Unregister(ZRef<Callback_t> iCallback)
		{ fCallbacks.Erase(iCallback); }

	void Invoke()
		{
		for (ZSafeSetIterConst<ZWeakRef<Callback_t> > iter = fCallbacks;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callback_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callback_t> theCB = theQ.Get())
					theCB.Get()->Invoke();
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callback_t> > fCallbacks;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallbackSet_T1

template <class P0>
class ZCallbackSet_T1
	{
public:
	typedef ZCallback_T1<P0> Callback_t;

	void Register(ZRef<Callback_t> iCallback)
		{ fCallbacks.Insert(iCallback); }

	void Unregister(ZRef<Callback_t> iCallback)
		{ fCallbacks.Erase(iCallback); }

	void Invoke(P0 iP0)
		{
		for (ZSafeSetIterConst<ZWeakRef<Callback_t> > iter = fCallbacks;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callback_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callback_t> theCB = theQ.Get())
					theCB.Get()->Invoke(iP0);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callback_t> > fCallbacks;
	};

} // namespace ZooLib

#endif // __ZCallback_T__
