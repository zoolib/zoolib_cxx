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

#ifndef __ZTaskWorker_Callable__
#define __ZTaskWorker_Callable__ 1
#include "zconfig.h"

#include "zoolib/ZTaskWorker.h"
#include "zoolib/ZWorker_Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_T0

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

inline ZRef<ZTaskWorker> MakeTaskWorker(ZRef<ZTaskMaster> iTaskMaster,
	const ZRef<ZCallable_R1<bool, ZRef<ZWorker> > >& iCallable)
	{ return new ZTaskWorker_Callable_T0(iTaskMaster, iCallable); }

} // namespace ZooLib

#endif // __ZTaskWorker_Callable__
