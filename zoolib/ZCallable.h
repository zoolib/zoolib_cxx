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

#ifndef __ZCallable__
#define __ZCallable__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableBase

template <typename Func_p>
class ZCallableBase
:	public ZCounted
	{
public:
	typedef Func_p Func;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable

template <typename Func> class ZCallable;

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 0 params)

template <typename R>
class ZCallable<R (void)>
:	public ZCallableBase<R (void)>
	{
public:
	virtual R Call() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 1 param)

template <typename R, typename P0>
class ZCallable<R (P0)>
:	public ZCallableBase<R (P0)>
	{
public:
	virtual R Call(P0) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 2 params)

template <typename R, typename P0, typename P1>
class ZCallable<R (P0,P1)>
:	public ZCallableBase<R (P0, P1)>
	{
public:
	virtual R Call(P0, P1) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 3 params)

template <typename R, typename P0, typename P1, typename P2>
class ZCallable<R (P0, P1, P2)>
:	public ZCallableBase<R (P0, P1, P2)>
	{
public:
	virtual R Call(P0, P1, P2) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 4 params)

template <typename R, typename P0, typename P1, typename P2, typename P3>
class ZCallable<R (P0, P1, P2, P3)>
:	public ZCallableBase<R (P0, P1, P2, P3)>
	{
public:
	virtual R Call(P0, P1, P2, P3) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 5 params)

template <typename R, typename P0, typename P1, typename P2, typename P3, typename P4>
class ZCallable<R (P0, P1, P2, P3, P4)>
:	public ZCallableBase<R (P0, P1, P2, P3, P4)>
	{
public:
	virtual R Call(P0, P1, P2, P3, P4) = 0;
	};

} // namespace ZooLib

#endif // __ZCallable__
