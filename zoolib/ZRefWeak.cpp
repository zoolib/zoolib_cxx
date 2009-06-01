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
#include "zoolib/ZRefWeak.h"

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

	ZWeakReferee* pLockUse();
	void pUnlock();

	ZMtx fMtx;
	ZWeakReferee* fReferee;
	friend class ZWeakReferee;
	friend class ZRefWeakBase;
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
	const bool inUse = fReferee || this->GetRefCount();
	fMtx.Release();
	if (!inUse)
		delete this;
	}

void ZWeakRefereeProxy::pDetachReferee(ZWeakReferee* iReferee)
	{
	fMtx.Acquire();
	ZAssert(iReferee == fReferee);
	fReferee = nullptr;
	const bool inUse = fReferee || this->GetRefCount();
	fMtx.Release();
	if (!inUse)
		delete this;
	}

ZWeakReferee* ZWeakRefereeProxy::pLockUse()
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
	this->pDetachProxy();
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
#pragma mark * ZRefWeakBase

ZRefWeakBase::ZRefWeakBase()
	{}

ZRefWeakBase::ZRefWeakBase(const ZRefWeakBase& iOther)
:	fWRP(iOther.fWRP)
	{}

ZRefWeakBase::ZRefWeakBase(ZWeakReferee* iWeakReferee)
	{
	if (iWeakReferee)
		fWRP = iWeakReferee->pGetWeakRefereeProxy();
	}

ZRefWeakBase::~ZRefWeakBase()
	{}

void ZRefWeakBase::AssignFrom(const ZRefWeakBase& iOther)
	{ fWRP = iOther.fWRP; }

void ZRefWeakBase::AssignFrom(ZWeakReferee* iWeakReferee)
	{
	if (iWeakReferee)
		fWRP = iWeakReferee->pGetWeakRefereeProxy();
	else
		fWRP.Clear();
	}

void ZRefWeakBase::Clear()
	{ fWRP.Clear(); }

ZWeakReferee* ZRefWeakBase::LockUse() const
	{
	if (fWRP)
		return fWRP->pLockUse();
	return nullptr;
	}

void ZRefWeakBase::Unlock() const
	{ fWRP->pUnlock(); }

NAMESPACE_ZOOLIB_END
