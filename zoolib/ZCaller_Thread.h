/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZCaller_Thread__
#define __ZCaller_Thread__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCallOnNewThread

template <class T>
void sCallOnNewThread(ZRef<ZCallable<T(void)> > iCallable)
	{ ZThread::sCreate_T<ZRef<ZCallable<T(void)> > >(sCallReturnVoid<T>, iCallable); }

inline
void sCallOnNewThread(ZRef<ZCallable_Void> iCallable)
	{ ZThread::sCreate_T<ZRef<ZCallable_Void> >(sCall<void>, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCaller_Thread

class ZCaller_Thread
:	public ZCaller
	{
public:
// From ZCaller
	virtual void Call(ZRef<ZCallable_Void> iCallable)
		{ sCallOnNewThread(iCallable); }
	};

} // namespace ZooLib

#endif // __ZCaller_Thread__
