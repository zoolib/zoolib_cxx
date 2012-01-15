/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZMutexComposite.h"

#include "zoolib/ZCompat_algorithm.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
// MARK: - ZMutexComposite

ZMutexComposite::ZMutexComposite()
:	fIsLocked(false)
	{}

ZMutexComposite::~ZMutexComposite()
	{}

void ZMutexComposite::Add(ZMutexBase& iLock)
	{
	vector<ZMutexBase*>::iterator insertPos = lower_bound(fLocks.begin(), fLocks.end(), &iLock);
	fLocks.insert(insertPos, &iLock);
	}

void ZMutexComposite::Acquire()
	{
	for (vector<ZMutexBase*>::iterator w = fLocks.begin(); w != fLocks.end(); ++w)
		(*w)->Acquire();
	fIsLocked = true;
	}

void ZMutexComposite::Release()
	{
	for (vector<ZMutexBase*>::iterator w = fLocks.begin(); w != fLocks.end(); ++w)
		(*w)->Release();
	fIsLocked = false;
	}

bool ZMutexComposite::IsLocked()
	{ return fIsLocked; }

} // namespace ZooLib
