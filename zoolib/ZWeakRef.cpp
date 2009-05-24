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

#include "zoolib/ZDebug.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZThreadImp.h"
#include "zoolib/ZWeakRef.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRefereeProxy

class ZWeakRefereeProxy : public ZRefCountedWithFinalize
	{
public:
	ZWeakRefereeProxy(ZWeakReferee* iReferee);
	virtual ~ZWeakRefereeProxy();

// From ZRefCountedWithFinalize
	virtual void Finalize();

private:
	void pDetachReferee(ZWeakReferee* iReferee);

	ZWeakReferee* pLockGet();
	void pUnlock();

	ZMtx fMtx;
	ZWeakReferee* fReferee;
	friend class ZWeakReferee;
	friend class ZWeakRefBase;
	};

ZWeakRefereeProxy::ZWeakRefereeProxy(ZWeakReferee* iReferee)
:	fReferee(iReferee)
	{}

ZWeakRefereeProxy::~ZWeakRefereeProxy()
	{
	ZAssert(!fReferee);
	}

void ZWeakRefereeProxy::Finalize()
	{
	fMtx.Acquire();
	this->FinalizationComplete();
	bool inUse = fReferee || this->GetRefCount();
	fMtx.Release();
	if (!inUse)
		delete this;
	}

void ZWeakRefereeProxy::pDetachReferee(ZWeakReferee* iReferee)
	{
	fMtx.Acquire();
	ZAssert(iReferee == fReferee);
	fReferee = nullptr;
	if (this->GetRefCount())
		{
		fMtx.Release();
		}
	else
		{
		fMtx.Release();
		delete this;
		}
	}

ZWeakReferee* ZWeakRefereeProxy::pLockGet()
	{
	fMtx.Acquire();
	if (fReferee)
		return fReferee;
	fMtx.Release();
	return nullptr;
	}

void ZWeakRefereeProxy::pUnlock()
	{ fMtx.Release(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakReferee

ZWeakReferee::ZWeakReferee()
:	fWRP(nullptr)
	{}

ZWeakReferee::~ZWeakReferee()
	{
	// Finalize must have called pDetachProxy.
	ZAssert(!fWRP);
	}

void ZWeakReferee::pDetachProxy()
	{
	if (ZWeakRefereeProxy* theWRP = fWRP)
		{
		fWRP = nullptr;
		theWRP->pDetachReferee(this);
		}
	}

ZWeakRefereeProxy* ZWeakReferee::pGetWeakRefereeProxy()
	{
	if (!fWRP)
		fWRP = new ZWeakRefereeProxy(this);
	return fWRP;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRefBase

ZWeakRefBase::ZWeakRefBase()
	{}

ZWeakRefBase::ZWeakRefBase(const ZWeakRefBase& iOther)
:	fWRP(iOther.fWRP)
	{}

ZWeakRefBase::ZWeakRefBase(ZWeakReferee* iWeakReferee)
	{
	if (iWeakReferee)
		fWRP = iWeakReferee->pGetWeakRefereeProxy();
	}

ZWeakRefBase::~ZWeakRefBase()
	{}

void ZWeakRefBase::AssignFrom(const ZWeakRefBase& iOther)
	{ fWRP = iOther.fWRP; }

void ZWeakRefBase::AssignFrom(ZWeakReferee* iWeakReferee)
	{
	if (iWeakReferee)
		fWRP = iWeakReferee->pGetWeakRefereeProxy();
	else
		fWRP.Clear();
	}

void ZWeakRefBase::Clear()
	{ fWRP.Clear(); }

ZWeakReferee* ZWeakRefBase::LockGet() const
	{
	if (fWRP)
		return fWRP->pLockGet();
	return nullptr;
	}

void ZWeakRefBase::Unlock() const
	{ fWRP->pUnlock(); }

// =================================================================================================
#pragma mark -
#pragma mark * test

class Test1 : public ZRefCountedWithFinalize, public ZWeakReferee
	{};

class Test2 : public ZRefCountedWithFinalize, public ZWeakReferee
	{};

static void sTest(ZRef<Test1> iRef1, ZRef<Test2> iRef2, ZRef<Test1> iWeak1)
	{
	ZRef<Test1> theRef1;
	theRef1 = iWeak1;

	ZWeakRef<Test1> theWeak1 = iRef1;
	theWeak1 = theRef1;
	theRef1 = theWeak1;
	theWeak1 = theWeak1;
//	theWeak1 = iRef1;
//	theWeak1 = iRef2;
	ZWeakRef<Test2> theWeak2;
//	theWeak2 = iRef1;
//	theWeak2 = iRef2;

#if 0
	ZWeak<Test1> oWeak1_1 = theWeak2;
	oWeak1_1 = theWeak1;
//	oWeak1_1 = theWeak2;
#endif
	}

NAMESPACE_ZOOLIB_END
#if 0
class A
	{
	int dummy;
	};

class B : public virtual A
	{
	int dummy;
	};

class C1 : public B
	{};

class C2 : public B
	{};

class C3 : public B
	{};

class D : public C1, public C2, public C3
	{};


static void sTest(A* iThing)
	{
	B* theB = static_cast<B*>(iThing);
//	D* thing = static_cast<D*>(iThing);
//	D* thing = static_cast<D*>(iThing);
	}
#endif
