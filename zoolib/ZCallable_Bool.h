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

#ifndef __ZCallable_Bool__
#define __ZCallable_Bool__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

typedef ZCallable<bool()> ZCallable_Bool;

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_True

ZRef<ZCallable_Bool> MakeCallable_True();

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_False

ZRef<ZCallable_Bool> MakeCallable_False();

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Not

class ZCallable_Not
:	public ZCallable_Bool
	{
public:
	ZCallable_Not(const ZRef<ZCallable_Bool>& iCallable);

// From ZCallable
	virtual bool Call();

private:
	const ZRef<ZCallable_Bool> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Not

ZRef<ZCallable_Bool> MakeCallable_Not(const ZRef<ZCallable_Bool>& iCallable);

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_And

class ZCallable_And
:	public ZCallable_Bool
	{
public:
	ZCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// From ZCallable
	virtual bool Call();

private:
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_And

ZRef<ZCallable_Bool>
MakeCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or

class ZCallable_Or
:	public ZCallable_Bool
	{
public:
	ZCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// From ZCallable
	virtual bool Call();

private:
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Or

ZRef<ZCallable_Bool>
MakeCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_If

class ZCallable_If
:	public ZCallable_Bool
	{
public:
	ZCallable_If(const ZRef<ZCallable_Bool>& iCondition,
		const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// From ZCallable
	virtual bool Call();

private:
	const ZRef<ZCallable_Bool> fCondition;
	const ZRef<ZCallable_Bool> f0;
	const ZRef<ZCallable_Bool> f1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_If

ZRef<ZCallable_Bool> MakeCallable_If(const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_While

class ZCallable_While
:	public ZCallable_Bool
	{
public:
	ZCallable_While(const ZRef<ZCallable_Bool>& iCallable);

// From ZCallable
	virtual bool Call();

private:
	const ZRef<ZCallable_Bool> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_While

ZRef<ZCallable_Bool> MakeCallable_While(const ZRef<ZCallable_Bool>& iCallable);

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Repeat

class ZCallable_Repeat
:	public ZCallable_Bool
	{
public:
	ZCallable_Repeat(size_t iCount, const ZRef<ZCallable_Bool>& iCallable);

// From ZCallable
	virtual bool Call();

private:
	const size_t fCount;
	const ZRef<ZCallable_Bool> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Repeat

ZRef<ZCallable_Bool> MakeCallable_Repeat(size_t iCount, const ZRef<ZCallable_Bool>& iCallable);

// =================================================================================================
#pragma mark -
#pragma mark * Shorter callable ctors

ZRef<ZCallable_Bool> Callable_True();

ZRef<ZCallable_Bool> Callable_False();

ZRef<ZCallable_Bool> Not(const ZRef<ZCallable_Bool>& iCallable);

ZRef<ZCallable_Bool> And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> If(const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> While(const ZRef<ZCallable_Bool>& iCallable);

ZRef<ZCallable_Bool> Repeat(size_t iCount, const ZRef<ZCallable_Bool>& iCallable);

// =================================================================================================
#pragma mark -
#pragma mark * Infix notation

ZRef<ZCallable_Bool> operator&(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>& operator&=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> operator|(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>& operator|=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> operator~(const ZRef<ZCallable_Bool>& iCallable);

} // namespace ZooLib

#endif // __ZCallable_Bool__
