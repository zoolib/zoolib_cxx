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

#ifndef __ZCallable_Function__
#define __ZCallable_Function__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_R0

template <class R>
class ZCallable_Function_R0
:	public ZCallable_R0<R>
	{
public:
	typedef R (*Function_t)();

	ZCallable_Function_R0(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_R0
	virtual R Invoke()
		{ return fFunction(); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_R1

template <class R, class P0>
class ZCallable_Function_R1
:	public ZCallable_R1<R, P0>
	{
public:
	typedef R (*Function_t)(P0);

	ZCallable_Function_R1(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_R1
	virtual R Invoke(P0 i0)
		{ return fFunction(i0); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_R2

template <class R, class P0, class P1>
class ZCallable_Function_R2
:	public ZCallable_R2<R, P0, P1>
	{
public:
	typedef R (*Function_t)(P0, P1);

	ZCallable_Function_R2(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_R2
	virtual R Invoke(P0 i0, P1 i1)
		{ return fFunction(i0, i1); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_R3

template <class R, class P0, class P1, class P2>
class ZCallable_Function_R3
:	public ZCallable_R3<R, P0, P1, P2>
	{
public:
	typedef R (*Function_t)(P0, P1, P2);

	ZCallable_Function_R3(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_R3
	virtual R Invoke(P0 i0, P1 i1, P2 i2)
		{ return fFunction(i0, i1, i2); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_R4

template <class R, class P0, class P1, class P2, class P3>
class ZCallable_Function_R4
:	public ZCallable_R4<R, P0, P1, P2, P3>
	{
public:
	typedef R (*Function_t)(P0, P1, P2, P3);

	ZCallable_Function_R4(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_R4
	virtual R Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fFunction(i0, i1, i2, i3); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_V0

class ZCallable_Function_V0
:	public ZCallable_V0
	{
public:
	typedef void (*Function_t)();

	ZCallable_Function_V0(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_V0
	virtual void Invoke()
		{ fFunction(); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_V1

template <class P0>
class ZCallable_Function_V1
:	public ZCallable_V1<P0>
	{
public:
	typedef void (*Function_t)(P0);

	ZCallable_Function_V1(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_V1
	virtual void Invoke(P0 i0)
		{ fFunction(i0); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_V2

template <class P0, class P1>
class ZCallable_Function_V2
:	public ZCallable_V2<P0, P1>
	{
public:
	typedef void (*Function_t)(P0, P1);

	ZCallable_Function_V2(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_V2
	virtual void Invoke(P0 i0, P1 i1)
		{ fFunction(i0, i1); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_V3

template <class P0, class P1, class P2>
class ZCallable_Function_V3
:	public ZCallable_V3<P0, P1, P2>
	{
public:
	typedef void (*Function_t)(P0, P1, P2);

	ZCallable_Function_V3(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_V3
	virtual void Invoke(P0 i0, P1 i1, P2 i2)
		{ fFunction(i0, i1, i2); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function_V4

template <class P0, class P1, class P2, class P3>
class ZCallable_Function_V4
:	public ZCallable_V4<P0, P1, P2, P3>
	{
public:
	typedef void (*Function_t)(P0, P1, P2, P3);

	ZCallable_Function_V4(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable_V4
	virtual void Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{ fFunction(i0, i1, i2, i3); }

private:
	Function_t fFunction;
	};

} // namespace ZooLib

#endif // __ZCallable_Function__
