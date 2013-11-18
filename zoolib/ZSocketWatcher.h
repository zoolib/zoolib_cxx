/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#ifndef __ZSocketWatcher_h__
#define __ZSocketWatcher_h__
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread.h"

#include "zoolib/ZCallable.h"

#include <set>

namespace ZooLib {

// =================================================================================================
// MARK: - ZSocketWatcher

class ZSocketWatcher
:	NonCopyable
	{
public:
	ZSocketWatcher();

// -----

	bool QInsert(int iSocket, const ZRef<ZCallable_Void>& iCallable);
	bool QErase(int iSocket, const ZRef<ZCallable_Void>& iCallable);

// -----

private:
	void pRun();
	static void spRun(ZSocketWatcher*);

	ZMtx fMtx;
	ZCnd fCnd;

	bool fThreadRunning;
	typedef std::set<std::pair<int,ZRef<ZCallable_Void> > > Set_t;
	Set_t fSet;
	};

} // namespace ZooLib

#endif // __ZSocketWatcher_h__
