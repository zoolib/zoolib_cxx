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
#pragma mark -
#pragma mark * Callables (anonymous)

namespace { // anonymous

struct Base_Binary : public ZCallable_Bool
	{
	Base_Binary(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1) : f0(i0) , f1(i1) {}
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;	
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Makers

ZRef<ZCallable_Bool> sCallable_True()
	{ return sCallable_Const(true); }

ZRef<ZCallable_Bool> sCallable_False()
	{ return sCallable_Const(false); }

ZRef<ZCallable_Bool> sCallable_Not(const ZRef<ZCallable_Bool>& iCallable)
	{
	struct Callable : public ZCallable_Bool
		{
		Callable(const ZRef<ZCallable_Bool>& iCallable) : fCallable(iCallable) {}
		virtual bool Call() { return not sCall(fCallable); }
		const ZRef<ZCallable_Bool> fCallable;
		};
	return new Callable(iCallable);
	}

ZRef<ZCallable_Bool> sCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{
	struct Callable : public Base_Binary
		{
		Callable(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
		:	Base_Binary(i0, i1) {}
		virtual bool Call() { return sCall(f0) && sCall(f1); }
		};
	return new Callable(i0, i1);
	}

ZRef<ZCallable_Bool> sCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{
	struct Callable : public Base_Binary
		{
		Callable(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
		:	Base_Binary(i0, i1) {}
		virtual bool Call() { return sCall(f0) || sCall(f1); }
		};
	return new Callable(i0, i1);
	}

ZRef<ZCallable_Bool> sCallable_Xor(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{
	struct Callable : public Base_Binary
		{
		Callable(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
		:	Base_Binary(i0, i1) {}
		virtual bool Call() { return sCall(f0) ^ sCall(f1); }
		};
	return new Callable(i0, i1);
	}

} // namespace ZooLib
