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

#ifndef __ZCallable_Bind__
#define __ZCallable_Bind__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_Bind {

// =================================================================================================
#pragma mark -
#pragma mark * ParamTraits

template <class T> struct ParamTraits
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct ParamTraits<const T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct ParamTraits<T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL1

template <class Func_p>
class CallableL1
:	public ZCallable<typename SigTraits_T<Func_p>::FuncL1>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableL1(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::R3 P0;
	typedef typename SigTraits::R2 P1;
	typedef typename SigTraits::R1 P2;
	typedef typename SigTraits::R0 P3;

	using ZCallable<typename SigTraits_T<Func_p>::FuncL1>::Call;

private:
	R Call()
		{ return fCallable->Call(f0); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, i0, i1, i2, i3); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL2

template <class Func_p>
class CallableL2
:	public ZCallable<typename SigTraits_T<Func_p>::FuncL2>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableL2(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0)
	,	f1(i1)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::R2 P0;
	typedef typename SigTraits::R1 P1;
	typedef typename SigTraits::R0 P2;

	using ZCallable<typename SigTraits_T<Func_p>::FuncL2>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, i0, i1, i2); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL3

template <class Func_p>
class CallableL3
:	public ZCallable<typename SigTraits_T<Func_p>::FuncL3>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableL3(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::R1 P0;
	typedef typename SigTraits::R0 P1;

	using ZCallable<typename SigTraits_T<Func_p>::FuncL3>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, i0, i1); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL4

template <class Func_p>
class CallableL4
:	public ZCallable<typename SigTraits_T<Func_p>::FuncL4>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableL4(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i3,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	f3(i3)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::R0 P0;

	using ZCallable<typename SigTraits_T<Func_p>::FuncL4>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, i0); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL5

template <class Func_p>
class CallableL5
:	public ZCallable<typename SigTraits_T<Func_p>::FuncL5>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableL5(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i4,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	f3(i3)
	,	f4(i4)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::R0 P0;

	using ZCallable<typename SigTraits_T<Func_p>::FuncL5>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR1

template <class Func_p>
class CallableR1
:	public ZCallable<typename SigTraits_T<Func_p>::FuncR1>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableR1(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i0)
	:	f0(i0)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;

	using ZCallable<typename SigTraits_T<Func_p>::FuncR1>::Call;

private:
	R Call(P0 i0)
		{ return fCallable->Call(i0, f0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0); }

	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f0;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR2

template <class Func_p>
class CallableR2
:	public ZCallable<typename SigTraits_T<Func_p>::FuncR2>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableR2(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i1)
	:	f0(i0)
	,	f1(i1)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;

	using ZCallable<typename SigTraits_T<Func_p>::FuncR2>::Call;

private:
	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1); }

	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f1;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR3

template <class Func_p>
class CallableR3
:	public ZCallable<typename SigTraits_T<Func_p>::FuncR3>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableR3(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i2)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;

	using ZCallable<typename SigTraits_T<Func_p>::FuncR3>::Call;

private:
	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2); }

	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f2;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR4

template <class Func_p>
class CallableR4
:	public ZCallable<typename SigTraits_T<Func_p>::FuncR4>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableR4(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i3)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	f3(i3)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;

	using ZCallable<typename SigTraits_T<Func_p>::FuncR4>::Call;

private:
	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3); }

	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f3;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR5

template <class Func_p>
class CallableR5
:	public ZCallable<typename SigTraits_T<Func_p>::FuncR5>
	{
public:
	typedef Func_p Func;
	typedef ZCallable<Func_p> Callable_t;
	typedef SigTraits_T<Func> SigTraits;

	CallableR5(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i4)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	f3(i3)
	,	f4(i4)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;

	using ZCallable<typename SigTraits_T<Func_p>::FuncR5>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4); }

	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f4;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * BindL

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncL1> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L0>::ConstRef_Add_t i0,
	const ZRef<Callable>& iCallable)
	{ return new CallableL1<typename Callable::Func>(i0, iCallable); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncL2> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L1>::ConstRef_Add_t i1,
	const ZRef<Callable>& iCallable)
	{ return new CallableL2<typename Callable::Func>(i0, i1, iCallable); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncL3> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L2>::ConstRef_Add_t i2,
	const ZRef<Callable>& iCallable)
	{ return new CallableL3<typename Callable::Func>(i0, i1, i2, iCallable); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncL4> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L3>::ConstRef_Add_t i3,
	const ZRef<Callable>& iCallable)
	{ return new CallableL4<typename Callable::Func>(i0, i1, i2, i3, iCallable); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncL5> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::L4>::ConstRef_Add_t i4,
	const ZRef<Callable>& iCallable)
	{ return new CallableL5<typename Callable::Func>(i0, i1, i2, i3, i4, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * BindR

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncR1> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R0>::ConstRef_Add_t i0)
	{ return new CallableR1<typename Callable::Func>(iCallable, i0); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncR2> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R1>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R0>::ConstRef_Add_t i1)
	{ return new CallableR2<typename Callable::Func>(iCallable, i0, i1); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncR3> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R2>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R0>::ConstRef_Add_t i2)
	{ return new CallableR3<typename Callable::Func>(iCallable, i0, i1, i2); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncR4> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R3>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R2>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R1>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R0>::ConstRef_Add_t i3)
	{ return new CallableR4<typename Callable::Func>(iCallable, i0, i1, i2, i3); }

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Func>::FuncR5> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R4>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R3>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R1>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Func>::R0>::ConstRef_Add_t i4)
	{ return new CallableR5<typename Callable::Func>(iCallable, i0, i1, i2, i3, i4); }

} // namespace ZCallable_Bind

using ZCallable_Bind::BindL;
using ZCallable_Bind::BindR;

} // namespace ZooLib

#endif // __ZCallable_Bind__
