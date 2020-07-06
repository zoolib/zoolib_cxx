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
	class End = void // Needed so that the 8-param specialization is still considered partial.
	>
class DeriveFrom;

// This template catches cases where we try doing a DeriveFrom of a DeriveFrom, which
// causes weird compilation errors.
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class DeriveFrom<DeriveFrom<P0, P1, P2, P3, P4, P5, P6, P7>>
	{};

// The real suite.
template <class P0>
class DeriveFrom<P0>
:	public virtual P0
	{
public:
	typedef TypeList<P0> AsTypeList_t;
	};

template <class P0, class P1>
class DeriveFrom<P0, P1>
:	public virtual DeriveFrom<    P1>
,	public virtual DeriveFrom<P0    >
	{
public:
	typedef TypeList<P0, P1> AsTypeList_t;
	};

template <class P0, class P1, class P2>
class DeriveFrom<P0, P1, P2>
:	public virtual DeriveFrom<    P1, P2>
,	public virtual DeriveFrom<P0,     P2>
,	public virtual DeriveFrom<P0, P1    >
	{
public:
	typedef TypeList<P0, P1, P2> AsTypeList_t;
	};

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

// !5 == 153. See http://oeis.org/A007489
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

// !6 == 873
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

// !7 == 5913
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

// !8 == 46233
//template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
//class DeriveFrom<P0, P1, P2, P3, P4, P5, P6, P7>
//:	public virtual DeriveFrom<    P1, P2, P3, P4, P5, P6, P7>
//,	public virtual DeriveFrom<P0,     P2, P3, P4, P5, P6, P7>
//,	public virtual DeriveFrom<P0, P1,     P3, P4, P5, P6, P7>
//,	public virtual DeriveFrom<P0, P1, P2,     P4, P5, P6, P7>
//,	public virtual DeriveFrom<P0, P1, P2, P3,     P5, P6, P7>
//,	public virtual DeriveFrom<P0, P1, P2, P3, P4,     P6, P7>
//,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5,     P7>
//,	public virtual DeriveFrom<P0, P1, P2, P3, P4, P5, P6    >
//	{
//public:
//	typedef TypeList<P0, P1, P2, P3, P4, P5, P6, P7> AsTypeList_t;
//	};

// =================================================================================================
#pragma mark - DeriveFrom

template <class TL>
struct AsDeriveFrom
	{
	typedef DeriveFrom
		<
		typename TL::T0, typename TL::T1, typename TL::T2, typename TL::T3,
		typename TL::T4, typename TL::T5, typename TL::T6, typename TL::T7
		> Result_t;
	};

} // namespace ZooLib

#endif // __ZooLib_DeriveFrom_h__
