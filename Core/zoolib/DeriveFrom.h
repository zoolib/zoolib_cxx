// Copyright (c) 2016-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_DeriveFrom_h__
#define __ZooLib_DeriveFrom_h__

#include "zconfig.h"

#include "zoolib/TypeList.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - DeriveFrom

template <
	class P0 = void, class P1 = void, class P2 = void, class P3 = void,
	class P4 = void, class P5 = void, class P6 = void, class P7 = void,
	class P8 = void,
	class End = void // Needed so that the 9-param specialization is still considered partial.
	>
class DeriveFrom;

// This template catches cases where we try doing a DeriveFrom of a DeriveFrom, which
// causes weird compilation errors.
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
class DeriveFrom<DeriveFrom<P0, P1, P2, P3, P4, P5, P6, P7, P8>>
	{};

// We end up with a lot of potential bases, and a lot of merging because they're virtual bases.
// The tentative number goes as the left factorial of the number of params (<http://oeis.org/A007489>),
// The number of vtbl ptrs in an instance goes as 2^numberOfParams. The runtime cost comes from the
// ctor-ing of each distinct base. It's roughly 3^numberOfParams. Compilation time is closer
// to the left factorial number, and is really quite bad with 9 bases.

// Generally try to use six or fewer bases, using seven or more only in special cases.
// Once the DeriveFrom<> object is ctor-ed, call dispatch is constant time, as you'd expect.

// !1 == 1, 0.0335us, 8 bytes
template <class P0>
class DeriveFrom<P0>
:	public virtual P0
	{
public:
	typedef TypeList<P0> AsTypeList_t;
	};

// !2 == 3, 3x, 0.0476us, 1.42x, 16 bytes
template <class P0, class P1>
class DeriveFrom<P0, P1>
:	public virtual DeriveFrom<    P1>
,	public virtual DeriveFrom<P0    >
	{
public:
	typedef TypeList<P0, P1> AsTypeList_t;
	};

// !3 == 9, 3x, 0.137us, 2.89x, 32 bytes
template <class P0, class P1, class P2>
class DeriveFrom<P0, P1, P2>
:	public virtual DeriveFrom<    P1, P2>
,	public virtual DeriveFrom<P0,     P2>
,	public virtual DeriveFrom<P0, P1    >
	{
public:
	typedef TypeList<P0, P1, P2> AsTypeList_t;
	};

// !4 == 33, 3.67x, 0.423us, 3.08x, 64 bytes
template <class P0, class P1, class P2, class P3>
class DeriveFrom<P0, P1, P2, P3>
:	public virtual DeriveFrom<    P1, P2, P3>
,	public virtual DeriveFrom<P0,     P2, P3>
,	public virtual DeriveFrom<P0, P1,     P3>
,	public virtual DeriveFrom<P0, P1, P2    >
	{
public:
	typedef TypeList<P0, P1, P2, P3> AsTypeList_t;
	};

// !5 == 153, 5.71x, 1.24us, 2.93x, 128 bytes
template <class P0, class P1, class P2, class P3, class P4>
class DeriveFrom<P0, P1, P2, P3, P4>
:	public virtual DeriveFrom<    P1, P2, P3, P4>
,	public virtual DeriveFrom<P0,     P2, P3, P4>
,	public virtual DeriveFrom<P0, P1,     P3, P4>
,	public virtual DeriveFrom<P0, P1, P2,     P4>
,	public virtual DeriveFrom<P0, P1, P2, P3    >
	{
public:
	typedef TypeList<P0, P1, P2, P3, P4> AsTypeList_t;
	};

// !6 == 873, 6.77x, 4.30us, 3.47x, 256 bytes
template <class P0, class P1, class P2, class P3, class P4, class P5>
class DeriveFrom<P0, P1, P2, P3, P4, P5>
:	public virtual DeriveFrom<    P1, P2, P3, P4, P5>
,	public virtual DeriveFrom<P0,     P2, P3, P4, P5>
,	public virtual DeriveFrom<P0, P1,     P3, P4, P5>
,	public virtual DeriveFrom<P0, P1, P2,     P4, P5>
,	public virtual DeriveFrom<P0, P1, P2, P3,     P5>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4    >
	{
public:
	typedef TypeList<P0, P1, P2, P3, P4, P5> AsTypeList_t;
	};

// Here be dragons. Runtime cost starts to become burdensome.

// !7 == 5913, 13.4us, 3.11x, 512 bytes
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class DeriveFrom<P0, P1, P2, P3, P4, P5, P6>
:	public virtual DeriveFrom<    P1, P2, P3, P4, P5, P6>
,	public virtual DeriveFrom<P0,     P2, P3, P4, P5, P6>
,	public virtual DeriveFrom<P0, P1,     P3, P4, P5, P6>
,	public virtual DeriveFrom<P0, P1, P2,     P4, P5, P6>
,	public virtual DeriveFrom<P0, P1, P2, P3,     P5, P6>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4,     P6>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5    >
	{
public:
	typedef TypeList<P0, P1, P2, P3, P4, P5, P6> AsTypeList_t;
	};

// !8 == 46233, 7.82x, 38.9us, 2.90x, 1024 bytes
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class DeriveFrom<P0, P1, P2, P3, P4, P5, P6, P7>
:	public virtual DeriveFrom<    P1, P2, P3, P4, P5, P6, P7>
,	public virtual DeriveFrom<P0,     P2, P3, P4, P5, P6, P7>
,	public virtual DeriveFrom<P0, P1,     P3, P4, P5, P6, P7>
,	public virtual DeriveFrom<P0, P1, P2,     P4, P5, P6, P7>
,	public virtual DeriveFrom<P0, P1, P2, P3,     P5, P6, P7>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4,     P6, P7>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5,     P7>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5, P6    >
	{
public:
	typedef TypeList<P0, P1, P2, P3, P4, P5, P6, P7> AsTypeList_t;
	};

// !9 == 409113, 8.85x, 105us, 2.70x, 2048 bytes
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
class DeriveFrom<P0, P1, P2, P3, P4, P5, P6, P7, P8>
:	public virtual DeriveFrom<    P1, P2, P3, P4, P5, P6, P7, P8>
,	public virtual DeriveFrom<P0,     P2, P3, P4, P5, P6, P7, P8>
,	public virtual DeriveFrom<P0, P1,     P3, P4, P5, P6, P7, P8>
,	public virtual DeriveFrom<P0, P1, P2,     P4, P5, P6, P7, P8>
,	public virtual DeriveFrom<P0, P1, P2, P3,     P5, P6, P7, P8>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4,     P6, P7, P8>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5,     P7, P8>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5, P6,     P8>
,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5, P6, P7    >
	{
public:
	typedef TypeList<P0, P1, P2, P3, P4, P5, P6, P7, P8> AsTypeList_t;
	};

// =================================================================================================
#pragma mark - DeriveFrom

template <class TL>
struct AsDeriveFrom
	{
	typedef DeriveFrom
		<
		typename TL::T0, typename TL::T1, typename TL::T2, typename TL::T3,
		typename TL::T4, typename TL::T5, typename TL::T6, typename TL::T7,
		typename TL::T8
		> Result_t;
	};

} // namespace ZooLib

#endif // __ZooLib_DeriveFrom_h__
