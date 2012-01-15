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

#include "zoolib/ZCallable_Bool.h"
#include "zoolib/ZCallable_Const.h"

namespace ZooLib {

// =================================================================================================
// MARK: - anonymous

namespace { // anonymous

typedef ZCallable_Bool ZCB;

struct Base : public ZCB
	{
	Base(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1) : f0(i0) , f1(i1) {}
	const ZRef<ZCB> f0;
	const ZRef<ZCB> f1;	
	};

} // anonymous namespace

// =================================================================================================
// MARK: - Makers

ZRef<ZCB> sCallable_True()
	{ return sCallable_Const(true); }

ZRef<ZCB> sCallable_False()
	{ return sCallable_Const(false); }

ZRef<ZCB> sCallable_Not(const ZRef<ZCB>& iCallable)
	{
	struct Callable : public ZCB
		{
		Callable(const ZRef<ZCB>& iCallable) : fCallable(iCallable) {}
		virtual ZQ<bool> QCall()
			{
			if (ZQ<bool> theQ = sQCall(fCallable))
				return not theQ.Get();
			return null;
			}
		const ZRef<ZCB> fCallable;
		};
	return new Callable(iCallable);
	}

ZRef<ZCB> sCallable_And(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1)
	{
	struct Callable : public Base
		{
		Callable(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1) : Base(i0, i1) {}
		virtual ZQ<bool> QCall()
			{
			if (ZQ<bool> theQ0 = sQCall(f0))
				{
				if (theQ0.Get())
					return sQCall(f1);
				return false;
				}
			return null;
			}
		};
	return new Callable(i0, i1);
	}

ZRef<ZCB> sCallable_Or(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1)
	{
	struct Callable : public Base
		{
		Callable(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1) : Base(i0, i1) {}
		virtual ZQ<bool> QCall()
			{
			if (ZQ<bool> theQ0 = sQCall(f0) && theQ0.Get())
				return true;
			return sQCall(f1);
			}
		};
	return new Callable(i0, i1);
	}

ZRef<ZCB> sCallable_Xor(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1)
	{
	struct Callable : public Base
		{
		Callable(const ZRef<ZCB>& i0, const ZRef<ZCB>& i1) : Base(i0, i1) {}
		virtual ZQ<bool> QCall()
			{
			if (ZQ<bool> theQ0 = sQCall(f0))
				{
				if (ZQ<bool> theQ1 = sQCall(f1))
					return theQ0.Get() ^ theQ1.Get();
				}
			return null;
			}
		};
	return new Callable(i0, i1);
	}

} // namespace ZooLib
