/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZRef_Counted__
#define __ZRef_Counted__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZThreadSafe.h"

// ZRef_Counted does not itself use the definitions in
// ZRef.h, but most/all users of ZRef_Counted do.
#include "zoolib/ZRef.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor;

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
#pragma mark * ZRefCountedWithFinalizeBase

class ZRefCountedWithFinalizeBase
	{
public:
	ZRefCountedWithFinalizeBase();
	virtual ~ZRefCountedWithFinalizeBase();

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
	ZAtomic_t fRefCount;
	};

inline void sRetain(ZRefCountedWithFinalizeBase& iObject)
	{ iObject.Retain(); }

inline void sRelease(ZRefCountedWithFinalizeBase& iObject)
	{ iObject.Release(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCountedWithFinalize

class ZRefCountedWithFinalize : public virtual ZRefCountedWithFinalizeBase
	{
public:
	ZRefCountedWithFinalize();
	virtual ~ZRefCountedWithFinalize();

// Our protocol
	virtual bool Accept(ZVisitor& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor

class ZVisitor
	{
public:
	virtual bool Visit(ZRef<ZRefCountedWithFinalize> iRep);
	};

NAMESPACE_ZOOLIB_END

#endif // __ZRef_Counted__
