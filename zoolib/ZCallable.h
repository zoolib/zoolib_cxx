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
#pragma mark * ZCallable

class ZCallable
:	public ZCounted
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable0

template <class R>
class ZCallable0
:	public ZCallable
	{
public:
	typedef R R_t;
	typedef R (*Function_t)();
	virtual R Invoke() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable1

template <class R, class P0>
class ZCallable1
:	public ZCallable
	{
public:
	typedef R R_t;
	typedef P0 P0_t;
	typedef R (*Function_t)(P0);
	virtual R Invoke(P0) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable2

template <class R, class P0, class P1>
class ZCallable2
:	public ZCallable
	{
public:
	typedef R R_t;
	typedef P0 P0_t;
	typedef P1 P1_t;
	typedef R (*Function_t)(P0, P1);
	virtual R Invoke(P0, P1) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable3

template <class R, class P0, class P1, class P2>
class ZCallable3
:	public ZCallable
	{
public:
	typedef R R_t;
	typedef P0 P0_t;
	typedef P1 P1_t;
	typedef P2 P2_t;
	typedef R (*Function_t)(P0, P1, P2);
	virtual R Invoke(P0, P1, P2) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable4

template <class R, class P0, class P1, class P2, class P3>
class ZCallable4
:	public ZCallable
	{
public:
	typedef R R_t;
	typedef P0 P0_t;
	typedef P1 P1_t;
	typedef P2 P2_t;
	typedef P3 P3_t;
	typedef R (*Function_t)(P0, P1, P2, P3);
	virtual R Invoke(P0, P1, P2, P3 i3) = 0;
	};

} // namespace ZooLib

#endif // __ZCallable__
