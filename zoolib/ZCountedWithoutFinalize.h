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

#ifndef __ZCountedWithoutFinalize_h__
#define __ZCountedWithoutFinalize_h__ 1
#include "zconfig.h"

#include "zoolib/ZThreadSafe.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedWithoutFinalize

class ZCountedWithoutFinalize
	{
public:
	ZCountedWithoutFinalize();
	virtual ~ZCountedWithoutFinalize();

	void Retain() { ZThreadSafe_Inc(fRefCount); }
	void Release();
	bool IsShared() const;
	bool IsReferenced() const;

private:
	ZThreadSafe_t fRefCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sRetain/sRelease for ZCountedWithoutFinalize derivatives

inline void sRetain(ZCountedWithoutFinalize& iObject)
	{ iObject.Retain(); }

inline void sRelease(ZCountedWithoutFinalize& iObject)
	{ iObject.Release(); }

inline void sCheck(ZCountedWithoutFinalize* iP)
	{}

} // namespace ZooLib

#endif // __ZCountedWithoutFinalize_h__
