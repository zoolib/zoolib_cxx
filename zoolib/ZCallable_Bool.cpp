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

class Callable_Not
:	public ZCallable_Bool
	{
public:
	Callable_Not(const ZRef<ZCallable_Bool>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual bool Call()
		{ return not sCall(fCallable); }

private:
	const ZRef<ZCallable_Bool> fCallable;
	};

//---

class Callable_And
:	public ZCallable_Bool
	{
public:
	Callable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZCallable
	virtual bool Call()
		{ return sCall(f0) && sCall(f1); }

private:
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;
	};

//---

class Callable_Or
:	public ZCallable_Bool
	{
public:
	Callable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZCallable
	virtual bool Call()
		{ return sCall(f0) || sCall(f1); }

private:
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;
	};

//---

class Callable_Xor
:	public ZCallable_Bool
	{
public:
	Callable_Xor(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZCallable
	virtual bool Call()
		{ return sCall(f0) ^ sCall(f1); }

private:
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;
	};

//---

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Makers

ZRef<ZCallable_Bool> MakeCallable_True()
	{ return MakeCallable_Const(true); }

ZRef<ZCallable_Bool> MakeCallable_False()
	{ return MakeCallable_Const(false); }

ZRef<ZCallable_Bool> MakeCallable_Not(const ZRef<ZCallable_Bool>& iCallable)
	{ return new Callable_Not(iCallable); }

ZRef<ZCallable_Bool>
MakeCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return new Callable_And(i0, i1); }

ZRef<ZCallable_Bool> MakeCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return new Callable_Or(i0, i1); }

ZRef<ZCallable_Bool> MakeCallable_Xor
	(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return new Callable_Xor(i0, i1); }

} // namespace ZooLib
