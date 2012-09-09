/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZCaller_Thread.h"
#include "zoolib/ZCallOnNewThread.h"
#include "zoolib/ZSingleton.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCaller_Thread

bool ZCaller_Thread::Queue(const ZRef<ZCallable_Void>& iCallable)
	{
	if (iCallable)
		{
		sCallOnNewThread(iCallable);
		return true;
		}
	return false;
	}

ZRef<ZCaller_Thread> ZCaller_Thread::sGet()
	{ return sSingleton<ZRef<ZCaller_Thread> >(); }

} // namespace ZooLib
