/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZRefCount__
#define __ZRefCount__ 1
#include "zconfig.h"

#include "zoolib/ZRef.h"
#include "zoolib/ZThreadSafe.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCounted

class ZRefCounted
	{
public:
	ZRefCounted();
	virtual ~ZRefCounted();

	void Retain() { ZThreadSafe_Inc(fRefCount); }
	void Release();
	int GetRefCount() const;

private:
	ZThreadSafe_t fRefCount;
	};

inline void sRetain(ZRefCounted& iObject)
	{ iObject.Retain(); }

inline void sRelease(ZRefCounted& iObject)
	{ iObject.Release(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCountedWithFinalization

class ZRefCountedWithFinalization
	{
public:
	ZRefCountedWithFinalization();
	virtual ~ZRefCountedWithFinalization();

	virtual void Initialize();
	virtual void Finalize();

	void FinalizationComplete();

	void Retain();
	void Release();
	int GetRefCount() const;

protected:
	int pCOMAddRef();
	int pCOMRelease();

private:
	ZThreadSafe_t fRefCount;
	};

inline void sRetain(ZRefCountedWithFinalization& iObject)
	{ iObject.Retain(); }

inline void sRelease(ZRefCountedWithFinalization& iObject)
	{ iObject.Release(); }

NAMESPACE_ZOOLIB_END

#endif // __ZRefCount__
