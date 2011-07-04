/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZWorkerRunner_Crowd__
#define __ZWorkerRunner_Crowd__ 1
#include "zconfig.h"

#include "zoolib/ZWorkerRunner.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Crowd

class ZWorkerRunner_Crowd
:	public ZWorkerRunner
	{
public:
	virtual void Attach(ZRef<ZWorker> iWorker) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunnerRegistration

class ZWorkerRunnerRegistration
	{
public:
	ZWorkerRunnerRegistration(ZRef<ZWorkerRunner_Crowd> iWR);
	~ZWorkerRunnerRegistration();

	static ZRef<ZWorkerRunner_Crowd> sCurrent();

private:
	ZRef<ZWorkerRunner_Crowd> fCurrent;
	ZRef<ZWorkerRunner_Crowd> fPrior;
	};

} // namespace ZooLib

#endif // __ZWorkerRunner_Crowd__
