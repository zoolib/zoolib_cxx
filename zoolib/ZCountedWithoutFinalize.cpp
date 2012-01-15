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

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCountedWithoutFinalize

ZCountedWithoutFinalize::ZCountedWithoutFinalize()
:	fRefCount(0)
	{}

ZCountedWithoutFinalize::~ZCountedWithoutFinalize()
	{
	ZAssertStopf(1, ZThreadSafe_Get(fRefCount) == 0,
		("Non-zero refcount at destruction, it is %d", ZThreadSafe_Get(fRefCount)));
	}

void ZCountedWithoutFinalize::Release()
	{
	if (ZThreadSafe_DecAndTest(fRefCount))
		delete this;
	}

bool ZCountedWithoutFinalize::IsShared() const
	{ return ZThreadSafe_Get(fRefCount) > 1; }

bool ZCountedWithoutFinalize::IsReferenced() const
	{ return ZThreadSafe_Get(fRefCount) > 0; }

} // namespace ZooLib
