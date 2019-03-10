/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#ifndef __ZooLib__PullPush_Any_h__
#define __ZooLib__PullPush_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Promise.h"
#include "zoolib/PullPush.h"
#include "zoolib/Val_Any.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

typedef
	Callable<bool(const Any& iAny, const ChanW_PPT& iChanW)> Callable_Any_WriteFilter;

void sFrom_Any_Push_PPT(const Any& iAny,
	const ZRef<Callable_Any_WriteFilter>& iWriteFilter,
	const ChanW_PPT& iChanW);

void sFrom_Any_Push_PPT(const Any& iAny,
	const ChanW_PPT& iChanW);

// --

typedef
	Callable<bool(const PPT& iPPT, const ChanR_PPT& iChanR, Any& oAny)> Callable_Any_ReadFilter;

void sPull_PPT_As_Any(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	const ZRef<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny);

bool sPull_PPT_As_Any(const ChanR_PPT& iChanR,
	const ZRef<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny);

bool sPull_PPT_As_Any(const ChanR_PPT& iChanR,
	Any& oAny);

ZQ<Any> sQAs_Any(const ChanR_PPT& iChanR);
Any sAs_Any(const ChanR_PPT& iChanR);

// --

ZRef<Delivery<Any>> sStartAsyncAs_Any(const ZRef<ChannerR_PPT>& iChannerR);

} // namespace ZooLib

#endif // __ZooLib__PullPush_Any_h__
