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
#pragma mark * MakeCallable_True

ZRef<ZCallable_Bool> MakeCallable_True()
	{ return MakeCallable_Const(true); }

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_False

ZRef<ZCallable_Bool> MakeCallable_False()
	{ return MakeCallable_Const(false); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Not

ZCallable_Not::ZCallable_Not(const ZRef<ZCallable_Bool>& iCallable)
:	fCallable(iCallable)
	{}

bool ZCallable_Not::Call()
	{ return ! fCallable->Call(); }

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Not

ZRef<ZCallable_Bool> MakeCallable_Not(const ZRef<ZCallable_Bool>& iCallable)
	{ return new ZCallable_Not(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_And

ZCallable_And::ZCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
:	f0(i0)
,	f1(i1)
	{}

bool ZCallable_And::Call()
	{ return f0->Call() && f1->Call(); }

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_And

ZRef<ZCallable_Bool>
MakeCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return new ZCallable_And(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or

ZCallable_Or::ZCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
:	f0(i0)
,	f1(i1)
	{}

bool ZCallable_Or::Call()
	{ return f0->Call() || f1->Call(); }

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Or

ZRef<ZCallable_Bool> MakeCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return new ZCallable_Or(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_If

ZCallable_If::ZCallable_If(const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
:	fCondition(iCondition)
,	f0(i0)
,	f1(i1)
	{}
	
bool ZCallable_If::Call()
	{ return fCondition->Call() ? f0->Call() : f1->Call(); }

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_If

ZRef<ZCallable_Bool> MakeCallable_If(const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{
	return MakeCallable_Or(MakeCallable_And(iCondition, i0), i1);
//	return new ZCallable_If(iCondition, i0, i1);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_While

ZCallable_While::ZCallable_While(const ZRef<ZCallable_Bool>& iCallable)
:	fCallable(iCallable)
	{}

bool ZCallable_While::Call()
	{
	while (fCallable->Call())
		{}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_While

ZRef<ZCallable_Bool> MakeCallable_While(const ZRef<ZCallable_Bool>& iCallable)
	{ return new ZCallable_While(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Repeat

ZCallable_Repeat::ZCallable_Repeat(size_t iCount, const ZRef<ZCallable_Bool>& iCallable)
:	fCount(iCount)
,	fCallable(iCallable)
	{}

bool ZCallable_Repeat::Call()
	{
	for (size_t theCount = fCount; theCount; --theCount)
		fCallable->Call();
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Repeat

ZRef<ZCallable_Bool> MakeCallable_Repeat(size_t iCount, const ZRef<ZCallable_Bool>& iCallable)
	{ return new ZCallable_Repeat(iCount, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Shorter callable ctors

ZRef<ZCallable_Bool> Callable_True()
	{ return MakeCallable_True(); }

ZRef<ZCallable_Bool> Callable_False()
	{ return MakeCallable_False(); }

ZRef<ZCallable_Bool> Not(const ZRef<ZCallable_Bool>& iCallable)
	{ return MakeCallable_Not(iCallable); }

ZRef<ZCallable_Bool> And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return MakeCallable_And(i0, i1); }

ZRef<ZCallable_Bool> Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return MakeCallable_Or(i0, i1); }

ZRef<ZCallable_Bool> If(const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return MakeCallable_If(iCondition, i0, i1); }

ZRef<ZCallable_Bool> While(const ZRef<ZCallable_Bool>& iCallable)
	{ return MakeCallable_While(iCallable); }

ZRef<ZCallable_Bool> Repeat(size_t iCount, const ZRef<ZCallable_Bool>& iCallable)
	{ return MakeCallable_Repeat(iCount, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Infix notation

ZRef<ZCallable_Bool> operator&(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return And(i0, i1); }

ZRef<ZCallable_Bool>& operator&=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1)
	{ return io0 = io0 & i1; }

ZRef<ZCallable_Bool> operator|(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1)
	{ return Or(i0, i1); }

ZRef<ZCallable_Bool>& operator|=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1)
	{ return io0 = io0 | i1; }

ZRef<ZCallable_Bool> operator~(const ZRef<ZCallable_Bool>& iCallable)
	{ return Not(iCallable); }

} // namespace ZooLib
