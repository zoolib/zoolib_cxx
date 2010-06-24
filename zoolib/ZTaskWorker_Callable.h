/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZTaskWorker_Callable__
#define __ZTaskWorker_Callable__ 1
#include "zconfig.h"

#include "zoolib/ZTaskWorker.h"
#include "zoolib/ZWorker_Callable.h"

namespace ZooLib {


// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T0

class ZTaskWorker_Callable_T0
:	public ZTaskWorker
,	public ZWorker_Callable_T0
	{
public:
	ZTaskWorker_Callable_T0(ZRef<ZTaskMaster> iTaskMaster, ZRef<Callable_t> iCallable)
	:	ZTaskWorker(iTaskMaster)
	,	ZWorker_Callable_T0(iCallable)
		{}
	};

#if 0
// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T1

template <class P0>
class ZTaskWorker_T1
:	public ZTaskWorker
,	public ZWorker_Callable_T1
	{
public:
	ZTaskWorker_T1(
		ZRef<ZTaskMaster> iTaskMaster,
		ZRef<Callable_t> iCallable,
		const P0& iP0)
	:	ZTaskWorker(iTaskMaster)
	,	ZWorker_Callable_T1(iCallable, iP0)
		{}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T2

template <class P0, class P1>
class ZTaskWorker_T2
:	public ZTaskWorker
,	public ZWorker_Callable_T2
	{
public:
	ZTaskWorker_T2(
		ZRef<ZTaskMaster> iTaskMaster,
		ZRef<Callable_t> iCallable,
		const P0& iP0, const P1& iP1)
	:	ZTaskWorker(iTaskMaster)
	,	ZWorker_Callable_T2(iCallable, iP0, iP1)
		{}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T3

template <class P0, class P1, class P2>
class ZTaskWorker_T3
:	public ZTaskWorker
,	public ZWorker_Callable_T3
	{
public:
	ZTaskWorker_T3(
		ZRef<ZTaskMaster> iTaskMaster,
		ZRef<Callable_t> iCallable,
		const P0& iP0, const P1& iP1, const P2& iP2)
	:	ZTaskWorker(iTaskMaster)
	,	ZWorker_Callable_T3(iCallable, iP0, iP1, iP2)
		{}
	};

#endif

} // namespace ZooLib

#endif // __ZTaskWorker_Callable__
