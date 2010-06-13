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
#pragma mark * ZCallback_T

template <class Param_t>
class ZCallback_T
:	public ZCounted
,	public ZWeakReferee
	{
public:
	virtual void Invoke(Param_t iParam) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallbackSet_T

template <class Param_t>
class ZCallbackSet_T
:	public ZCounted
,	public ZWeakReferee
	{
public:
	void Register(ZRef<ZCallback_T<Param_t> > iCallback)
		{ fCallbacks.Insert(iCallback); }

	void Unregister(ZRef<ZCallback_T<Param_t> > iCallback)
		{ fCallbacks.Erase(iCallback); }

	void Invoke(Param_t iParam)
		{
		for (ZSafeSetIterConst<ZWeakRef<ZCallback_T<Param_t> > > iter = fCallbacks;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<ZCallback_T<Param_t> > > theQ = iter.QReadInc())
				{
				if (ZRef<ZCallback_T<Param_t> > theCB = theQ.Get())
					theCB.Get()->Invoke(iParam);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<ZCallback_T<Param_t> > > fCallbacks;
	};

} // namespace ZooLib

#endif // __ZDataspace_Source__
