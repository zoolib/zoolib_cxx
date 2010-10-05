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

struct Empty_t {};

// =================================================================================================
#pragma mark -
#pragma mark * SigTraits_T

template <class Func>
struct SigTraits_T;

template <class R_p>
struct SigTraits_T<R_p (void)>
	{
	typedef R_p R;

	typedef Empty_t L0;
	typedef Empty_t L1;
	typedef Empty_t L2;
	typedef Empty_t L3;
	typedef Empty_t L4;

	typedef Empty_t R4;
	typedef Empty_t R3;
	typedef Empty_t R2;
	typedef Empty_t R1;
	typedef Empty_t R0;
	
	typedef R (Func)();
	};

template <class R_p, class P0>
struct SigTraits_T<R_p (P0)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef Empty_t L1;
	typedef Empty_t L2;
	typedef Empty_t L3;
	typedef Empty_t L4;

	typedef P0 R0;
	typedef Empty_t R1;
	typedef Empty_t R2;
	typedef Empty_t R3;
	typedef Empty_t R4;

	typedef R (Func)(P0);
	
	typedef R (FuncL1)();

	typedef R (FuncR1)();
	};

template <class R_p, class P0, class P1>
struct SigTraits_T<R_p (P0, P1)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef Empty_t L2;
	typedef Empty_t L3;
	typedef Empty_t L4;

	typedef P1 R0;
	typedef P0 R1;
	typedef Empty_t R2;
	typedef Empty_t R3;
	typedef Empty_t R4;

	typedef R (Func)(P0,P1);

	typedef R (FuncL1)(P1);
	typedef R (FuncL2)();

	typedef R (FuncR1)(P0);
	typedef R (FuncR2)();
	};

template <class R_p, class P0, class P1, class P2>
struct SigTraits_T<R_p (P0, P1, P2)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef Empty_t L3;
	typedef Empty_t L4;

	typedef P2 R0;
	typedef P1 R1;
	typedef P0 R2;
	typedef Empty_t R3;
	typedef Empty_t R4;

	typedef R (Func)(P0, P1, P2);

	typedef R (FuncL1)(P1,P2);
	typedef R (FuncL2)(P2);
	typedef R (FuncL3)();

	typedef R (FuncR1)(P0, P1);
	typedef R (FuncR2)(P0);
	typedef R (FuncR3)();
	};

template <class R_p, class P0, class P1, class P2, class P3>
struct SigTraits_T<R_p (P0, P1, P2, P3)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef Empty_t L4;

	typedef P3 R0;
	typedef P2 R1;
	typedef P1 R2;
	typedef P0 R3;
	typedef Empty_t R4;

	typedef R (Func)(P0, P1, P2, P3);

	typedef R (FuncL1)(P1, P2, P3);
	typedef R (FuncL2)(P2, P3);
	typedef R (FuncL3)(P3);
	typedef R (FuncL4)();

	typedef R (FuncR1)(P0, P1, P2);
	typedef R (FuncR2)(P0, P1);
	typedef R (FuncR3)(P0);
	typedef R (FuncR4)();
	};

template <class R_p, class P0, class P1, class P2, class P3, class P4>
struct SigTraits_T<R_p (P0, P1, P2, P3, P4)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;

	typedef P4 R0;
	typedef P3 R1;
	typedef P2 R2;
	typedef P1 R3;
	typedef P0 R4;

	typedef R (Func)(P0, P1, P2, P3, P4);

	typedef R (FuncL1)(P1, P2, P3, P4);
	typedef R (FuncL2)(P2, P3, P4);
	typedef R (FuncL3)(P3, P4);
	typedef R (FuncL4)(P4);
	typedef R (FuncL5)();

	typedef R (FuncR1)(P0, P1, P2, P3);
	typedef R (FuncR2)(P0, P1, P2);
	typedef R (FuncR3)(P0, P1);
	typedef R (FuncR4)(P0);
	typedef R (FuncR5)();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable

template <typename Func> class ZCallable;

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
