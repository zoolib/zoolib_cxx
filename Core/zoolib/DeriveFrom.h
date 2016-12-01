/* -------------------------------------------------------------------------------------------------
Copyright (c) 2016 Andrew Green
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

#ifndef __ZooLib_DeriveFrom_h__
#define __ZooLib_DeriveFrom_h__

#include "zconfig.h"

#include "zoolib/TypeList.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark DeriveFrom

template <
	class P0 = null_t, class P1 = null_t, class P2 = null_t, class P3 = null_t,
	class P4 = null_t, class P5 = null_t, class P6 = null_t, class P7 = null_t,
	class End = null_t // Needed so that the 8-param specialization is still considered partial.
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

// =================================================================================================
#pragma mark -
#pragma mark DeriveFrom

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
